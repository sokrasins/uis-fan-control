#ifndef FAN_H_
#define FAN_H_

#include "status.h"

typedef void * fan_handle_t;

typedef enum {
    FAN_SPEED_OFF,
    FAN_SPEED_1,
    FAN_SPEED_2,
    FAN_SPEED_3,
    FAN_SPEED_4,
    FAN_SPEED_5,
    FAN_SPEED_6,
    FAN_SPEED_7,
    FAN_SPEED_8,
    FAN_SPEED_9,
    FAN_SPEED_ON,
    FAN_SPEED_MAX,
} fan_speed_t;

typedef enum {
    FAN_CHAN_1,
    FAN_CHAN_MAX,
} fan_chan_t;

fan_handle_t fan_init(fan_chan_t chan, int pin);

status_t fan_set_speed(fan_handle_t *handle, fan_speed_t speed);

#endif /* FAN_H_*/