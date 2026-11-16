#include "../../logger/logger.h"
#include "../../can/msg/cpu_info/cpu_info.h"
#include <unistd.h>
#include "../../can/utils.h"
#include "stdbool.h"

typedef struct system_cpu_info {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
    unsigned long long guest;
    unsigned long long guest_nice;
    unsigned long long total; // Sum of all the other values
} system_cpu_info_t;

#define MAX_CPU_LINE 1000


bool starts_with(char* str, char* seq){
    if (str == NULL || seq == NULL) {
        return false;
    }

    return strncmp(str, seq, strlen(seq)) == 0;
}

void read_system_cpu_info(FILE* fp, system_cpu_info_t* cpu, char line[MAX_CPU_LINE]) {
    fseek(fp, 0, SEEK_SET);
    while (fgets(line, MAX_CPU_LINE, fp) != NULL) {
        size_t end_index = strcspn(line, "\r\n");
        line[end_index] = '\0';
        
        if (starts_with(line, "cpu ")) {
            sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
                &cpu->user, &cpu->nice, &cpu->system, &cpu->idle, &cpu->iowait, &cpu->irq, &cpu->softirq, &cpu->steal, &cpu->guest, &cpu->guest_nice);
            // Don't include guest values in total since they are always included in user or nice
            cpu->total = cpu->user + cpu->nice + cpu->system + cpu->idle + cpu->iowait + cpu->irq + cpu->softirq + cpu->steal;
        } 
    }
}

// Return percent * 10 (1000 = 100%)
uint16_t calculate_system_cpu_usage(system_cpu_info_t* prev, system_cpu_info_t* curr) {
    // CPU util = 100% - idle_percent
    // idle_percent = idle_delta / total_delta_ticks

    unsigned long long total_delta = curr->total - prev->total;
    if (total_delta == 0) {
        return 0;
    }
    
    double idle_pct = (curr->idle - prev->idle) / (double) total_delta * 100;
    double cpu_util_pct = 100.0 - idle_pct;
    return (uint16_t) (cpu_util_pct * 10 + 0.5); // Add 0.5 to round, not truncate
}


int main(void) {
    logger_t* logger = create_logger("cpu", E_BOTH);
    if (logger == NULL) {
        fprintf(stderr, "Error creating logger\n");
        return 1;
    }

    int can_sock_fd = open_can_socket("vcan0");
    if (can_sock_fd < 0) {
        LOG_ERR(logger, "Error opening vcan0 socket\n");
        return 1;
    }

    uint64_t last_send_time = millis();
    bool keep_running = true;

    char cpu_info_str[CPU_INFO_STR_LEN];

    char system_cpu_info_str[MAX_CPU_LINE];
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        LOG_ERR(logger, "cannot open /proc/stat");
        return 1;
    }

    // Seed with info
    system_cpu_info_t prev_system_cpu_info;
    system_cpu_info_t curr_system_cpu_info;

    read_system_cpu_info(fp, &prev_system_cpu_info, system_cpu_info_str);

    cpu_info_t cpu_info = {0};
    while (keep_running) {
        uint64_t now = millis();


        uint64_t elapsed = now - last_send_time;
        if (elapsed >= CPU_INFO_INTERVAL_MS) {
            // Read CPU info from /proc/stat
            read_system_cpu_info(fp, &curr_system_cpu_info, system_cpu_info_str);
            cpu_info.cpu_load_pct_x10 = calculate_system_cpu_usage(&prev_system_cpu_info, &curr_system_cpu_info);
            prev_system_cpu_info = curr_system_cpu_info;

            // Write CPUInfo to CAN bus
            can_frame_t frame = cpu_info_to_can(&cpu_info);
            SockResult_t result = write_to_socket(can_sock_fd, &frame);
            if (result != SOCK_OK) {
                LOG_ERR(logger, "write_to_socket error: %s\n", sock_result_str(result));
                return 1;
            }

            if(!cpu_info_to_str(cpu_info, cpu_info_str)) {
                LOG_ERR(logger, "cpu_info_to_str failed");
            } else {
                LOG_INFO(logger, "Sent: %s", cpu_info_str);
            }

            last_send_time = now;
        }
    }

    free_logger(logger);
    return 0;
}