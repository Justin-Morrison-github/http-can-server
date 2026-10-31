#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#define INVALID_ID 0
int open_can_socket(char *can_interface);
uint64_t millis();
typedef struct can_frame can_frame_t;
char* str_bool(bool b);
#endif