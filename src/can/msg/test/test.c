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
	if(frame == NULL || frame->len != TEST_LEN) {
		test.valid = false;
		return test;
	}

	// TODO finish data unpacking
	test.valid = true;
	return test;
}
void print_test(test_t msg) {
}
