#include "motor_error_flags.h"

#include <stddef.h>
can_frame_t motor_error_flags_to_can(const motor_error_flags_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = MOTOR_ERROR_FLAGS_ID;
	frame.len = MOTOR_ERROR_FLAGS_LEN;
	return frame;
}
motor_error_flags_t can_to_motor_error_flags(const can_frame_t* frame) {
	motor_error_flags_t motor_error_flags;
	if(frame == NULL || frame->len != MOTOR_ERROR_FLAGS_LEN) {
		motor_error_flags.valid = false;
		return motor_error_flags;
	}

	// TODO finish data unpacking
	motor_error_flags.valid = true;
	return motor_error_flags;
}
void print_motor_error_flags(motor_error_flags_t msg) {
}
