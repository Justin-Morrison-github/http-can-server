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

size_t g_num_loggers = 0;
logger_t g_loggers[MAX_LOGGERS];

log_context_t create_logger(char* name, int enables) {
    log_context_t ctx;

    if (enables < 0 || enables > E_BOTH) {
        fprintf(stderr, "Invalid enables value (%d)\n", enables);
        ctx.index = SIZE_MAX;
        return ctx;
    }
    
    if (g_num_loggers == MAX_LOGGERS) {
        fprintf(stderr, "Max loggers reached: Cannot add logger for %s\n", name);
        ctx.index = SIZE_MAX;
        return ctx;
    }
    
    ctx.index = g_num_loggers;
    ctx.name = name;

    logger_t logger;
    char iso_date[ISO_LEN];
    
    time_t now;
    time(&now);
    struct tm* local_tm = localtime(&now);

    strftime(iso_date, ISO_LEN, "%FT%TZ", local_tm);
    snprintf(logger.root, strlen(LOGS_ROOT) + MAX_LOGGER_NAME + 4, "%s/%s", LOGS_ROOT, name);
    char active_file_path[PATH_MAX];
    snprintf(active_file_path, strlen(LOGS_ROOT) + MAX_LOGGER_NAME + ISO_LEN + 6, "%s/%s.log", logger.root, iso_date);
    printf("active_file_path: %s\n", active_file_path);

    if (mkdir(LOGS_ROOT, 0755) != 0) {
        // Fine if directory already exists
        if (errno != EEXIST) {
            perror("mkdir");
            fprintf(stderr, "Could not create LOGS_ROOT: %s\n", LOGS_ROOT);
            ctx.index = SIZE_MAX;
            return ctx;
        }
    }

    if (mkdir(logger.root, 0755) != 0) {
        // Fine if directory already exists
        if (errno != EEXIST) {
            perror("mkdir");
            fprintf(stderr, "Could not create dir: %s\n", logger.root);
            ctx.index = SIZE_MAX;
            return ctx;
        }
    }

    FILE* fp = fopen(active_file_path, "w");
    if (fp == NULL) {
        perror("open");
        fprintf(stderr, "Could not open log file: %s\n", active_file_path);
        ctx.index = SIZE_MAX;
        return ctx;
    }

    setvbuf(fp, NULL, _IOLBF, 0);   // flush on every newline

    logger.fp = fp; 
    logger.enables = (uint8_t) enables;
    g_loggers[g_num_loggers++] = logger;
    return ctx;
}
static void vlog_extra(log_context_t* ctx, const char* level, const char* file, int line, const char* format, va_list args){
    fprintf(stdout, "[%s] %s:%d  ", level, file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

static void vlog(log_context_t* ctx, const char* level, const char* format, va_list args){
    logger_t logger = g_loggers[ctx->index];

    if (logger.enables & E_STDOUT) {
        va_list copy; // need a copy because vfprintf consumes list 
        va_copy(copy, args);
        fprintf(stdout, "[%s] ", level);
        vfprintf(stdout, format, copy);
        fprintf(stdout, "\n");
        va_end(copy);
    }
    
    if (logger.enables & E_LOG) {
        fprintf(logger.fp, "[%s] ", level);
        vfprintf(logger.fp, format, args);
        fprintf(logger.fp, "\n");
    }
}


void _log(log_context_t* ctx, const char* level, const char* file, int line, bool extra, const char* format, ...) {
    va_list args;
    va_start(args, format);
    if (extra) {
        vlog_extra(ctx, level, file, line, format, args);
    } else {
        vlog(ctx, level, format, args);
    }
    
    va_end(args);
}