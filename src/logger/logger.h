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
#define ISO_LEN 21

// How to control where logs print to  
#define E_STDOUT 1
#define E_LOG 2
#define E_BOTH 3

typedef struct logger {
    char* name;         // Name of folder to put logs in, also for debugging
    char root[MAX_LOGGER_NAME + 6];
    FILE* fp;
    uint8_t enables;    // E_STDOUT, E_LOG, E_BOTH
} logger_t;

logger_t* create_logger(char* name, int enables);
void free_logger(logger_t* logger);

#define LOG(logger, ...) _log_no_level(logger, __FILE__, __LINE__, false, __VA_ARGS__) 
#define LOG_INFO(logger, ...) _log(logger, "INFO", __FILE__, __LINE__, false, __VA_ARGS__) 
#define LOG_WARN(logger, ...) _log(logger, "WARN", __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERR(logger, ...) _log(logger, "ERROR", __FILE__, __LINE__, false, __VA_ARGS__)

#define LOG_INFO_EXT(logger, ...) _log(logger, "INFO", __FILE__, __LINE__, true, __VA_ARGS__) 
#define LOG_WARN_EXT(logger, ...) _log(logger, "WARN", __FILE__, __LINE__, true, __VA_ARGS__)
#define LOG_ERR_EXT(logger, ...) _log(logger, "ERROR", __FILE__, __LINE__, true, __VA_ARGS__)

static void vlog_extra(logger_t* logger, const char* level, const char* file, int line, const char* format, va_list args);
static void vlog(logger_t* logger, const char* level, const char* format, va_list args);
void _log(logger_t* logger, const char* level, const char* file, int line, bool extra, const char* format, ...);
void _log_no_level(logger_t* logger, const char* file, int line, bool extra, const char* format, ...);

#endif