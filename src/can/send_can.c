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
#include <time.h>
#include <stdint.h>

#include "utils.h"

uint64_t millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((uint64_t)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

int main(void) {
    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }

    // Make a CAN frame
    struct can_frame frame = {0};
    frame.can_id = 0x123;
    frame.len = 7;
    memcpy(frame.data, "TEST123", 7);

    uint64_t last_hearbeat_time = 0;

    while (1) {
        uint64_t now = millis();
        if (now - last_hearbeat_time >= 1000) {
            ssize_t n_bytes = write(can_sock_fd, &frame, sizeof(frame));
            if (n_bytes < 0) {
                perror("write");
                close(can_sock_fd);
                exit(2);
            } else if (n_bytes != sizeof(struct can_frame)) {
                // Non-fatal for now
                fprintf(stderr, "%ld of %ld bytes written", n_bytes, sizeof(struct can_frame));
            } else {
                last_hearbeat_time = now;
                printf("Sent CAN ID 0x%03X, %u bytes\n", frame.can_id, frame.len);
            }
        }
    }

    return 0;
}