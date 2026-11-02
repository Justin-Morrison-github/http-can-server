#include "motor_control_status.h"

#include <stddef.h>
can_frame_t motor_control_status_to_can(const motor_control_status_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = MOTOR_CONTROL_STATUS_ID;
	frame.len = MOTOR_CONTROL_STATUS_LEN;
	return frame;
}
motor_control_status_t can_to_motor_control_status(const can_frame_t* frame) {
	motor_control_status_t motor_control_status;
	if(frame == NULL || frame->len != MOTOR_CONTROL_STATUS_LEN) {
		motor_control_status.valid = false;
		return motor_control_status;
	}

	// TODO finish data unpacking
	motor_control_status.valid = true;
	return motor_control_status;
}
void print_motor_control_status(motor_control_status_t msg) {
}
