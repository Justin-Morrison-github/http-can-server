#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H
#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"
#define TEMPERATURE_SENSOR_ID 0x0FF
#define TEMPERATURE_SENSOR_LEN 9
#define TEMPERATURE_SENSOR_INTERVAL_MS 100
#define TEMPERATURE_SENSOR_NAME "TemperatureSensor"
typedef struct temperature_sensor {
	uint64_t time_ms;
	int8_t temp_c;
} temperature_sensor_t; 
#endif
