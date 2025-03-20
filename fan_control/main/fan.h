#ifndef FAN_H_
#define FAN_H_

#include "status.h"

/// @brief Settable fan speed levels
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

/// @brief Available fan channels
typedef enum {
    FAN_CHAN_1,
    FAN_CHAN_MAX,
} fan_chan_t;

/// @brief Handle for fan instance
typedef void * fan_handle_t;

/**
 * @brief Initialize a fan
 * @param chan the channel for the fan control. Each channel can only be 
 * assigned to a single pin.
 * @param pin hardware pin the fan is connected to
 * @return NULL if error, handle if successful
 */
fan_handle_t fan_init(fan_chan_t chan, int pin);

/**
 * @brief Set the speed of the fan instance
 * @param handle
 * @param speed
 * @return STATUS_OK if successful
 *         STATUS_BAD_PARAM if handle is NULL
 */
status_t fan_set_speed(fan_handle_t *handle, fan_speed_t speed);

#endif /* FAN_H_*/