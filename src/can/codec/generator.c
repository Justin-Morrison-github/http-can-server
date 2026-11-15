#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include "common.h"
#include "generator.h"
#include <inttypes.h>

Type_t type_from_num_bits_and_sign(size_t num_bits, bool is_signed) {
    
    if (num_bits < 1 || num_bits > 64) {
        fprintf(stderr, "invalid num_bits for type");
        return TYPE_UNKNOWN;
    }
    
    if (num_bits == 1) {
        return TYPE_BOOL;
    }

    if (is_signed) {
        if (num_bits <= 8) {
            return TYPE_I8;
        } else if (num_bits <= 16) {
            return TYPE_I16;
        } else if (num_bits <= 32) {
            return TYPE_I32;
        } else if (num_bits <= 64) {
            return TYPE_I64;
        }
    } else {
        if (num_bits <= 8) {
            return TYPE_U8;
        } else if (num_bits <= 16) {
            return TYPE_U16;
        } else if (num_bits <= 32) {
            return TYPE_U32;
        } else if (num_bits <= 64) {
            return TYPE_U64;
        }
    }

    return TYPE_UNKNOWN;
}

char* type_str(Type_t type) {
    switch (type) {
        case TYPE_BOOL: return "bool";
        case TYPE_U8: return "uint8_t";
        case TYPE_U16: return "uint16_t";
        case TYPE_U32: return "uint32_t";
        case TYPE_U64: return "uint64_t";
        case TYPE_I8: return "int8_t";
        case TYPE_I16: return "int16_t";
        case TYPE_I32: return "int32_t";
        case TYPE_I64: return "int64_t";
        case TYPE_UNKNOWN: // Purposeful fallthrough
        default: return "?";
    }
}

char* type_format_specifier_actual(Type_t type) {
    switch (type) {
        case TYPE_BOOL: return "%s";        // Because str_bool is used
        case TYPE_U8: return "%\" PRIu8 \"";
        case TYPE_U16: return "%\" PRIu16 \"";
        case TYPE_U32: return "%\" PRIu32 \"";
        case TYPE_U64: return "%\" PRIu64 \"";
        case TYPE_I8: return "%\" PRId8 \"";
        case TYPE_I16: return "%\" PRId16 \"";
        case TYPE_I32: return "%\" PRId32 \"";
        case TYPE_I64: return "%\" PRId64 \"";
        case TYPE_UNKNOWN: // Purposeful fallthrough
        default: return "?";
    }
}

char* type_format_specifier(Type_t type) {
    switch (type) {
        case TYPE_BOOL: return "%s";        // Because str_bool is used
        case TYPE_U8: return "%u";
        case TYPE_U16: return "%u";
        case TYPE_U32: return "%u";
        case TYPE_U64: return "%lu";
        case TYPE_I8: return "%d";
        case TYPE_I16: return "%d";
        case TYPE_I32: return "%d";
        case TYPE_I64: return "%ld";
        case TYPE_UNKNOWN: // Purposeful fallthrough
        default: return "%s";
    }
}

// The maximum number of string characters needed to encode each type
int type_str_max_len(Type_t type) {
    switch (type) {
        case TYPE_BOOL: return 5; // For "False"
        case TYPE_U8: return 3;
        case TYPE_U16: return 5;
        case TYPE_U32: return 10;
        case TYPE_U64: return 20;
        case TYPE_I8: return 4;
        case TYPE_I16: return 6;
        case TYPE_I32: return 11;
        case TYPE_I64: return 20;
        case TYPE_UNKNOWN: // Purposeful fallthrough
        default: return 1; // For the '?'
    }
}

