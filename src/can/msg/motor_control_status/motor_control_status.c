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
	frame.data[0] |= (uint8_t) ((msg->speed_ms >> 8) & 0xFF);
	frame.data[1] |= (uint8_t) (msg->speed_ms & 0xFF);
	frame.data[2] |= (uint8_t) (msg->charge_percent & 0xFF);
	return frame;
}
motor_control_status_t can_to_motor_control_status(const can_frame_t* frame) {
	motor_control_status_t motor_control_status;
	if(frame == NULL || frame->data == NULL || frame->len != MOTOR_CONTROL_STATUS_LEN) {
		motor_control_status.valid = false;
		return motor_control_status;
	}

	uint8_t* data = frame->data;
	motor_control_status.speed_ms = ((uint64_t)(data[0] & 0xFF) << 8) | (data[1] & 0xFF);
	motor_control_status.charge_percent = (data[2] & 0xFF);
	motor_control_status.valid = true;
	return motor_control_status;
}
void print_motor_control_status(motor_control_status_t msg) {
}
