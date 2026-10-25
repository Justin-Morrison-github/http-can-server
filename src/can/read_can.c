#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

int main(void) {
    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }

    // Read CAN frames
    struct can_frame frame_in;

    while (1)
    {
        ssize_t n_bytes = read(can_sock_fd, &frame_in, sizeof(frame_in));
        if (n_bytes < 0) {
            perror("write");
            close(can_sock_fd);
            exit(1);
        } else if (n_bytes == 0) {
            fprintf(stderr, "read EOF");
        } else if (n_bytes < sizeof(frame_in)) {
            fprintf(stderr, "read: incomplete CAN frame\n");
            exit(1);
        } else {
            printf("Read CAN ID 0x%03X, %u bytes\n", frame_in.can_id, frame_in.len);
        }
    }
    
    return 0;
}