#include "http.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "../utils/utils.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

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

void read_until(char* input, char* sentinel, char* output, size_t output_size) {
    size_t input_len = strlen(input);
    size_t sentinel_len = strlen(sentinel);

    if (sentinel_len == 0 || input_len < sentinel_len) {
        output[0] = '\0';
        return;
    }

    for (size_t i = 0; i <= input_len - sentinel_len; i++) {
        if (strncmp(&input[i], sentinel, sentinel_len) == 0) {
            memcpy(output, input, i);
            output[i] = '\0';
            return;
        }
    }
    
    output[0] = '\0';
}

HttpRequestLine_t parse_request_line(char* input) {
    HttpRequestLine_t request_line = {0};
    char method[100];
    char path[PATH_MAX];
    char version[20];
    char request_line_str[1000];

    read_until(input, "\r\n", request_line_str, 1000);

    if(sscanf(request_line_str, "%s %s %s\r\n", method, path, version) != 3) {
        fprintf(stderr, "sscanf failed");
        perror("sscanf");
        exit(1);
    }

    request_line.method = str_to_method(method);
    snprintf(request_line.path, sizeof request_line.path, "%s", path);
    request_line.version = str_to_version(version);
    
    return request_line;
}

HttpRequest_t parse_incoming_message(char* buffer) {
    // printf("Incoming: %s", buffer);

    HttpRequestLine_t request_line = parse_request_line(buffer);
    printf("request_line: %s %s %s\n", method_to_str(request_line.method), request_line.path, version_to_str(request_line.version));

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

int connect_server() {
    struct addrinfo* res = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);    // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // Don't care about IPv4 vs IPv6
    hints.ai_socktype = SOCK_STREAM;    // Use TCP
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    int status = getaddrinfo(NULL, "8001", &hints, &res);
    if (status < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    
    // Get socket
    int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_fd < 0) {
        perror("socket error");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        exit(1);
    }

    if (bind(sock_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind error");
        exit(1);
    }

    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(res->ai_addr, res->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) != 0){
        perror("getnameinfo");
        exit(1);
    }

    freeaddrinfo(res); // free the linked list

    printf("Server bound to %s:%s\n", ip, port);
    return sock_fd;
}

int connect_client() {
    struct addrinfo* res = NULL;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);    // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // Don't care about IPv4 vs IPv6
    hints.ai_socktype = SOCK_STREAM;    // Use TCP
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    int status = getaddrinfo(NULL, "8001", &hints, &res);
    if (status < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Get socket
    int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_fd < 0) {
        perror("socket error");
        exit(3);
    }
    printf("Got socket\n");

    if (connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind error");
        exit(4);
    }

    freeaddrinfo(res); // free the linked list

    return sock_fd;
}