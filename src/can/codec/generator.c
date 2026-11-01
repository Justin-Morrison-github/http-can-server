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

    FILE* h_fp = fopen(file_path_h, "w");
    if (h_fp == NULL) {
        perror("fopen");
        return 1;
    }

    size_t LINE_BUF_SIZE = 1000;
    char line_buf[LINE_BUF_SIZE];
    char upper_case_name[100];
    to_upper_str(snake_case_name, upper_case_name);
    
    fprintf(h_fp, "#ifndef %s_H\n", upper_case_name);    
    fprintf(h_fp, "#define %s_H\n\n", upper_case_name);    
    fputs("#include <stdint.h>\n", h_fp);
    fputs("#include <stdbool.h>\n", h_fp);
    fputs("#include \"../../utils.h\"\n\n", h_fp);
    
    fprintf(h_fp, "#define %s_ID 0x%03X\n", upper_case_name, msg->header.id);
    fprintf(h_fp, "#define %s_LEN %ld\n", upper_case_name, msg->size_bytes);
    fprintf(h_fp, "#define %s_INTERVAL_MS %d\n", upper_case_name, msg->header.interval_ms);
    fprintf(h_fp, "#define %s_NAME \"%s\"\n\n", upper_case_name, msg->header.name);
    fprintf(h_fp, "typedef struct %s {\n", snake_case_name);
    for (signal_t* msg_sig = msg->signals->next; msg_sig != NULL; msg_sig = msg_sig->next) {
        char* type = type_from_num_bits_and_sign(msg_sig->num_bits, msg_sig->sign);
        
        fprintf(h_fp, "\t%s %s;\n", type, msg_sig->name);        
    }

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    fprintf(h_fp, "} %s;\n\n", struct_t_name);
    fprintf(h_fp, "can_frame_t %s_to_can(const %s* msg);\n", snake_case_name, struct_t_name);
    fprintf(h_fp, "%s can_to_%s(const can_frame_t* frame);\n", struct_t_name, snake_case_name);
    fprintf(h_fp, "void print_%s(%s msg);\n\n", snake_case_name, struct_t_name);

    fputs("#endif\n", h_fp);
    return 0;
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

    FILE* h_fp = fopen(file_path_c, "w");
    if (h_fp == NULL) {
        perror("fopen");
        return 1;
    }

    size_t LINE_BUF_SIZE = 1000;
    char line_buf[LINE_BUF_SIZE];
    char upper_case_name[100];
    to_upper_str(snake_case_name, upper_case_name);
    
    fprintf(h_fp, "#include \"%s.h\"\n\n", snake_case_name);

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    fprintf(h_fp, "can_frame_t %s_to_can(const %s* msg) {\n", snake_case_name, struct_t_name);
    fputs("}\n", h_fp);
    fprintf(h_fp, "%s can_to_%s(const can_frame_t* frame) {\n", struct_t_name, snake_case_name);
    fputs("}\n", h_fp);
    fprintf(h_fp, "void print_%s(%s msg) {\n", snake_case_name, struct_t_name);
    fputs("}\n", h_fp);
    return 0;
}
    
    