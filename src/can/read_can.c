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
#include "msg/power_status/power_status.h"

int main(void) {
    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }

    // Read CAN frames
    can_frame_t frame_in;

    while (1)
    {
        ssize_t n_bytes = read(can_sock_fd, &frame_in, sizeof(frame_in));
        if (n_bytes < 0) {
            perror("write");
            close(can_sock_fd);
            exit(1);
        } else if (n_bytes == 0) {
            fprintf(stderr, "read EOF");
            exit(1);
        } else if (n_bytes < sizeof(frame_in)) {
            fprintf(stderr, "read: incomplete CAN frame\n");
            exit(1);
        }  
        
        if (frame_in.can_id == POWER_STATUS_ID) {
            printf("Recived: 0x%03X\n", frame_in.can_id);

            power_status_t power_status = can_to_power_status(&frame_in);

            can_ack_t power_status_ack = create_ack(POWER_STATUS_ID, ACK_ERR);
            print_can_ack(power_status_ack);
            can_frame_t frame = ack_to_can(power_status_ack);

            ssize_t n_bytes = write(can_sock_fd, &frame, sizeof(frame));
            if (n_bytes < 0) {
                perror("write");
                close(can_sock_fd);
                exit(2);
            } else if (n_bytes != sizeof(can_frame_t)) {
                // Non-fatal for now
                fprintf(stderr, "%ld of %ld bytes written", n_bytes, sizeof(can_frame_t));
            } else {
                printf("Sent ACK for: 0x%03X\n", power_status_ack.can_id);
            }
        }
    }
    
    return 0;
}