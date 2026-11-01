#ifndef MOTOR_ERROR_FLAGS_H
#define MOTOR_ERROR_FLAGS_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define MOTOR_ERROR_FLAGS_ID 0x403
#define MOTOR_ERROR_FLAGS_LEN 1
#define MOTOR_ERROR_FLAGS_INTERVAL_MS 1000
#define MOTOR_ERROR_FLAGS_NAME "MotorErrorFlags"

typedef struct motor_error_flags {
	bool over_voltage;
	bool over_current;
	bool under_voltage;
	bool under_current;
	bool overheating;
} motor_error_flags_t;

can_frame_t motor_error_flags_to_can(const motor_error_flags_t* msg);
motor_error_flags_t can_to_motor_error_flags(const can_frame_t* frame);
void print_motor_error_flags(motor_error_flags_t msg);

#endif
