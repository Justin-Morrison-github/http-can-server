#include "http.h"
#include "stdio.h"

char* version_to_str(HttpVersion_t version) {
    switch (version) {
        case V1_1:
            return "HTTP/1.1";

        case UNKNOWN: // Purposeful fallthrough
        default:
            return "Unknown";
    }
}

HttpVersion_t str_to_version(char* version) {
    if (strcmp(version, "HTTP/1.1") == 0) {
        return V1_1;
    } else {
        return UNKNOWN;
    }
    
}

char* method_to_str(HttpMethod_t nethod) {
    switch (nethod) {
        case GET:
            return "GET";
        default:
            return "Invalid";
    }
}

HttpMethod_t str_to_method(char* method) {
    if (strcmp(method, "GET") == 0) {
        return GET;
    } else {
        return INVALID;
    }
    
}

HttpRequest_t parse_incoming_message(char* buffer) {
    printf("Incoming: %s", buffer);

    HttpRequest_t request;
    request.valid = true;
    const char *body = "{\"status\":\"ok\"}";

    request.response_len = snprintf(
        request.response,
        sizeof(request.response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        strlen(body),
        body
    );

    return request;
}