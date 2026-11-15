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
#include "../logger/logger.h"


int main(void) {
    log_context_t logger = create_logger("send_can", E_BOTH);
    if (logger.index == SIZE_MAX) {
        LOG_ERR(&logger, "Error creating logger\n");
        exit(1);
    }

    LOG_INFO(&logger, "Info message");

    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        LOG_ERR(&logger, "Error opening CAN socket");
        exit(1);
    }
    
    // Make a power_status_t
    power_status_t power_status;
    power_status.percent_charge = 0;
    power_status.status = 3;
    power_status.power_on = true;
    power_status.valid = true;
    // print_power_status(power_status);

    can_frame_t frame = power_status_to_can(&power_status);
    LOG_INFO(&logger, "Data to send: [0x%02X, 0x%02X]", frame.data[0], frame.data[1]);
    uint64_t last_send_time = 0;

    while (1) {
        uint64_t now = millis();
        if (now - last_send_time >= POWER_STATUS_INTERVAL_MS) {
            can_frame_t frame = power_status_to_can(&power_status);

            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                fprintf(stderr, "write_to_socket failed: %s", sock_result_str(result));
                exit(1);
            }
            LOG_INFO(&logger, "Sent");

            last_send_time = now;
            power_status.percent_charge++;
            // print_power_status(power_status);

            // bool ack_received = false;
            // can_frame_t ack_frame; 
            // while (!ack_received) {
            //     SockResult_t result = read_from_socket(can_sock_fd, &ack_frame);
            //     if (result != SOCK_OK) {
            //         fprintf(stderr, "read_from_socket failed: %s", sock_result_str(result));
            //         exit(1);
            //     }

            //     if (ack_frame.can_id == CAN_ACK_ID) {
            //         can_ack_t can_ack = can_to_ack(ack_frame);
            //         printf("Received ACK: 0x%03X\n", can_ack.can_id);
            //         ack_received = true;
            //     }
            // }
        }
    }

    return 0;
}