#ifndef GENERATE_HEADER_H
#define GENERATE_HEADER_H

#include "common.h"
typedef enum Type {
    TYPE_UNKNOWN = 0,
    TYPE_BOOL,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64
} Type_t;

Type_t type_from_num_bits_and_sign(size_t num_bits, bool sign);
char* type_str(Type_t type);
char* type_format_specifier(Type_t type);

int generate_header(const message_t* msg);
int generate_source(const message_t* msg);
#endif