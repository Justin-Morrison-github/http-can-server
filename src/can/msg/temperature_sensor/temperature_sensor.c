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
	return frame;
}
temperature_sensor_t can_to_temperature_sensor(const can_frame_t* frame) {
	temperature_sensor_t temperature_sensor;
	if(frame == NULL || frame->data == NULL || frame->len != TEMPERATURE_SENSOR_LEN) {
		temperature_sensor.valid = false;
		return temperature_sensor;
	}

	uint8_t* data = frame->data;
	temperature_sensor.time_ms = ((data[0] & 0xFF) << 56) | ((data[1] & 0xFF) << 48) | ((data[2] & 0xFF) << 40) | ((data[3] & 0xFF) << 32) | ((data[4] & 0xFF) << 24) | ((data[5] & 0xFF) << 16) | ((data[6] & 0xFF) << 8) | (data[7] & 0xFF);
	temperature_sensor.temp_c = (data[8] & 0xFF);
	temperature_sensor.valid = true;
	return temperature_sensor;
}
void print_temperature_sensor(temperature_sensor_t msg) {
}
