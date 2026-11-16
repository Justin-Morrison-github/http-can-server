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
#include <signal.h>
#include <time.h>

#include "../../can/utils.h"
#include "../../logger/logger.h"

 // 8 bytes max, 2 hex chars per byte + null terminator
#define CAN_DATA_STR_LEN    CAN_MAX_DLEN * 2 + 1

volatile bool keep_running = true;

void sig_int_handler(int sig) {
    (void) sig;
    keep_running = false;
}

static const char HEX[] = "0123456789ABCDEF";

// Upper referring to nibble not to casing
char hex_upper(uint8_t byte) {
    return HEX[byte >> 4];
}

char hex_lower(uint8_t byte) {
    return HEX[byte & 0x0F];
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

    logger_t* logger = create_logger("can_logger", E_BOTH);
    if (logger == NULL) {
        LOG_ERR(logger, "Error creating logger");
        exit(1);
    }
    LOG_INFO(logger, "Starting");

    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        exit(1);
    }


    // Read CAN frames
    can_frame_t frame;
    struct timespec ts;
    char frame_data_str[CAN_DATA_STR_LEN];

    while (keep_running)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        // return ((uint64_t)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
        SockResult_t result = read_from_socket(can_sock_fd, &frame);
        if (result != SOCK_OK) {
            LOG_ERR(logger, "read_from_socket failed: %s", sock_result_str(result));
            exit(1);
        }
        
        printf("frame.len: %u", frame.len);
        for (size_t i = 0; i < frame.len; i++) {
            frame_data_str[i*2] = hex_upper(frame.data[i]);
            frame_data_str[i*2 + 1] = hex_lower(frame.data[i]);
        }
        frame_data_str[2 * frame.len] = '\0';   

        LOG(logger, "(%lld.%6lld) vcan0 %03X#%s", ts.tv_sec, ts.tv_nsec, frame.can_id, frame_data_str);
    }

    LOG_INFO(logger, "Exiting");
    free_logger(logger);
    return 0;
}