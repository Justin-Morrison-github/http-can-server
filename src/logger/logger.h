#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>

#define LOGS_ROOT "logs"

#define MAX_LOGGER_NAME 32
#define MAX_LOGGERS 128
#define ISO_LEN 21

// How to control where logs print to  
#define E_STDOUT 1
#define E_LOG 2
#define E_BOTH 3

typedef struct logger {
    char root[MAX_LOGGER_NAME + 6];
    FILE* fp;
    uint8_t enables;    // E_STDOUT, E_LOG, E_BOTH
} logger_t;

typedef struct log_context {
    char* name;         // Name of folder to put logs in, also for debugging
    size_t index;       // Index of logger in g_loggers
} log_context_t;

log_context_t create_logger(char* name, int enables);

#define LOG_INFO(ctx, ...) _log(ctx, "INFO", __FILE__, __LINE__, false, __VA_ARGS__) 
#define LOG_WARN(ctx, ...) _log(ctx, "WARN", __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERR(ctx, ...) _log(ctx, "ERROR", __FILE__, __LINE__, false, __VA_ARGS__)

#define LOG_INFO_EXT(ctx, ...) _log(ctx, "INFO", __FILE__, __LINE__, true, __VA_ARGS__) 
#define LOG_WARN_EXT(ctx, ...) _log(ctx, "WARN", __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ERR_EXT(ctx, ...) _log(ctx, "ERROR", __FILE__, __LINE__, true, __VA_ARGS__)

static void vlog_extra(log_context_t* ctx, const char* level, const char* file, int line, const char* format, va_list args);
static void vlog(log_context_t* ctx, const char* level, const char* format, va_list args);
void _log(log_context_t* ctx, const char* level, const char* file, int line, bool extra, const char* format, ...);

#endif