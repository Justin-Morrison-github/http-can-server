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
#include <signal.h>

#include "utils.h"
#include "msg/test_u64/test_u64.h"
#include "../logger/logger.h"

volatile bool keep_running = true;

void sig_int_handler(int sig) {
    (void) sig;
    keep_running = false;
}

int main(void) {
    struct sigaction sig = {0};
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    sig.sa_handler = sig_int_handler;

    if (sigaction(SIGINT, &sig, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    logger_t* logger = create_logger("send_can", E_BOTH);
    if (logger == NULL) {
        fprintf(stderr, "Error creating logger\n");
        exit(1);
    }
    LOG_INFO(logger, "Starting");

    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        LOG_ERR(logger, "Error opening CAN socket");
        exit(1);
    }
    
    // Make a test_u64_t
    test_u64_t test_u64;
    test_u64.u64 = 0;
    test_u64.valid = true;
    char test_u64_str[TEST_U64_STR_LEN]; // Buffer to format string into, only allocate once

    uint64_t last_send_time = 0;

    while (keep_running) {
        uint64_t now = millis();
        if (now - last_send_time >= TEST_U64_INTERVAL_MS) {
            can_frame_t frame = test_u64_to_can(&test_u64);

            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                LOG_ERR(logger, "write_to_socket failed: %s", sock_result_str(result));
                exit(1);
            }
            
            if(!test_u64_to_str(test_u64, test_u64_str)) {
                LOG_ERR(logger, "test_u64_to_str failed");
            } else {
                LOG_INFO(logger, "Sent: %s", test_u64_str);
            }

            last_send_time = now;
            test_u64.u64++;

            // bool ack_received = false;
            // can_frame_t ack_frame; 
            // while (!ack_received) {
            //     SockResult_t result = read_from_socket(can_sock_fd, &ack_frame);
            //     if (result != SOCK_OK) {
            //         LOG_ERR(logger, "read_from_socket failed: %s", sock_result_str(result));
            //         exit(1);
            //     }

            //     if (ack_frame.can_id == CAN_ACK_ID) {
            //         can_ack_t can_ack = can_to_ack(ack_frame);
            //         LOG_INFO(logger, "Received ACK: 0x%03X", can_ack.can_id);
            //         ack_received = true;
            //     }
            // }
        }
    }

    LOG_INFO(logger, "Exiting");
    free_logger(logger);
    return 0;
}