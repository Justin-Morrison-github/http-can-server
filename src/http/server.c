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
    // printf("Start\n");
    int sock_fd = connect_server();
    if (sock_fd < 0) {
        fprintf(stderr, "connect_server failed: %d", sock_fd);
        return 1;
    }

    if (listen(sock_fd, 1) < 0) {
        perror("listen error");
        exit(1);
    }
    printf("Server listening...\n");

    struct sockaddr_storage their_addr;
    int addr_size = sizeof(their_addr);
    int accept_fd;

    accept_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);
    if (accept_fd < 0) {
        perror("accept error");
        exit(1);
    }
    // printf("Server accepted client\n");

    // Read data sent by client
    char buffer[1000] = {"0"};

    ssize_t bytes_read = recv(accept_fd, buffer, 1000, 0);
    if (bytes_read < 0)     {
        perror("recv error");
        exit(1);
    } else if (bytes_read == 0) {
        perror("client closed connection");
        exit(1);
    }
    buffer[bytes_read] = '\0';
    // printf("Server recieved %zd bytes\n", bytes_read);

    HttpRequest_t request = parse_incoming_message(buffer);
    if (!request.valid){
        fprintf(stderr, "incoming message was not valid");
        exit(1);
    }
    

    ssize_t bytes_sent = send(accept_fd, request.response, request.response_len, 0);
    if (bytes_sent < 0)     {
        perror("send error");
        exit(1);
    } else if (bytes_sent != request.response_len) {
        fprintf(stderr, "send couldn't fit all msg bytes in one send: %ld bytes left", request.response_len - bytes_sent);
    }

    // printf("Server sent %zd bytes: %s\n", bytes_sent, request.response);

    close(sock_fd);
    close(accept_fd);
    // printf("End\n");
    return 0;
}