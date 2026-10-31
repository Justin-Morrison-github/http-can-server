
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "power_status.h"

can_frame_t power_status_to_can(const power_status_t* status) {
    can_frame_t frame;
    if (status == NULL) {
        frame.can_id = INVALID_ID;
        return frame;
    }
    frame.can_id = POWER_STATUS_ID;
    frame.len = POWER_STATUS_DLC;
    memcpy(&frame.data[0], &status->percent_charge, sizeof(status->percent_charge));
    memcpy(&frame.data[1], &status->power_on, sizeof(status->power_on));

    return frame;
}

power_status_t can_to_power_status(const can_frame_t* frame) {
    power_status_t status;
    if (frame == NULL || frame->len != POWER_STATUS_DLC) {
        status.valid = false;
        return status;
    }

    status.valid = true;
    status.percent_charge = frame->data[0];
    status.power_on = (bool) frame->data[1];

    return status;
}

void print_power_status(power_status_t status) {
    printf("power_status: %s\n", status.valid ? "OK" : "ERR");
    printf("  percent_charge: %d\n", status.percent_charge);
    printf("  power_on: %s\n", str_bool(status.power_on));
}