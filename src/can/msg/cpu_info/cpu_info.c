#include "cpu_info.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>

can_frame_t cpu_info_to_can(const cpu_info_t* msg) {
	can_frame_t frame = {0};
	if(msg == NULL) {
		frame.can_id = INVALID_ID;
		return frame;
	}

	frame.can_id = CPU_INFO_ID;
	frame.len = CPU_INFO_LEN;
	frame.data[0] |= (uint8_t) ((msg->cpu_temp_mC >> 8) & 0xFF);
	frame.data[1] |= (uint8_t) (msg->cpu_temp_mC & 0xFF);
	frame.data[2] |= (uint8_t) ((msg->cpu_load_pct_x10 >> 2) & 0xFF);
	frame.data[3] |= (uint8_t) ((msg->cpu_load_pct_x10 & 0x03) << 6);
	frame.data[3] |= (uint8_t) (msg->cpu_num_cores & 0x3F);
	return frame;
}

cpu_info_t can_to_cpu_info(const can_frame_t* frame) {
	cpu_info_t cpu_info;
	if(frame == NULL || frame->data == NULL || frame->len != CPU_INFO_LEN) {
		cpu_info.valid = false;
		return cpu_info;
	}

	const uint8_t* data = frame->data;
	cpu_info.cpu_temp_mC = ((uint64_t)(data[0] & 0xFF) << 8) | (data[1] & 0xFF);
	cpu_info.cpu_load_pct_x10 = ((uint64_t)(data[2] & 0xFF) << 2) | ((data[3] >> 6) & 0x03);
	cpu_info.cpu_num_cores = (data[3] & 0x3F);
	cpu_info.valid = true;
	return cpu_info;
}

void print_cpu_info(cpu_info_t msg) {
}

bool cpu_info_to_str(cpu_info_t msg, char out[CPU_INFO_STR_LEN]) {
	int written = snprintf(out, CPU_INFO_STR_LEN, "CPU_INFO: {cpu_temp_mC: %u, cpu_load_pct_x10: %u, cpu_num_cores: %u}", msg.cpu_temp_mC, msg.cpu_load_pct_x10, msg.cpu_num_cores);
	return written == strlen(out);
}

bool can_to_cpu_info_str(const can_frame_t* frame, char out[CPU_INFO_STR_LEN]) {
	return cpu_info_to_str(can_to_cpu_info(frame), out);
}

