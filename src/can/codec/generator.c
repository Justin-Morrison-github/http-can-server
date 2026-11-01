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
    for (signal_t* msg_sig = msg->signals->next; msg_sig != NULL; msg_sig = msg_sig->next) {
        char* type = type_from_num_bits_and_sign(msg_sig->num_bits, msg_sig->sign);
        
        fprintf(fp, "\t%s %s;\n", type, msg_sig->name);        
    }

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    fprintf(fp, "} %s;\n\n", struct_t_name);
    fprintf(fp, "can_frame_t %s_to_can(const %s* msg);\n", snake_case_name, struct_t_name);
    fprintf(fp, "%s can_to_%s(const can_frame_t* frame);\n", struct_t_name, snake_case_name);
    fprintf(fp, "void print_%s(%s msg);\n\n", snake_case_name, struct_t_name);

    fputs("#endif\n", fp);
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

    FILE* fp = fopen(file_path_c, "w");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    size_t LINE_BUF_SIZE = 1000;
    char line_buf[LINE_BUF_SIZE];
    char upper_case_name[100];
    to_upper_str(snake_case_name, upper_case_name);
    
    fprintf(fp, "#include \"%s.h\"\n\n", snake_case_name);

    char struct_t_name[110];
    snprintf(struct_t_name, 110, "%s_t", snake_case_name);

    fprintf(fp, "can_frame_t %s_to_can(const %s* msg) {\n", snake_case_name, struct_t_name);
    fputs("can_frame_t frame;", fp);
    // can_frame_t frame;
    // if (status == NULL) {
    //     frame.can_id = INVALID_ID;
    //     return frame;
    // }
    // frame.can_id = POWER_STATUS_ID;
    // frame.len = POWER_STATUS_DLC;
    // memcpy(&frame.data[0], &status->percent_charge, sizeof(status->percent_charge));
    // memcpy(&frame.data[1], &status->power_on, sizeof(status->power_on));

    // return frame;
    
    
    fputs("}\n", fp);




    fprintf(fp, "%s can_to_%s(const can_frame_t* frame) {\n", struct_t_name, snake_case_name);
    fputs("}\n", fp);
    fprintf(fp, "void print_%s(%s msg) {\n", snake_case_name, struct_t_name);
    fputs("}\n", fp);
    return 0;
}
    
    