int generate_header(const message_t* msg) { 
    char snake_case_name[100];
    camel_to_snake_case(msg->header.name, snake_case_name);
    
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

    FILE* fp = fopen(file_path_h, "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    size_t LINE_BUF_SIZE = 1000;
    char line_buf[LINE_BUF_SIZE];
    char upper_case_name[100];
    to_upper_str(snake_case_name, upper_case_name);
    
    // Header guard + includes
    fprintf(fp, 
        "#ifndef %s_H\n"
        "#define %s_H\n\n"
        "#include <stdint.h>\n"
        "#include <stdbool.h>\n"
        "#include \"../../utils.h\"\n\n", 
        upper_case_name, 
        upper_case_name
    );    
    
    // Defines
    fprintf(fp, 
        "#define %s_ID 0x%03X\n"
        "#define %s_LEN %ld\n" 
        "#define %s_INTERVAL_MS %d\n"
        "#define %s_NAME \"%s\"\n",
        upper_case_name, msg->header.id,
        upper_case_name, msg->size_bytes,
        upper_case_name, msg->header.interval_ms,
        upper_case_name, msg->header.name
    );

    int msg_str_len = 0;
    // A line like this:
    // snprintf(out, "TEMPERATURE_SENSOR: {time_ms: %lu, temp_c: %d}", msg.time_ms, msg.temp_c);

    // would turn into (at worst)
    // "TEMPERATURE_SENSOR: {time_ms: 18446744073709551615, temp_c: -128}"
    
    msg_str_len += strlen(upper_case_name);
    msg_str_len += 5; // Account for ":", space, opening "{", closing "}", and null terminator

    for (signal_t* signal = msg->signals->next; signal != NULL; signal = signal->next) {
        msg_str_len += strlen(signal->name); // Account for message name

        Type_t type = type_from_num_bits_and_sign(signal->num_bits, signal->sign);
        msg_str_len += type_str_max_len(type); // Account for max size of type
    }

    msg_str_len += 2 * (msg->num_signals);      // Account for the ":" and the space before each value
    msg_str_len += 2 * (msg->num_signals - 1);  // Account for the "," and the space after each value but the last

    fprintf(fp, "#define %s_STR_LEN %d\n\n", upper_case_name, msg_str_len);
    
    fprintf(fp, "typedef struct %s {\n", snake_case_name);

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    for (signal_t* signal = msg->signals->next; signal != NULL; signal = signal->next) {
        Type_t type = type_from_num_bits_and_sign(signal->num_bits, signal->sign);
        fprintf(fp, "\t%s %s;\n", type_str(type), signal->name);        
    }
    fputs("\tbool valid;  // Not included on wire\n", fp);        
    fprintf(fp, "} %s;\n\n", struct_t_name);
    fprintf(fp, "can_frame_t %s_to_can(const %s* msg);\n", snake_case_name, struct_t_name);
    fprintf(fp, "%s can_to_%s(const can_frame_t* frame);\n", struct_t_name, snake_case_name);
    fprintf(fp, "void print_%s(%s msg);\n", snake_case_name, struct_t_name);
    fprintf(fp, "bool %s_to_str(%s msg, char out[%s_STR_LEN]);\n\n", snake_case_name, struct_t_name, upper_case_name);

    fputs("#endif\n", fp);
    return 0;
}

#define safe_min(a, b) ({          \
    __typeof__ (a) _a = (a);       \
    __typeof__ (b) _b = (b);       \
    _a < _b ? _a : _b;             \
})

void can_to_msg_fn(FILE* fp, const message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
    fprintf(fp, "%s can_to_%s(const can_frame_t* frame) {\n", struct_t_name, snake_case_name);
    fprintf(fp, "\t%s %s;\n", struct_t_name, snake_case_name);
    fprintf(fp, "\tif(frame == NULL || frame->data == NULL || frame->len != %s_LEN) {\n", upper_case_name);
    fprintf(fp, "\t\t%s.valid = false;\n", snake_case_name);
    fprintf(fp, "\t\treturn %s;\n", snake_case_name);
    fputs("\t}\n\n", fp);
    
    fprintf(fp, "\tuint8_t* data = frame->data;\n");

    int bit_offset = 0;
    for (signal_t* signal = msg->signals->next; signal != NULL; signal = signal->next) {
        size_t consumed = 0;
        fprintf(fp, "\t%s.%s = ", snake_case_name, signal->name);
        int num_ors = 0;

        while (consumed < signal->num_bits) {
            size_t pos = bit_offset + consumed;
            size_t bit_in_byte = pos % 8;

            size_t taken = safe_min(signal->num_bits - consumed,  8 - bit_in_byte);
            if (num_ors > 0) {
                fprintf(fp, " | ");
            }

            size_t r_shift = 8 - bit_in_byte - taken;
            size_t l_shift =  signal->num_bits - consumed - taken;

            if (r_shift == 0) {
                if (l_shift == 0) {
                    fprintf(fp, "(data[%zu] & %s)", pos / 8, byte_mask_from_num_bits(taken));
                } else {
                    fprintf(fp, "((uint64_t)(data[%zu] & %s) << %zu)", pos / 8, byte_mask_from_num_bits(taken), l_shift);
                }
                
            } else {
                 if (l_shift == 0) {
                    fprintf(fp, "((data[%zu] >> %zu) & %s)", pos / 8, r_shift, byte_mask_from_num_bits(taken));
                } else {
                    fprintf(fp, "(((uint64_t)(data[%zu] >> %zu) & %s) << %zu)", pos / 8, r_shift, byte_mask_from_num_bits(taken), l_shift);
                }
            }
            
            num_ors++;
            consumed += taken;
        }
        fputs(";\n", fp);

        bit_offset += signal->num_bits;
    }

    fprintf(fp, "\t%s.valid = true;\n", snake_case_name);

    fprintf(fp, "\treturn %s;\n", snake_case_name);
    fputs("}\n\n", fp);

}

