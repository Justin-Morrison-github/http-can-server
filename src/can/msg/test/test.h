#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define TEST_ID 0xFFF
#define TEST_LEN 31
#define TEST_INTERVAL_MS 0
#define TEST_NAME "Test"

typedef struct test {
	bool bool1;
	bool bool2;
	uint8_t u8;
	int8_t i8;
	uint16_t u16;
	int16_t i16;
	uint32_t u32;
	int32_t i32;
	uint64_t u64;
	int64_t i64;
	bool valid;  // Not included on wire
} test_t;

can_frame_t test_to_can(const test_t* msg);
test_t can_to_test(const can_frame_t* frame);
void print_test(test_t msg);

#endif
