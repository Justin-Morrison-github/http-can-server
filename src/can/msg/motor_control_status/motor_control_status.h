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
#endif
