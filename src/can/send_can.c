#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "utils.h"
#include "msg/power_status/power_status.h"

int main(void) {
    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }
    
    // Make a power_status_t
    power_status_t power_status;
    power_status.percent_charge = 16;
    power_status.power_on = true;

    can_frame_t frame = power_status_to_can(&power_status);
    printf("Data to send: [0x%02X, 0x%02X]\n", frame.data[0], frame.data[1]);
    uint64_t last_send_time = 0;

    while (1) {
        uint64_t now = millis();
        if (now - last_send_time >= POWER_STATUS_INTERVAL_MS) {
            ssize_t n_bytes = write(can_sock_fd, &frame, sizeof(frame));
            if (n_bytes < 0) {
                perror("write");
                close(can_sock_fd);
                exit(2);
            } else if (n_bytes != sizeof(can_frame_t)) {
                // Non-fatal for now
                fprintf(stderr, "%ld of %ld bytes written", n_bytes, sizeof(can_frame_t));
            } else {
                last_send_time = now;
                printf("Sent 0x%03X, %u bytes\n", frame.can_id, frame.len);
            }

            bool ack_received = false;
            can_frame_t ack_frame; 
            while (!ack_received) {
                ssize_t n_bytes = read(can_sock_fd, &ack_frame, sizeof(ack_frame));
                if (n_bytes < 0) {
                    perror("write");
                    close(can_sock_fd);
                    exit(1);
                } else if (n_bytes == 0) {
                    fprintf(stderr, "read EOF");
                    exit(1);
                } else if (n_bytes < sizeof(ack_frame)) {
                    fprintf(stderr, "read: incomplete CAN frame\n");
                    exit(1);
                }  

                if (ack_frame.can_id == CAN_ACK_ID) {
                    printf("Received ACK: 0x%03X\n", ack_frame.can_id);
                    can_ack_t can_ack = can_to_ack(ack_frame);
                    print_can_ack(can_ack);
                    ack_received = true;
                }
            }
        }
    }

    return 0;
}