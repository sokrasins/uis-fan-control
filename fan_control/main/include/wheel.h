#ifndef WHEEL_H_
#define WHEEL_H_

#include "status.h"
#include "fan.h"

status_t wheel_init(void);

status_t wheel_update_level(fan_speed_t level);

#endif /*WHEEL_H_*/