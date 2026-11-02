#include "temperature_sensor.h"

#include <stddef.h>
can_frame_t temperature_sensor_to_can(const temperature_sensor_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = TEMPERATURE_SENSOR_ID;
	frame.len = TEMPERATURE_SENSOR_LEN;
	frame.data[0] |= (uint8_t) ((msg->time_ms >> 56) & 0xFF);
	frame.data[1] |= (uint8_t) ((msg->time_ms >> 48) & 0xFF);
	frame.data[2] |= (uint8_t) ((msg->time_ms >> 40) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->time_ms >> 32) & 0xFF);
	frame.data[4] |= (uint8_t) ((msg->time_ms >> 24) & 0xFF);
	frame.data[5] |= (uint8_t) ((msg->time_ms >> 16) & 0xFF);
	frame.data[6] |= (uint8_t) ((msg->time_ms >> 8) & 0xFF);
	frame.data[7] |= (uint8_t) (msg->time_ms & 0xFF);
	frame.data[8] |= (uint8_t) (msg->temp_c & 0xFF);
	return frame;
}
temperature_sensor_t can_to_temperature_sensor(const can_frame_t* frame) {
	temperature_sensor_t temperature_sensor;
	if(frame == NULL || frame->data == NULL || frame->len != TEMPERATURE_SENSOR_LEN) {
		temperature_sensor.valid = false;
		return temperature_sensor;
	}

	uint8_t* data = frame->data;
	temperature_sensor.time_ms = ((uint64_t)(data[0] & 0xFF) << 56) | ((uint64_t)(data[1] & 0xFF) << 48) | ((uint64_t)(data[2] & 0xFF) << 40) | ((uint64_t)(data[3] & 0xFF) << 32) | ((uint64_t)(data[4] & 0xFF) << 24) | ((uint64_t)(data[5] & 0xFF) << 16) | ((uint64_t)(data[6] & 0xFF) << 8) | (data[7] & 0xFF);
	temperature_sensor.temp_c = (data[8] & 0xFF);
	temperature_sensor.valid = true;
	return temperature_sensor;
}
void print_temperature_sensor(temperature_sensor_t msg) {
}
