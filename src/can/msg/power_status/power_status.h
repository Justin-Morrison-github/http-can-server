#ifndef POWER_STATUS_H
#define POWER_STATUS_H

#include <stdint.h>
#include <stdbool.h>
#include "../../utils.h"

#define INVALID_ID 0
#define POWER_STATUS_ID 0x444
#define POWER_STATUS_DLC 2
#define POWER_STATUS_INTERVAL_MS 1000
#define POWER_STATUS_NAME "PowerStatus"

typedef struct power_status {
    uint8_t percent_charge;
    bool power_on;
    bool valid;
} power_status_t;
can_frame_t power_status_to_can(const power_status_t* status);
power_status_t can_to_power_status(const can_frame_t* frame);
void print(power_status_t status);
#endif