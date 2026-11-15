#include "test_u64.h"
#include <stddef.h>

can_frame_t test_u64_to_can(const test_u64_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = TEST_U64_ID;
	frame.len = TEST_U64_LEN;
	frame.data[0] |= (uint8_t) ((msg->u64 >> 56) & 0xFF);
	frame.data[1] |= (uint8_t) ((msg->u64 >> 48) & 0xFF);
	frame.data[2] |= (uint8_t) ((msg->u64 >> 40) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->u64 >> 32) & 0xFF);
	frame.data[4] |= (uint8_t) ((msg->u64 >> 24) & 0xFF);
	frame.data[5] |= (uint8_t) ((msg->u64 >> 16) & 0xFF);
	frame.data[6] |= (uint8_t) ((msg->u64 >> 8) & 0xFF);
	frame.data[7] |= (uint8_t) (msg->u64 & 0xFF);
	return frame;
}

test_u64_t can_to_test_u64(const can_frame_t* frame) {
	test_u64_t test_u64;
	if(frame == NULL || frame->data == NULL || frame->len != TEST_U64_LEN) {
		test_u64.valid = false;
		return test_u64;
	}

	uint8_t* data = frame->data;
	test_u64.u64 = ((uint64_t)(data[0] & 0xFF) << 56) | ((uint64_t)(data[1] & 0xFF) << 48) | ((uint64_t)(data[2] & 0xFF) << 40) | ((uint64_t)(data[3] & 0xFF) << 32) | ((uint64_t)(data[4] & 0xFF) << 24) | ((uint64_t)(data[5] & 0xFF) << 16) | ((uint64_t)(data[6] & 0xFF) << 8) | (data[7] & 0xFF);
	test_u64.valid = true;
	return test_u64;
}

void print_test_u64(test_u64_t msg) {
}

bool test_u64_to_str(test_u64_t msg, char out[TEST_U64_STR_LEN]) {
	int written = snprintf(out, "TEST_U64: {u64: %lu}", msg.u64);
	return written == strlen(out);
}
