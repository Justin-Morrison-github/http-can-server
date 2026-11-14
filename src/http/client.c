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

#include "http.h"

int main(int argc, char *argv[]) {
    printf("Start\n");

    int sock_fd = connect_client();
    if (sock_fd < 0) {
        fprintf(stderr, "connect_client failed: %d", sock_fd);
        return 1;
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

    close(sock_fd);
    printf("End\n");
    return 0;
}