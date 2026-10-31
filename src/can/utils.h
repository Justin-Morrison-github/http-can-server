#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/can.h>
#include <linux/can/raw.h>

typedef struct can_frame can_frame_t;

#define INVALID_ID 0
#define CAN_ACK_ID 1
#define CAN_ACK_LEN 5
typedef enum AckStatus {
    ACK_OK = 0,
    ACK_ERR,
} AckStatus_t;

char* ack_status_to_str();

typedef struct can_ack {
    uint32_t can_id;
    AckStatus_t status;
} can_ack_t;

void print_can_ack(can_ack_t ack);
can_ack_t create_ack(int can_id, AckStatus_t status);
can_frame_t ack_to_can(can_ack_t ack);
can_ack_t can_to_ack(can_frame_t frame);

int open_can_socket(char *can_interface);
uint64_t millis();
char* str_bool(bool b);

typedef enum SockResult {
    SOCK_OK,
    SOCK_WRITE_FAIL,
    SOCK_READ_FAIL,
    SOCK_FRAME_INVALID
} SockResult_t;
char* sock_result_str(SockResult_t result);
SockResult_t write_to_socket(int fd, can_frame_t* frame);
SockResult_t read_from_socket(int fd, can_frame_t* frame);

#endif