#include "logger.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>

logger_t* create_logger(char* name, int enables) {
    if (enables < 0 || enables > E_BOTH) {
        fprintf(stderr, "Invalid enables value (%d)\n", enables);
        return NULL;
    }
    
    logger_t* logger = (logger_t*) malloc(sizeof(logger_t));
    if (logger == NULL) {
        perror("malloc");
        return NULL;
    }

    logger->name = name;
    logger->enables = (uint8_t) enables;
    
    
    time_t now;
    time(&now);
    struct tm* local_tm = localtime(&now);
    
    char iso_date[ISO_LEN];
    strftime(iso_date, ISO_LEN, "%FT%TZ", local_tm);

    snprintf(logger->root, strlen(LOGS_ROOT) + MAX_LOGGER_NAME + 4, "%s/%s", LOGS_ROOT, name);

    char active_file_path[PATH_MAX];
    snprintf(active_file_path, strlen(LOGS_ROOT) + MAX_LOGGER_NAME + ISO_LEN + 6, "%s/%s.log", logger->root, iso_date);

    if (mkdir(LOGS_ROOT, 0755) != 0) {
        // Fine if directory already exists
        if (errno != EEXIST) {
            perror("mkdir");
            fprintf(stderr, "Could not create LOGS_ROOT: %s\n", LOGS_ROOT);
            return NULL;
        }
    }

    if (mkdir(logger->root, 0755) != 0) {
        // Fine if directory already exists
        if (errno != EEXIST) {
            perror("mkdir");
            fprintf(stderr, "Could not create dir: %s\n", logger->root);
            return NULL;
        }
    }

    FILE* fp = fopen(active_file_path, "w");
    if (fp == NULL) {
        perror("open");
        fprintf(stderr, "Could not open log file: %s\n", active_file_path);
        return NULL;
    }

    setvbuf(fp, NULL, _IOLBF, 0);   // flush on every newline
    logger->fp = fp; 

    return logger;
}

void free_logger(logger_t* logger) {
    free(logger);
}

static void vlog_extra(logger_t* logger, const char* level, const char* file, int line, const char* format, va_list args){
    fprintf(stdout, "[%s] %s:%d  ", level, file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

static void vlog(logger_t* logger, const char* level, const char* format, va_list args){
    if (logger->enables & E_STDOUT) {
        va_list copy; // need a copy because vfprintf consumes list 
        va_copy(copy, args);
        fprintf(stdout, "[%s] ", level);
        vfprintf(stdout, format, copy);
        fprintf(stdout, "\n");
        va_end(copy);
    }
    
    if (logger->enables & E_LOG) {
        fprintf(logger->fp, "[%s] ", level);
        vfprintf(logger->fp, format, args);
        fprintf(logger->fp, "\n");
    }
}


void _log(logger_t* logger, const char* level, const char* file, int line, bool extra, const char* format, ...) {
    va_list args;
    va_start(args, format);
    if (extra) {
        vlog_extra(logger, level, file, line, format, args);
    } else {
        vlog(logger, level, format, args);
    }
    
    va_end(args);
}