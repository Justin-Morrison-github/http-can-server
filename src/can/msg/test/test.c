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
	test.u8 = ((data[0] & 0x3F) << 2) | ((data[1] >> 6) & 0x03);
	test.u16 = ((data[1] & 0x3F) << 10) | ((data[2] & 0xFF) << 2) | ((data[3] >> 6) & 0x03);
	test.u32 = ((data[3] & 0x3F) << 26) | ((data[4] & 0xFF) << 18) | ((data[5] & 0xFF) << 10) | ((data[6] & 0xFF) << 2) | ((data[7] >> 6) & 0x03);
	test.valid = true;
	return test;
}
void print_test(test_t msg) {
}
