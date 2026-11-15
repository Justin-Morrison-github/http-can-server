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
#include <poll.h>
#include <time.h>

#include "../can/utils.h"
#include "../http/http.h"
#include "../logger/logger.h"
#include "../can/msg/test_u64/test_u64.h"

#define NUM_SOCKETS 2
#define CAN_SOCKET 0
#define HTTP_SOCKET 1


// Define the registry that holds latest received CAN frames
// Use basic indexed array based on CAN ID, waste a little memory for ease of implementation
// Assuming standard CAN with 11-bit IDs
#define STD_CAN_ID_COUNT 2048  // 2^11

typedef struct can_message {
    struct timespec last_seen;    
    uint64_t rx_count;     // 0 means it hasn't been seen yet
    can_frame_t frame;
} can_message_t;

typedef struct can_context {
    can_message_t msg;
    void* (*decoder)(const can_frame_t* frame);
} can_context_t;

static can_context_t contexts[STD_CAN_ID_COUNT];

bool add_rx_can_msg(can_frame_t frame) {
    size_t id = frame.can_id & CAN_SFF_MASK;
    can_message_t* msg = &contexts[id].msg;
    
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return false;
    }

    msg->frame = frame;
    msg->rx_count++;
    msg->last_seen = now;

    return true;
}

void* no_op(const can_frame_t* frame) {}

void init_contexts() {
    for (size_t i = 0; i < STD_CAN_ID_COUNT; i++) {
        contexts[i].decoder = no_op;
    }

    contexts[TEST_U64_ID].decoder = can_to_test_u64;
}

int main(void) {
    init_contexts();

    logger_t* logger = create_logger("bridge", E_BOTH);
    if (logger == NULL) {
        LOG_ERR(logger, "Error creating logger\n");
        exit(1);
    }
    

    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        LOG_ERR(logger, "CAN socket fd is invalid\n");
        return 1;
    }

    int http_sock_fd = connect_server();
    if (http_sock_fd < 0)
    {
        LOG_ERR(logger, "HTTP socket fd is invalid\n");
        return 1;
    }

    if (listen(http_sock_fd, 1) < 0) {
        perror("listen error");
        exit(1);
    }

    LOG_INFO(logger, "Server listening...\n");
    

    struct pollfd fds[NUM_SOCKETS];
    fds[CAN_SOCKET].fd = can_sock_fd;
    fds[CAN_SOCKET].events = POLLIN;
    
    fds[HTTP_SOCKET].fd = http_sock_fd;
    fds[HTTP_SOCKET].events = POLLIN;
    
    can_frame_t in_frame;

    while(1) {
        int ret = poll(fds, NUM_SOCKETS, -1);
        if (ret < 0) {
            perror("poll");
            break;
        }

        if (fds[CAN_SOCKET].revents & POLLERR) {
            LOG_ERR(logger, "CAN socket error: POLLERR\n");
        }

        if (fds[HTTP_SOCKET].revents & POLLERR) {
            LOG_ERR(logger, "HTTP socket error: POLLERR\n");
        }
        
        if (fds[CAN_SOCKET].revents & POLLIN) {
            SockResult_t result = read_from_socket(can_sock_fd, &in_frame);
            if (result != SOCK_OK) {
                LOG_ERR(logger, "read_from_socket failed: %s", sock_result_str(result));
                exit(1);
            } else {
                // LOG_INFO(logger, "CAN frame recieved\n");
                
                if (in_frame.can_id & CAN_EFF_FLAG) { // Only handle standard (11-bit ID) frames for now
                    LOG_INFO(logger, "Error: extended frame\n");
                } else {
                    add_rx_can_msg(in_frame);
                }
            }
        }

        if (fds[HTTP_SOCKET].revents & POLLIN) {
            int accept_fd = accept(http_sock_fd, NULL, NULL);
            if (accept_fd < 0) {
                LOG_ERR(logger, "accept failed");
                break;
            }  else {
                LOG_INFO(logger, "New Client\n");
                // Parse HTTP request
                // Lets assume a client requested the telemetry of TEST_U64 (0x444)
                HttpCANRequest_t can_request;
                can_request.can_id = TEST_U64_ID;
                can_request.method = CAN_GET;

                can_message_t latest = contexts[can_request.can_id].msg;
                test_u64_t test_u64 = can_to_test_u64(&latest.frame);
                LOG_INFO(logger, "Count %ld : %lld.%09ld\n", latest.rx_count, (long long)latest.last_seen.tv_sec, (long)latest.last_seen.tv_nsec);
            }
        }
    }



    // can_frame_t frame_in;

    // while (1)
    // {
    //     SockResult_t result = read_from_socket(can_sock_fd, &frame_in);
    //     if (result != SOCK_OK) {
    //         LOG_ERR(logger, "read_from_socket failed: %s", sock_result_str(result));
    //         exit(1);
    //     }
        
    //     if (frame_in.can_id == TEST_U64_ID) {
    //         LOG_INFO(logger, "Recived: 0x%03X\n", frame_in.can_id);

    //         test_u64_t test_u64 = can_to_test_u64(&frame_in);
    //         if (!test_u64.valid) {
    //             LOG_INFO(logger, "error decoding test_u64\n");
    //             exit(1);
    //         }

    //         can_ack_t test_u64_ack = create_ack(TEST_U64_ID, ACK_OK);
    //         can_frame_t frame = ack_to_can(test_u64_ack);

    //         SockResult_t result = write_to_socket(can_sock_fd, &frame);
    //         if (result != SOCK_OK) {
    //             LOG_ERR(logger, "write_to_socket failed: %s", sock_result_str(result));
    //             exit(1);
    //         }
    //     }
    // }
    


    return 0;
}