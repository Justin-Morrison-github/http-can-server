#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <limits.h>
#include <stdbool.h>

typedef enum HttpVersion {
    UNKNOWN,
    V1_1,
} HttpVersion_t;

char* version_to_str(HttpVersion_t version);
HttpVersion_t str_to_version(char* version);

typedef enum HttpMethod {
    INVALID,
    GET
} HttpMethod_t;

char* method_to_str(HttpMethod_t nethod);
HttpMethod_t str_to_method(char* method);

typedef struct HttpRequest
{
    char path[PATH_MAX];
    char response[256];
    size_t response_len;
    HttpVersion_t version;
    HttpMethod_t method;
    bool valid;
} HttpRequest_t;

HttpRequest_t parse_incoming_message(char* buffer);
#endif