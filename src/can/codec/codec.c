#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIGNAL_NAME 64
#define MAX_MESSAGE_NAME 64

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

#define MESSAGES_HEADER "<MESSAGES"
bool is_messages_header(char* str){
    return strcmp(str, MESSAGES_HEADER) == 0;
}

#define MESSAGES_FOOTER "MESSAGES>"
bool is_messages_footer(char* str){
    return strcmp(str, MESSAGES_FOOTER) == 0;
}

#define MESSAGE_HEADER_SUB_STR "<MESSAGE"
bool is_single_message_header(char* str){
    return strstr(str, MESSAGE_HEADER_SUB_STR) != NULL;
}

#define MESSAGE_FOOTER "MESSAGE>"
bool is_single_message_footer(char* str){
    return strcmp(str, MESSAGE_FOOTER) == 0;
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

    // Left will equal str[0..i-1] inclusive
    size_t left_len = i; // -1 and +1 cancel out
    memcpy(left, str, left_len);
    left[left_len] = '\0';

    // Right will equal str[i+1..len+1] inclusive (include null terminator)
    size_t right_len = len - left_len;
    memcpy(right, str + i + 1, right_len);
}

message_header_t parse_message_header_line(char* line) {
    char left[MAX_MESSAGE_NAME];
    char right[MAX_MESSAGE_NAME];
    split(line, ':', left, right);
    strip(left);
    strip(right);

    char msg_name[MAX_MESSAGE_NAME];
    char msg_id[MAX_MESSAGE_NAME];
    split(right, ' ', msg_name, msg_id);
    strip(msg_name);
    strip(msg_id);

    uint32_t id;

    message_header_t msg_header;
    msg_header.id = hex_id_to_u32(msg_id);
    memcpy(msg_header.name, msg_name, strlen(msg_name) + 1);
    return msg_header;
}

signal_t parse_signal_line(char* line) {
    // Line is already stripped of end whitespace
    // Just need to split on ":", and strip whitespace again
    // Left of : is Name, right of : is ID 
    char name[64];
    char bits_and_sign[64];

    split(line, ':', name, bits_and_sign);
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

    char line_buf[1000];

    while (fgets(line_buf, 100, fp) != NULL) {
        strip(line_buf);
        if(strcmp(line_buf, "\0") == 0) {
            continue;  // Ignore Empty Lines
        }

        if(is_messages_header(line_buf)) {
            continue;  // Ignore message header line
        }

        if (is_messages_footer(line_buf)) {
            break;
        }

        char signal_name[MAX_SIGNAL_NAME];
        char signal_id[MAX_SIGNAL_NAME];

        if(is_single_message_header(line_buf)) {
            message_header_t msg_header = parse_message_header_line(line_buf);
            printf("%s:0x%03X\n", msg_header.name, msg_header.id);
            
            // Parse inner signal lines
            while (fgets(line_buf, 100, fp) != NULL) {
                strip(line_buf);

                if (is_single_message_footer(line_buf)) {
                    break;
                }

                signal_t signal = parse_signal_line(line_buf);
                printf("  %s | %ld | %s\n", signal.name, signal.num_bits, signal.sign ? "-" : "+");
            }
        }
    }
    
    fclose(fp);
    return 0;
}