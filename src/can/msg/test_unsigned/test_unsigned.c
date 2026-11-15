#include "test_unsigned.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

can_frame_t test_unsigned_to_can(const test_unsigned_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = TEST_UNSIGNED_ID;
	frame.len = TEST_UNSIGNED_LEN;
	frame.data[0] |= (uint8_t) ((msg->flag & 0x01) << 7);
	frame.data[0] |= (uint8_t) ((msg->u8 >> 1) & 0x7F);
	frame.data[1] |= (uint8_t) ((msg->u8 & 0x01) << 7);
	frame.data[1] |= (uint8_t) ((msg->u16 >> 9) & 0x7F);
	frame.data[2] |= (uint8_t) ((msg->u16 >> 1) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->u16 & 0x01) << 7);
	frame.data[3] |= (uint8_t) ((msg->u32 >> 25) & 0x7F);
	frame.data[4] |= (uint8_t) ((msg->u32 >> 17) & 0xFF);
	frame.data[5] |= (uint8_t) ((msg->u32 >> 9) & 0xFF);
	frame.data[6] |= (uint8_t) ((msg->u32 >> 1) & 0xFF);
	frame.data[7] |= (uint8_t) ((msg->u32 & 0x01) << 7);
	return frame;
}

test_unsigned_t can_to_test_unsigned(const can_frame_t* frame) {
	test_unsigned_t test_unsigned;
	if(frame == NULL || frame->data == NULL || frame->len != TEST_UNSIGNED_LEN) {
		test_unsigned.valid = false;
		return test_unsigned;
	}

	const uint8_t* data = frame->data;
	test_unsigned.flag = ((data[0] >> 7) & 0x01);
	test_unsigned.u8 = ((uint64_t)(data[0] & 0x7F) << 1) | ((data[1] >> 7) & 0x01);
	test_unsigned.u16 = ((uint64_t)(data[1] & 0x7F) << 9) | ((uint64_t)(data[2] & 0xFF) << 1) | ((data[3] >> 7) & 0x01);
	test_unsigned.u32 = ((uint64_t)(data[3] & 0x7F) << 25) | ((uint64_t)(data[4] & 0xFF) << 17) | ((uint64_t)(data[5] & 0xFF) << 9) | ((uint64_t)(data[6] & 0xFF) << 1) | ((data[7] >> 7) & 0x01);
	test_unsigned.valid = true;
	return test_unsigned;
}

void print_test_unsigned(test_unsigned_t msg) {
}

bool test_unsigned_to_str(test_unsigned_t msg, char out[TEST_UNSIGNED_STR_LEN]) {
	int written = snprintf(out, TEST_UNSIGNED_STR_LEN, "TEST_UNSIGNED: {flag: %s, u8: %u, u16: %u, u32: %u}", str_bool(msg.flag), msg.u8, msg.u16, msg.u32);
	return written == strlen(out);
}

bool can_to_test_unsigned_str(const can_frame_t* frame, char out[TEST_UNSIGNED_STR_LEN]) {
	return test_unsigned_to_str(can_to_test_unsigned(frame), out);
}