void msg_to_can_fn(FILE* fp, const message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
    fprintf(fp, "can_frame_t %s_to_can(const %s* msg) {\n", snake_case_name, struct_t_name);
    fputs("\tcan_frame_t frame = {0};\n", fp);
    fputs("\tif(msg == NULL) {\n", fp);
    fputs("\t\tframe.can_id = INVALID_ID;\n", fp);
    fputs("\t\treturn frame;\n", fp);
    fputs("\t}\n\n", fp);

    fprintf(fp, "\tframe.can_id = %s_ID;\n", upper_case_name);
    fprintf(fp, "\tframe.len = %s_LEN;\n", upper_case_name);  

    int bit_offset = 0;
    for (signal_t* signal = msg->signals->next; signal != NULL; signal = signal->next) {
        size_t consumed = 0;

        while (consumed < signal->num_bits) {
            size_t pos = bit_offset + consumed;
            size_t bit_in_byte = pos % 8;

            size_t taken = safe_min(signal->num_bits - consumed,  8 - bit_in_byte);

            size_t l_shift = 8 - bit_in_byte - taken;
            size_t r_shift =  signal->num_bits - consumed - taken;
            char* mask = byte_mask_from_num_bits(taken);
            size_t byte = pos / 8;
            fprintf(fp, "\tframe.data[%zu] |= (uint8_t) ", byte);

            if (r_shift == 0) {
                if (l_shift == 0) {
                    fprintf(fp, "(msg->%s & %s)", signal->name, mask);
                } else {
                    fprintf(fp, "((msg->%s & %s) << %zu)", signal->name, mask, l_shift);
                }
                
            } else {
                 if (l_shift == 0) {
                    fprintf(fp, "((msg->%s >> %zu) & %s)", signal->name, r_shift, mask);
                } else {
                    fprintf(fp, "(((msg->%s >> %zu) & %s) << %zu)", signal->name, r_shift, mask, l_shift);
                }
            }

            fputs(";\n", fp);
            consumed += taken;
        }

        bit_offset += signal->num_bits;
    }

    fputs("\treturn frame;\n", fp);

    //function end
    fputs("}\n\n", fp);
}

void print_fn(FILE* fp, const message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
    fprintf(fp, "void print_%s(%s msg) {\n", snake_case_name, struct_t_name);
    fputs("}\n\n", fp);
}

void str_fn(FILE* fp, const message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
    // int written = snprintf(out, POWER_STATUS_STR_LEN, "%s: {percent_charge: %d, status: %d, power_on: %s}", POWER_STATUS_NAME, status.percent_charge, status.status, str_bool(status.power_on));
    // printf("%s\n", out);
    // return written == strlen(out);
    fprintf(fp, "bool %s_to_str(%s msg, char out[%s_STR_LEN]) {\n", snake_case_name, struct_t_name, upper_case_name);

    fprintf(fp, "\tint written = snprintf(out, \"%s: {", upper_case_name);
    for (signal_t* signal = msg->signals->next; signal != NULL; signal = signal->next) {
        Type_t type = type_from_num_bits_and_sign(signal->num_bits, signal->sign);
        if (signal->next == NULL) {
            fprintf(fp, "%s: %s", signal->name, type_format_specifier(type));
        } else {
            fprintf(fp, "%s: %s, ", signal->name, type_format_specifier(type));
        }
    }

    fputs("}\", ", fp);
    for (signal_t* signal = msg->signals->next; signal != NULL; signal = signal->next) {
        if (signal->next == NULL) {
            fprintf(fp, "msg.%s", signal->name);
        } else {
            fprintf(fp, "msg.%s, ", signal->name);
        }
    }
    fputs(");\n", fp);
    fputs("\treturn written == strlen(out);\n", fp);
    fputs("}\n", fp);
}

int generate_source(const message_t* msg) { 
    char snake_case_name[100];
    camel_to_snake_case(msg->header.name, snake_case_name);
    
    char parent_dir_path[PATH_MAX];
    snprintf(parent_dir_path, PATH_MAX, "./src/can/msg/%s", snake_case_name);

    if (mkdir(parent_dir_path, 0755) != 0) {
        // Fine if directory already exists
        if (errno != EEXIST) {
            perror("Error creating directory");
            exit(1);
        }
    }
    
    char file_path_c[PATH_MAX];
    snprintf(file_path_c, PATH_MAX, "./src/can/msg/%s/%s.c", snake_case_name, snake_case_name);

    FILE* fp = fopen(file_path_c, "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    char upper_case_name[100];
    to_upper_str(snake_case_name, upper_case_name);
    
    fprintf(fp, "#include \"%s.h\"\n", snake_case_name);
    fputs("#include <stddef.h>\n\n", fp);

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    msg_to_can_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    can_to_msg_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    print_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    str_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    return 0;
}
    
    