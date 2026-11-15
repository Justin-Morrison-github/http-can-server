#ifndef TEST_U64_H
#define TEST_U64_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define TEST_U64_ID 0x064
#define TEST_U64_LEN 8
#define TEST_U64_INTERVAL_MS 0
#define TEST_U64_NAME "TestU64"
#define TEST_U64_STR_LEN 38

typedef struct test_u64 {
	uint64_t u64;
	bool valid;  // Not included on wire
} test_u64_t;

can_frame_t test_u64_to_can(const test_u64_t* msg);
test_u64_t can_to_test_u64(const can_frame_t* frame);
void print_test_u64(test_u64_t msg);
bool test_u64_to_str(test_u64_t msg, char out[TEST_U64_STR_LEN]);

#endif
