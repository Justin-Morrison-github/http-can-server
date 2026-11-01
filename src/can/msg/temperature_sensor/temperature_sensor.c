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
}
void print_temperature_sensor(temperature_sensor_t msg) {
}
