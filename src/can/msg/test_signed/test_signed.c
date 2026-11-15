#include "test_signed.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

can_frame_t test_signed_to_can(const test_signed_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = TEST_SIGNED_ID;
	frame.len = TEST_SIGNED_LEN;
	frame.data[0] |= (uint8_t) ((msg->flag & 0x01) << 7);
	frame.data[0] |= (uint8_t) ((msg->i8 >> 1) & 0x7F);
	frame.data[1] |= (uint8_t) ((msg->i8 & 0x01) << 7);
	frame.data[1] |= (uint8_t) ((msg->i16 >> 9) & 0x7F);
	frame.data[2] |= (uint8_t) ((msg->i16 >> 1) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->i16 & 0x01) << 7);
	frame.data[3] |= (uint8_t) ((msg->i32 >> 25) & 0x7F);
	frame.data[4] |= (uint8_t) ((msg->i32 >> 17) & 0xFF);
	frame.data[5] |= (uint8_t) ((msg->i32 >> 9) & 0xFF);
	frame.data[6] |= (uint8_t) ((msg->i32 >> 1) & 0xFF);
	frame.data[7] |= (uint8_t) ((msg->i32 & 0x01) << 7);
	return frame;
}

test_signed_t can_to_test_signed(const can_frame_t* frame) {
	test_signed_t test_signed;
	if(frame == NULL || frame->data == NULL || frame->len != TEST_SIGNED_LEN) {
		test_signed.valid = false;
		return test_signed;
	}

	const uint8_t* data = frame->data;
	test_signed.flag = ((data[0] >> 7) & 0x01);
	test_signed.i8 = ((uint64_t)(data[0] & 0x7F) << 1) | ((data[1] >> 7) & 0x01);
	test_signed.i16 = ((uint64_t)(data[1] & 0x7F) << 9) | ((uint64_t)(data[2] & 0xFF) << 1) | ((data[3] >> 7) & 0x01);
	test_signed.i32 = ((uint64_t)(data[3] & 0x7F) << 25) | ((uint64_t)(data[4] & 0xFF) << 17) | ((uint64_t)(data[5] & 0xFF) << 9) | ((uint64_t)(data[6] & 0xFF) << 1) | ((data[7] >> 7) & 0x01);
	test_signed.valid = true;
	return test_signed;
}

void print_test_signed(test_signed_t msg) {
}

bool test_signed_to_str(test_signed_t msg, char out[TEST_SIGNED_STR_LEN]) {
	int written = snprintf(out, TEST_SIGNED_STR_LEN, "TEST_SIGNED: {flag: %s, i8: %d, i16: %d, i32: %d}", str_bool(msg.flag), msg.i8, msg.i16, msg.i32);
	return written == strlen(out);
}
