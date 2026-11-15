#ifndef TEST_I64_H
#define TEST_I64_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define TEST_I64_ID 0x640
#define TEST_I64_LEN 8
#define TEST_I64_INTERVAL_MS 0
#define TEST_I64_NAME "TestI64"
#define TEST_I64_STR_LEN 38

typedef struct test_i64 {
	int64_t i64;
	bool valid;  // Not included on wire
} test_i64_t;

can_frame_t test_i64_to_can(const test_i64_t* msg);
test_i64_t can_to_test_i64(const can_frame_t* frame);
void print_test_i64(test_i64_t msg);
bool test_i64_to_str(test_i64_t msg, char out[TEST_I64_STR_LEN]);

#endif
