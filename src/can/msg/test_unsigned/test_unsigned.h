#ifndef TEST_UNSIGNED_H
#define TEST_UNSIGNED_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define TEST_UNSIGNED_ID 0x123
#define TEST_UNSIGNED_LEN 8
#define TEST_UNSIGNED_INTERVAL_MS 1000
#define TEST_UNSIGNED_NAME "TestUnsigned"
#define TEST_UNSIGNED_STR_LEN 67

typedef struct test_unsigned {
	bool flag;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	bool valid;  // Not included on wire
} test_unsigned_t;

can_frame_t test_unsigned_to_can(const test_unsigned_t* msg);
test_unsigned_t can_to_test_unsigned(const can_frame_t* frame);
void print_test_unsigned(test_unsigned_t msg);
bool test_unsigned_to_str(test_unsigned_t msg, char out[TEST_UNSIGNED_STR_LEN]);
bool can_to_test_unsigned_str(const can_frame_t* frame, char out[TEST_UNSIGNED_STR_LEN]);

#endif
