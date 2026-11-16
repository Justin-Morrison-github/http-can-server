#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define CPU_INFO_ID 0x555
#define CPU_INFO_LEN 4
#define CPU_INFO_INTERVAL_MS 1000
#define CPU_INFO_NAME "CpuInfo"
#define CPU_INFO_STR_LEN 76

typedef struct cpu_info {
	uint16_t cpu_temp_mC;
	uint16_t cpu_load_pct_x10;
	uint8_t cpu_num_cores;
	bool valid;  // Not included on wire
} cpu_info_t;

can_frame_t cpu_info_to_can(const cpu_info_t* msg);
cpu_info_t can_to_cpu_info(const can_frame_t* frame);
void print_cpu_info(cpu_info_t msg);
bool cpu_info_to_str(cpu_info_t msg, char out[CPU_INFO_STR_LEN]);
bool can_to_cpu_info_str(const can_frame_t* frame, char out[CPU_INFO_STR_LEN]);

#endif
