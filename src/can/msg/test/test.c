#include "test.h"

#include <stddef.h>
can_frame_t test_to_can(const test_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = TEST_ID;
	frame.len = TEST_LEN;
	frame.data[0] |= (uint8_t) ((msg->bool1 & 0x01) << 7);
	frame.data[0] |= (uint8_t) ((msg->bool2 & 0x01) << 6);
	frame.data[0] |= (uint8_t) ((msg->u8 >> 2) & 0x3F);
	frame.data[1] |= (uint8_t) ((msg->u8 & 0x03) << 6);
	frame.data[1] |= (uint8_t) ((msg->u16 >> 10) & 0x3F);
	frame.data[2] |= (uint8_t) ((msg->u16 >> 2) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->u16 & 0x03) << 6);
	frame.data[3] |= (uint8_t) ((msg->u32 >> 26) & 0x3F);
	frame.data[4] |= (uint8_t) ((msg->u32 >> 18) & 0xFF);
	frame.data[5] |= (uint8_t) ((msg->u32 >> 10) & 0xFF);
	frame.data[6] |= (uint8_t) ((msg->u32 >> 2) & 0xFF);
	frame.data[7] |= (uint8_t) ((msg->u32 & 0x03) << 6);
	return frame;
}
test_t can_to_test(const can_frame_t* frame) {
	test_t test;
	if(frame == NULL || frame->data == NULL || frame->len != TEST_LEN) {
		test.valid = false;
		return test;
	}

	uint8_t* data = frame->data;
	test.bool1 = ((data[0] >> 7) & 0x01);
	test.bool2 = ((data[0] >> 6) & 0x01);
	test.u8 = ((uint64_t)(data[0] & 0x3F) << 2) | ((data[1] >> 6) & 0x03);
	test.u16 = ((uint64_t)(data[1] & 0x3F) << 10) | ((uint64_t)(data[2] & 0xFF) << 2) | ((data[3] >> 6) & 0x03);
	test.u32 = ((uint64_t)(data[3] & 0x3F) << 26) | ((uint64_t)(data[4] & 0xFF) << 18) | ((uint64_t)(data[5] & 0xFF) << 10) | ((uint64_t)(data[6] & 0xFF) << 2) | ((data[7] >> 6) & 0x03);
	test.valid = true;
	return test;
}
void print_test(test_t msg) {
}
