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
	frame.data[0] |= (uint8_t) ((msg->over_voltage & 0x01) << 7);
	frame.data[0] |= (uint8_t) ((msg->over_current & 0x01) << 6);
	frame.data[0] |= (uint8_t) ((msg->under_voltage & 0x01) << 5);
	frame.data[0] |= (uint8_t) ((msg->under_current & 0x01) << 4);
	frame.data[0] |= (uint8_t) ((msg->overheating & 0x01) << 3);
	return frame;
}
motor_error_flags_t can_to_motor_error_flags(const can_frame_t* frame) {
	motor_error_flags_t motor_error_flags;
	if(frame == NULL || frame->data == NULL || frame->len != MOTOR_ERROR_FLAGS_LEN) {
		motor_error_flags.valid = false;
		return motor_error_flags;
	}

	uint8_t* data = frame->data;
	motor_error_flags.over_voltage = ((data[0] >> 7) & 0x01);
	motor_error_flags.over_current = ((data[0] >> 6) & 0x01);
	motor_error_flags.under_voltage = ((data[0] >> 5) & 0x01);
	motor_error_flags.under_current = ((data[0] >> 4) & 0x01);
	motor_error_flags.overheating = ((data[0] >> 3) & 0x01);
	motor_error_flags.valid = true;
	return motor_error_flags;
}
void print_motor_error_flags(motor_error_flags_t msg) {
}
