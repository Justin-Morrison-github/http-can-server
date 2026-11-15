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
#include "msg/test_u64/test_u64.h"

int main(void) {
    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }

    // Read CAN frames
    can_frame_t frame_in;

    while (1)
    {
        SockResult_t result = read_from_socket(can_sock_fd, &frame_in);
        if (result != SOCK_OK) {
            fprintf(stderr, "read_from_socket failed: %s", sock_result_str(result));
            exit(1);
        }
        
        if (frame_in.can_id == TEST_U64_ID) {
            printf("Recived: 0x%03X\n", frame_in.can_id);

            test_u64_t test_u64 = can_to_test_u64(&frame_in);
            if (!test_u64.valid) {
                printf("error decoding test_u64\n");
                exit(1);
            }

            can_ack_t test_u64_ack = create_ack(TEST_U64_ID, ACK_ERR);
            can_frame_t frame = ack_to_can(test_u64_ack);

            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                fprintf(stderr, "write_to_socket failed: %s", sock_result_str(result));
                exit(1);
            }
        }
    }
    
    return 0;
}