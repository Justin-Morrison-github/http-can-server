#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h> // For PATH_MAX
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_SIGNAL_NAME 64
#define MAX_MESSAGE_NAME 64

typedef struct signal {
    char name[MAX_SIGNAL_NAME];
    size_t num_bits;
    bool sign;
    struct signal* next;
    struct signal* tail;
} signal_t;

typedef struct message_header {
    char name[MAX_MESSAGE_NAME];
    uint32_t id;
} message_header_t;

typedef struct message {
    message_header_t header;
    signal_t* signals;
    size_t num_signals; 
    struct message* next;
    struct message* tail;
} message_t;

void* malloc_or_fail(size_t size){
    void* p = malloc(size);
    if (p == NULL) {
        perror("malloc");
        exit(1);
    }
    return p;
}

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

signal_t* parse_signal_line(char* line) {
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

    signal_t* signal_p = (signal_t*) malloc_or_fail(sizeof(signal_t));

    signal_p->num_bits = strtol(num_bits_str, NULL, 10);
    if (strcmp(sign_str, "-") == 0) {
        signal_p->sign = true;
    } else {
        signal_p->sign = false;
    }
    
    memcpy(signal_p->name, name, strlen(name) + 1);
    return signal_p;
}

void camel_to_snake_case(char* name, char* output){

    int j = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        char c = name[i];
        if (isupper(c)) {
            if (i > 0) {
                output[j++] = '_';
            }
            output[j++] = tolower(c);
        } else {
            output[j++] = c;
        }
    }
    output[j] = '\0';
}

void to_upper_str(char* input, char* output){
    int i;
    for (i = 0; input[i] != '\0'; i++) {
        output[i] = toupper(input[i]);
    }
    output[i] = '\0';
}

int main(void) {
    FILE* fp = fopen("./src/can/codec/test.can", "r");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    char line_buf[1000];

    message_t* msg_dummy = (message_t*) malloc_or_fail(sizeof(message_t));

    msg_dummy->next = NULL;
    msg_dummy->tail = msg_dummy;

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
            message_t* message_p = (message_t*) malloc_or_fail(sizeof(message_t));
            message_p->num_signals = 0;
            message_p->header = parse_message_header_line(line_buf);
            message_p->signals = (signal_t*) malloc_or_fail(sizeof(signal_t));
            message_p->signals->next = NULL;
            message_p->signals->tail = message_p->signals;

            // Parse inner signal lines
            while (fgets(line_buf, 100, fp) != NULL) {
                strip(line_buf);

                if (is_single_message_footer(line_buf)) {
                    break;
                }
                message_p->num_signals++;

                signal_t* signal = parse_signal_line(line_buf);
                
                // Set new node to point to NULL (end of list)
                signal->next = NULL;

                // Set tail to point at new node
                message_p->signals->tail->next = signal;

                // Update tail itself to point at new node
                message_p->signals->tail = signal;
            }

            // Set new node to point to NULL (end of list)
            message_p->next = NULL;

            // Set tail to point at new node
            msg_dummy->tail->next = message_p;

            // Update tail itself to point at new node
            msg_dummy->tail = message_p;
        }
    }
    
    //Prints all messages and signals
    for (message_t* curr = msg_dummy->next; curr != NULL; curr = curr->next) {
        printf("%s : 0x%03X\n", curr->header.name, curr->header.id);
        
        char snake_case_name[100];
        camel_to_snake_case(curr->header.name, snake_case_name);
        
        char parent_dir_path[PATH_MAX];
        snprintf(parent_dir_path, PATH_MAX, "./src/can/msg/%s", snake_case_name);

        if (mkdir(parent_dir_path, 0755) != 0) {
            // Fine if directory already exists
            if (errno != EEXIST) {
                perror("Error creating directory");
                exit(1);
            }
        }
        
        char file_path_h[PATH_MAX];
        snprintf(file_path_h, PATH_MAX, "./src/can/msg/%s/%s.h", snake_case_name, snake_case_name);

        char file_path_c[PATH_MAX];
        snprintf(file_path_c, PATH_MAX, "./src/can/msg/%s/%s.c", snake_case_name, snake_case_name);

        FILE* h_fp = fopen(file_path_h, "w");
        if (h_fp == NULL) {
            perror("fopen");
            return 1;
        }

        // FILE* c_fp = fopen(file_path_c, "w");
        // if (c_fp == NULL) {
        //     perror("fopen");
        //     return 1;
        // }
        
        char ifndef_str[120];
        char define_str[120];
        char endif_str[10];
        char upper_case_name[100];
        to_upper_str(snake_case_name, upper_case_name);
        snprintf(ifndef_str, 120, "#ifndef %s_H\n", upper_case_name);
        snprintf(define_str, 120, "#define %s_H\n", upper_case_name);
        snprintf(endif_str, 120, "#endif\n");
        fwrite(ifndef_str, sizeof(char), strlen(ifndef_str), h_fp);
        fwrite(define_str, sizeof(char), strlen(define_str), h_fp);
        fwrite(endif_str, sizeof(char), strlen(endif_str), h_fp);

        for (signal_t* curr_sig = curr->signals->next; curr_sig != NULL; curr_sig = curr_sig->next) {
            printf("    %s | %ld | %s\n", curr_sig->name, curr_sig->num_bits, curr_sig->sign ? "-" : "+");
        }
    }
    

    // Free messages that were malloc'd
    message_t *next_msg = NULL;
    for (message_t* curr_msg = msg_dummy->next; curr_msg != NULL; curr_msg=next_msg) {
        next_msg = curr_msg->next;

        signal_t *next_sig = NULL;
        for (signal_t* curr_sig = curr_msg->signals; curr_sig != NULL; curr_sig=next_sig) {
            next_sig = curr_sig->next;
            free(curr_sig);
        }

        free(curr_msg);
    }

    free(msg_dummy);

    fclose(fp);
    return 0;
}