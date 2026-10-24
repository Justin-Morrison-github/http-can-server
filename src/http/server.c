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

    if (bind(sock_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind error");
        exit(4);
    }

    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(res->ai_addr, res->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) != 0){
        perror("getnameinfo");
        exit(5);
    }
    printf("Server bound to %s:%s\n", ip, port);


    if (listen(sock_fd, 1) < 0) {
        perror("listen error");
        exit(6);
    }
    printf("Server listening...\n");

    struct sockaddr_storage their_addr;
    int addr_size = sizeof(their_addr);
    int accept_fd;

    accept_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);
    if (accept_fd < 0) {
        perror("accept error");
        exit(7);
    }
    printf("Server accepted client\n");

    // Read data sent by client
    char buffer[1000] = {"0"};

    ssize_t bytes_read = recv(accept_fd, buffer, 1000, 0);
    if (bytes_read < 0)     {
        perror("recv error");
        exit(8);
    } else if (bytes_read == 0) {
        perror("client closed connection");
        exit(9);
    }
    buffer[bytes_read] = '\0';
    printf("Server recieved %zd bytes: %s\n", bytes_read, buffer);

    freeaddrinfo(res); // free the linked list
    close(sock_fd);
    close(accept_fd);
    printf("End\n");
    return 0;
}