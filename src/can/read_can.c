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
#include "../logger/logger.h"
#include "msg/test_u64/test_u64.h"

int main(void) {
    log_context_t logger = create_logger("read_can", E_BOTH);
    if (logger.index == SIZE_MAX) {
        LOG_ERR(&logger, "Error creating logger\n");
        exit(1);
    }

    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }

    char test_u64_str[TEST_U64_STR_LEN]; // Buffer to format string into, only allocate once

    // Read CAN frames
    can_frame_t frame_in;

    while (1)
    {
        SockResult_t result = read_from_socket(can_sock_fd, &frame_in);
        if (result != SOCK_OK) {
            LOG_ERR(&logger, "read_from_socket failed: %s", sock_result_str(result));
            exit(1);
        }
        
        if (frame_in.can_id == TEST_U64_ID) {
            test_u64_t test_u64 = can_to_test_u64(&frame_in);
            if (!test_u64.valid) {
                LOG_INFO(&logger, "error decoding test_u64\n");
                exit(1);
            }
            
            if(!test_u64_to_str(test_u64, test_u64_str)) {
                LOG_ERR(&logger, "test_u64_to_str failed");
            } else {
                LOG_INFO(&logger, "Recv: %s", test_u64_str);
            }

            can_ack_t test_u64_ack = create_ack(TEST_U64_ID, ACK_ERR);
            can_frame_t frame = ack_to_can(test_u64_ack);

            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                LOG_ERR(&logger, "write_to_socket failed: %s", sock_result_str(result));
                exit(1);
            }
        }
    }
    
    return 0;
}