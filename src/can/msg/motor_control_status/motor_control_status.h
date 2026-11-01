#ifndef MOTOR_CONTROL_STATUS_H
#define MOTOR_CONTROL_STATUS_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define MOTOR_CONTROL_STATUS_ID 0x010
#define MOTOR_CONTROL_STATUS_LEN 3
#define MOTOR_CONTROL_STATUS_INTERVAL_MS 10
#define MOTOR_CONTROL_STATUS_NAME "MotorControlStatus"

typedef struct motor_control_status {
	int16_t speed_ms;
	uint8_t charge_percent;
} motor_control_status_t;

can_frame_t motor_control_status_to_can(const motor_control_status_t* msg);
motor_control_status_t can_to_motor_control_status(const can_frame_t* frame);
void print_motor_control_status(motor_control_status_t msg);

#endif
