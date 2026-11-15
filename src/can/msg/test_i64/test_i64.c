#include "test_i64.h"
#include <stddef.h>

can_frame_t test_i64_to_can(const test_i64_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = TEST_I64_ID;
	frame.len = TEST_I64_LEN;
	frame.data[0] |= (uint8_t) ((msg->i64 >> 56) & 0xFF);
	frame.data[1] |= (uint8_t) ((msg->i64 >> 48) & 0xFF);
	frame.data[2] |= (uint8_t) ((msg->i64 >> 40) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->i64 >> 32) & 0xFF);
	frame.data[4] |= (uint8_t) ((msg->i64 >> 24) & 0xFF);
	frame.data[5] |= (uint8_t) ((msg->i64 >> 16) & 0xFF);
	frame.data[6] |= (uint8_t) ((msg->i64 >> 8) & 0xFF);
	frame.data[7] |= (uint8_t) (msg->i64 & 0xFF);
	return frame;
}

test_i64_t can_to_test_i64(const can_frame_t* frame) {
	test_i64_t test_i64;
	if(frame == NULL || frame->data == NULL || frame->len != TEST_I64_LEN) {
		test_i64.valid = false;
		return test_i64;
	}

	uint8_t* data = frame->data;
	test_i64.i64 = ((uint64_t)(data[0] & 0xFF) << 56) | ((uint64_t)(data[1] & 0xFF) << 48) | ((uint64_t)(data[2] & 0xFF) << 40) | ((uint64_t)(data[3] & 0xFF) << 32) | ((uint64_t)(data[4] & 0xFF) << 24) | ((uint64_t)(data[5] & 0xFF) << 16) | ((uint64_t)(data[6] & 0xFF) << 8) | (data[7] & 0xFF);
	test_i64.valid = true;
	return test_i64;
}

void print_test_i64(test_i64_t msg) {
}

bool test_i64_to_str(test_i64_t msg, char out[TEST_I64_STR_LEN]) {
	int written = snprintf(out, "TEST_I64: {i64: %ld}", msg.i64);
	return written == strlen(out);
}
