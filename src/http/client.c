#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Start\n");
    int status = 0;
    int sock_fd;

    struct addrinfo *res;  // will point to the results

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);    // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;        // Don't care about IPv4 vs IPv6
    hints.ai_socktype = SOCK_STREAM;    // Use TCP
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    status = getaddrinfo(NULL, "8001", &hints, &res);
    if (status < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Get socket
    sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_fd < 0) {
        perror("socket error");
        exit(3);
    }
    printf("Got socket\n");

    if (connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind error");
        exit(4);
    }
    printf("Connected\n");

    char *msg = "TEST123";
    size_t len = strlen(msg);
    ssize_t bytes_sent = send(sock_fd, msg, len, 0);
    if (bytes_sent < 0) {
        perror("send error");
        exit(7);
    } else if (bytes_sent != len) {
        fprintf(stderr, "send couldn't fit all msg bytes in one send: %ld bytes left", len - bytes_sent);
    }

    printf("Sent %ld bytes: %s\n", len, msg);

    freeaddrinfo(res); // free the linked list
    close(sock_fd);
    printf("End\n");
    return 0;
}