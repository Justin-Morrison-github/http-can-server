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
        SockResult_t result = read_from_socket(can_sock_fd, &frame_in);
        if (result != SOCK_OK) {
            fprintf(stderr, "read_from_socket failed: %s", sock_result_str(result));
            exit(1);
        }
        
        if (frame_in.can_id == POWER_STATUS_ID) {
            printf("Recived: 0x%03X\n", frame_in.can_id);

            power_status_t power_status = can_to_power_status(&frame_in);
            can_ack_t power_status_ack = create_ack(POWER_STATUS_ID, ACK_ERR);
            can_frame_t frame = ack_to_can(power_status_ack);

            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                fprintf(stderr, "write_to_socket failed: %s", sock_result_str(result));
                exit(1);
            }
        }
    }
    
    return 0;
}