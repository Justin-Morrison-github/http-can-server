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
#include "msg/test_u64/test_u64.h"
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
    
    // Make a test_u64_t
    test_u64_t test_u64;
    test_u64.u64 = 0;
    test_u64.valid = true;
    char test_u64_str[TEST_U64_STR_LEN]; // Buffer to format string into, only allocate once

    if(!test_u64_to_str(test_u64, test_u64_str)) {
        LOG_ERR(&logger, "test_u64_to_str failed");
    } else {
        LOG_INFO(&logger, "%s", test_u64_str);
    }

    can_frame_t frame = test_u64_to_can(&test_u64);
    LOG_INFO(&logger, "Data to send: [0x%02X, 0x%02X]", frame.data[0], frame.data[1]);
    uint64_t last_send_time = 0;

    while (1) {
        uint64_t now = millis();
        if (now - last_send_time >= TEST_U64_INTERVAL_MS) {
            can_frame_t frame = test_u64_to_can(&test_u64);

            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                fprintf(stderr, "write_to_socket failed: %s", sock_result_str(result));
                exit(1);
            }
            LOG_INFO(&logger, "Sent");

            last_send_time = now;
            test_u64.u64++;

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