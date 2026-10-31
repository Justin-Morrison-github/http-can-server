
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>     // 
#include <sys/ioctl.h>      // For ioctl and SIOCGIFINDEX
#include <linux/can.h>      // For CAN_RAW and sockaddr_can
#include <net/if.h>         // For ifreq
#include <unistd.h>
#include <time.h>

#include "utils.h"

int open_can_socket(char *can_interface)
{
    int can_sock_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_sock_fd < 0)
    {
        perror("CAN socket could not be opened");
        close(can_sock_fd);
        return -1;
    }
    printf("CAN socket opened succesfully\n");

    struct ifreq ifr;
    strcpy(ifr.ifr_name, can_interface);

    if (ioctl(can_sock_fd, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("ioctl SIOCGIFINDEX");
        close(can_sock_fd);
        return -1;
    }

    // Bind the socket to vcan0
    struct sockaddr_can addr = {0};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(can_sock_fd);
        return -1;
    }
    printf("CAN socket bound to vcan0\n");
    return can_sock_fd;
}
char* sock_result_str(SockResult_t result) {
    switch (result) {
        case SOCK_OK:
            return "OK";
        case SOCK_WRITE_FAIL:
            return "WRITE_FAIL";
        case SOCK_READ_FAIL:
            return "READ_FAIL";
        case SOCK_FRAME_INVALID:
            return "FRAME_INVALID";
        default:
            return "UNKNOWN";
    }
}

SockResult_t write_to_socket(int fd, can_frame_t* frame) {
    if (frame == NULL) {
        fprintf(stderr, "cannot write null frame to socket");
        return SOCK_FRAME_INVALID;
    }
    
    ssize_t n_bytes = write(fd, frame, sizeof(can_frame_t));
    if (n_bytes < 0) {
        perror("write");
        close(fd);
        return SOCK_WRITE_FAIL;
    } else if (n_bytes != sizeof(can_frame_t)) {
        fprintf(stderr, "%ld of %ld bytes written", n_bytes, sizeof(can_frame_t));
        return SOCK_WRITE_FAIL;
    } else {
        printf("Sent 0x%03X, %u bytes\n", frame->can_id, frame->len);
    }

    return SOCK_OK;
}

SockResult_t read_from_socket(int fd, can_frame_t* frame) {
    if (frame == NULL) {
        fprintf(stderr, "cannot write null frame to socket");
        return SOCK_FRAME_INVALID;
    }
    
    ssize_t n_bytes = read(fd, frame, sizeof(can_frame_t));
    if (n_bytes < 0) {
        perror("write");
        close(fd);
        return SOCK_READ_FAIL;
    } else if (n_bytes == 0) {
        fprintf(stderr, "read EOF");
        return SOCK_READ_FAIL;
    } else if (n_bytes < sizeof(can_frame_t)) {
        fprintf(stderr, "read: incomplete CAN frame\n");
        return SOCK_READ_FAIL;
    }  

    return SOCK_OK;
}

uint64_t millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((uint64_t)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

char* str_bool(bool b) {
    return b ? "True" : "False";
}

char* ack_status_to_str(AckStatus_t status) {
    switch (status) {
        case ACK_OK:
            return "OK";
        case ACK_ERR:
            return "ERR";
        default:
            return "UNKNOWN";
    }
}


void print_can_ack(can_ack_t ack){
    printf("can_ack\n");
    printf("  msg_id: %03X\n", ack.can_id);
    printf("  status: %s\n", ack_status_to_str(ack.status));
}

can_ack_t create_ack(int can_id, AckStatus_t status) {
    can_ack_t ack;
    ack.can_id = (uint32_t)can_id;
    ack.status = status;
    return ack;
}

can_frame_t ack_to_can(const can_ack_t ack) {
    can_frame_t frame;
    frame.can_id = (uint32_t) CAN_ACK_ID;
    frame.len = (uint8_t) CAN_ACK_LEN;
    memcpy(&frame.data[0], &ack.can_id, sizeof(ack.can_id)); 
    memcpy(&frame.data[4], &ack.status, sizeof(ack.status)); 
    return frame;
}

can_ack_t can_to_ack(can_frame_t frame) {
    can_ack_t ack;
    memcpy(&ack.can_id, &frame.data[0], 4);
    memcpy(&ack.status, &frame.data[4], 1);
    return ack;
}
