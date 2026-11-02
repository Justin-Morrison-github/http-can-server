#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include "common.h"

int generate_header(message_t* msg) { 
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
    
    fprintf(fp, "#ifndef %s_H\n", upper_case_name);    
    fprintf(fp, "#define %s_H\n\n", upper_case_name);    
    fputs("#include <stdint.h>\n", fp);
    fputs("#include <stdbool.h>\n", fp);
    fputs("#include \"../../utils.h\"\n\n", fp);
    
    fprintf(fp, "#define %s_ID 0x%03X\n", upper_case_name, msg->header.id);
    fprintf(fp, "#define %s_LEN %ld\n", upper_case_name, msg->size_bytes);
    fprintf(fp, "#define %s_INTERVAL_MS %d\n", upper_case_name, msg->header.interval_ms);
    fprintf(fp, "#define %s_NAME \"%s\"\n\n", upper_case_name, msg->header.name);
    fprintf(fp, "typedef struct %s {\n", snake_case_name);

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    for (signal_t* msg_sig = msg->signals->next; msg_sig != NULL; msg_sig = msg_sig->next) {
        char* type = type_from_num_bits_and_sign(msg_sig->num_bits, msg_sig->sign);
        fprintf(fp, "\t%s %s;\n", type, msg_sig->name);        
    }
    fputs("\tbool valid;  // Not included on wire\n", fp);        
    fprintf(fp, "} %s;\n\n", struct_t_name);
    fprintf(fp, "can_frame_t %s_to_can(const %s* msg);\n", snake_case_name, struct_t_name);
    fprintf(fp, "%s can_to_%s(const can_frame_t* frame);\n", struct_t_name, snake_case_name);
    fprintf(fp, "void print_%s(%s msg);\n\n", snake_case_name, struct_t_name);

    fputs("#endif\n", fp);
    return 0;
}

#define safe_min(a, b) ({          \
    __typeof__ (a) _a = (a);       \
    __typeof__ (b) _b = (b);       \
    _a < _b ? _a : _b;             \
})

void can_to_msg_fn(FILE* fp, message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
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
    fputs("}\n", fp);

}

void msg_to_can_fn(FILE* fp, message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
    fprintf(fp, "can_frame_t %s_to_can(const %s* msg) {\n", snake_case_name, struct_t_name);
    fputs("\tcan_frame_t frame = {0};\n", fp);
    fputs("\tif(msg == NULL) {\n", fp);
    fputs("\t\tframe.can_id = INVALID_ID;\n", fp);
    fputs("\t\treturn frame;\n", fp);
    fputs("\t}\n\n", fp);

    fprintf(fp, "\tframe.can_id = %s_ID;\n", upper_case_name);
    fprintf(fp, "\tframe.len = %s_LEN;\n", upper_case_name);  

    // TODO
    fputs("\treturn frame;\n", fp);

    //function end
    fputs("}\n", fp);
}

void print_fn(FILE* fp, message_t* msg, char* struct_t_name, char* snake_case_name, char* upper_case_name) {
    fprintf(fp, "void print_%s(%s msg) {\n", snake_case_name, struct_t_name);
    fputs("}\n", fp);
}

int generate_source(message_t* msg) { 
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
    
    fprintf(fp, "#include \"%s.h\"\n\n", snake_case_name);
    fputs("#include <stddef.h>\n", fp);

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    msg_to_can_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    can_to_msg_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    print_fn(fp, msg, struct_t_name, snake_case_name, upper_case_name);

    return 0;
}
    
    