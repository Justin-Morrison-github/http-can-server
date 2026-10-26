#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NODE_NAME 64
#define MAX_SIGNAL_NAME 64
#define MAX_MESSAGE_NAME 64

typedef struct node {
    char name[MAX_NODE_NAME];
    uint32_t id;
} node_t;

typedef struct codec {
    node_t *nodes;
} codec_t;

typedef struct signal {
    char name[MAX_SIGNAL_NAME];
    size_t num_bits;
    bool sign;
} signal_t;

typedef struct message_header {
    char name[MAX_MESSAGE_NAME];
    uint32_t id;
} message_header_t;

typedef struct message {
    message_header_t header;
    signal_t* signals;
    size_t num_signals; 
} message_t;




void strip(char* str) {
    if (str == NULL || strlen(str) == 0) {
        return;
    }

    size_t start = 0;
    size_t end = strlen(str);

    while (start < end && isspace(str[start])) {
        start++; 
    }

    while (end > start && isspace(str[end - 1])){
        end--;
    }
    str[end] = '\0';

    size_t len = end - start;
    memmove(str, str + start, len + 1);
}

#define NODES_START "NODES>"
bool is_nodes_start(char* str){
    return strcmp(str, NODES_START) == 0;
}

#define NODES_END "<NODES"
bool is_nodes_end(char* str){
    return strcmp(str, NODES_END) == 0;
}


#define MESSAGES_START "MESSAGES>"
bool is_messages_header(char* str){
    return strcmp(str, MESSAGES_START) == 0;
}

#define MESSAGES_END "<MESSAGES"
bool is_messages_footer(char* str){
    return strcmp(str, MESSAGES_END) == 0;
}

uint32_t hex_id_to_u32(char hex_id[5]) {
    char* endptr;
    long parsed = strtol(hex_id, &endptr, 16);

    if (hex_id == endptr) {
        printf("Parsing failed: No valid hex digits found.\n");
        return 0;
    }

    return (uint32_t) parsed;
}

void split(char* str, char delimiter, char* left, char* right) {
    size_t len = strlen(str);
    if (str == NULL || left== NULL || right==NULL || len == 0) {
        return;
    }

    size_t i;

    for (i = 0; i < len; i++) {
        if (str[i] == delimiter) {
            // Now i is the index of the delimiter
            break;
        }
    }

    // TEST: 0x555
    // : = x = index 4
    // TEST = [0,1,2,3] = [0,x-1]
    // ' 0x555' = [5,6,7,8,9,10] = [i+1, strlen = 1]

    // Left will equal str[0..i-1] inclusive
    size_t left_len = i; // -1 and +1 cancel out
    memcpy(left, str, left_len);
    left[left_len] = '\0';

    // Right will equal str[i+1..len+1] inclusive (include null terminator)
    size_t right_len = len - left_len;
    memcpy(right, str + i + 1, right_len);
}



node_t parse_node_line(char* node_line) {
    // Line is already stripped of end whitespace
    // Just need to split on ":", and strip whitespace again
    // Left of : is Name, right of : is ID 
    char name[64];
    char id_str[64]; // A valid ID string will always be at most 5 chars + null terminator, '0xXYZ'. But account for whitespace around

    split(node_line, ':', name, id_str);
    strip(name);
    strip(id_str);

    node_t node;
    node.id = hex_id_to_u32(id_str);
    memcpy(node.name, name, strlen(name) + 1);
    return node;
}

message_header_t parse_message_header_line(char* node_line) {
    // Line is already stripped of end whitespace
    // Just need to split on ":", and strip whitespace again
    // Left of : is Name, right of : is ID 
    char name[64];
    char id_str[64];
    uint32_t id;

    split(node_line, ':', id_str, name);
    strip(name);
    strip(id_str);


    message_header_t message;
    message.id = hex_id_to_u32(id_str);
    memcpy(message.name, name, strlen(name) + 1);
    return message;
}

signal_t parse_signal_line(char* node_line) {
    // Line is already stripped of end whitespace
    // Just need to split on ":", and strip whitespace again
    // Left of : is Name, right of : is ID 
    char name[64];
    char bits_and_sign[64];

    split(node_line, ':', name, bits_and_sign);
    strip(name);
    strip(bits_and_sign);

    char num_bits_str[32];
    char sign_str[32];

    split(bits_and_sign, '|', num_bits_str, sign_str);
    strip(num_bits_str);
    strip(sign_str);

    signal_t signal;
    signal.num_bits = strtol(num_bits_str, NULL, 10);
    if (strcmp(sign_str, "-") == 0) {
        signal.sign = true;
    } else {
        signal.sign = false;
    }
    
    memcpy(signal.name, name, strlen(name) + 1);
    return signal;
}

int main(void) {

    FILE* fp = fopen("./src/can/codec/test.can", "r");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }
    bool nodes_start = false;
    bool nodes_end = false;

    bool msg_header_seen = false;
    bool msg_footer_seen = false;
    bool message_header_started = false;
    bool signal_started = false;

    char line_buf[1000];
    while (fgets(line_buf, 100, fp) != NULL) {
        strip(line_buf);
        if(strcmp(line_buf, "\0") == 0) {
            // Ignore Empty Lines
            continue;
        }

        if (!nodes_end) {
            if (!nodes_start && is_nodes_start(line_buf)) {
                printf("NODES:\n");
                nodes_start = true;
                continue;
            }
            
            if (nodes_start) {
                if (is_nodes_end(line_buf)) {
                    nodes_end = true;
                } else {
                    // Parse Node line
                    node_t node = parse_node_line(line_buf);
                    printf("  NODE: %s(0x%X)\n", node.name, node.id);
                }
            }
        }

        if (!msg_footer_seen) {
            if (!msg_header_seen && is_messages_header(line_buf)) {
                printf("MESSAGES:\n");
                msg_header_seen = true;
                continue;
            }
            
            if (msg_header_seen) {
                if (is_messages_footer(line_buf)) {
                    msg_footer_seen = true;
                } else {
                    // Parse Message line
                    if (!message_header_started) {
                        /* code */
                        message_header_t header = parse_message_header_line(line_buf);
                        printf("  HEADER: %s(0x%X)\n", header.name, header.id);
                        message_header_started = true;
                    } else {
                        signal_t signal = parse_signal_line(line_buf);
                        printf("  SIGNAL: %s(%ld%c)\n", signal.name, signal.num_bits, signal.sign ? '-'  : '+');

                    }
                }
            }
        }
    }
    
    fclose(fp);
    return 0;
}