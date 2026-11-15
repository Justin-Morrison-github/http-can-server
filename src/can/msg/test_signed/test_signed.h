#ifndef TEST_SIGNED_H
#define TEST_SIGNED_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define TEST_SIGNED_ID 0x456
#define TEST_SIGNED_LEN 8
#define TEST_SIGNED_INTERVAL_MS 0
#define TEST_SIGNED_NAME "TestSigned"
#define TEST_SIGNED_STR_LEN 68

typedef struct test_signed {
	bool flag;
	int8_t i8;
	int16_t i16;
	int32_t i32;
	bool valid;  // Not included on wire
} test_signed_t;

can_frame_t test_signed_to_can(const test_signed_t* msg);
test_signed_t can_to_test_signed(const can_frame_t* frame);
void print_test_signed(test_signed_t msg);
bool test_signed_to_str(test_signed_t msg, char out[TEST_SIGNED_STR_LEN]);

#endif
