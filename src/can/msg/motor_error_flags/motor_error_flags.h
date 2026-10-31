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
#endif
