
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "power_status.h"

can_frame_t power_status_to_can(const power_status_t* status) {
    can_frame_t frame = {0};
    if (status == NULL) {
        frame.can_id = INVALID_ID;
        return frame;
    }
    frame.can_id = POWER_STATUS_ID;
    frame.len = POWER_STATUS_DLC;
    frame.data[0] |= (status->percent_charge & 0x7F) << 0;
    frame.data[0] |= (status->status & 0x01) << 7;
    frame.data[1] |= ((status->status >> 1) & 0x01) << 0;
    frame.data[1] |= (status->power_on & 0x01) << 1;

    return frame;
}

power_status_t can_to_power_status(const can_frame_t* frame) {
    power_status_t status;
    if (frame == NULL || frame->len != POWER_STATUS_DLC) {
        status.valid = false;
        return status;
    }

    status.valid = true;
    status.percent_charge = (frame->data[0] & 0x7F);
    status.status = ((frame->data[0] >> 7) & 0x01) | (((frame->data[1] >> 0) & 0x01) << 1); 
    status.power_on = (bool) ((frame->data[1] >> 1) & 0x01);

    return status;
}

void print_power_status(power_status_t status) {
    printf("power_status: %s\n", status.valid ? "OK" : "ERR");
    printf("  percent_charge: %d\n", status.percent_charge);
    printf("  status: %d\n", status.status);
    printf("  power_on: %s\n", str_bool(status.power_on));
}