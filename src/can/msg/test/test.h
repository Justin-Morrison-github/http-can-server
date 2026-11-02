#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define TEST_ID 0xFFF
#define TEST_LEN 8
#define TEST_INTERVAL_MS 0
#define TEST_NAME "Test"

typedef struct test {
	bool bool1;
	bool bool2;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	bool valid;  // Not included on wire
} test_t;

can_frame_t test_to_can(const test_t* msg);
test_t can_to_test(const can_frame_t* frame);
void print_test(test_t msg);

#endif
