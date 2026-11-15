#include <stdio.h>
#include <ctype.h>
#include "common.h"

void camel_to_snake_case(const char* name, char* output){

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

void to_upper_str(const char* input, char* output){
    int i;
    for (i = 0; input[i] != '\0'; i++) {
        output[i] = toupper(input[i]);
    }
    output[i] = '\0';
}


char* byte_mask_from_num_bits(size_t num_bits) {
    switch (num_bits)
    {
        case 1: return "0x01";
        case 2: return "0x03";
        case 3: return "0x07";
        case 4: return "0x0F";
        case 5: return "0x1F";
        case 6: return "0x3F";
        case 7: return "0x7F";
        case 8: return "0xFF";
        default: return "0x00";
    }
}