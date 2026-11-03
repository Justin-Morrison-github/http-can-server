#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <string.h>
#include <ctype.h>

void split(char* str, char delimiter, char* left, char* right);
void strip(char* str);
void read_until(char* input, char* sentinel, char* output, size_t output_size);
#endif