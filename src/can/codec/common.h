#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
    uint16_t interval_ms;
} message_header_t;

typedef struct message {
    message_header_t header;
    signal_t* signals;
    size_t num_signals; 
    struct message* next;
    struct message* tail;
    size_t size_bytes;
} message_t;


void camel_to_snake_case(char* name, char* output);
void to_upper_str(char* input, char* output);
char* type_from_num_bits_and_sign(size_t num_bits, bool is_signed);
char* byte_mask_from_num_bits(size_t num_bits);

#endif