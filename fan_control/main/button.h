#ifndef BUTTON_H_
#define BUTTON_H_

#include "status.h"

typedef enum {
    BUTTON_STATE_OFF,
    BUTTON_STATE_ON,
} button_state_t;

typedef void *button_handle_t;
typedef void (*button_cb_t)(button_handle_t handle, button_state_t state);

button_handle_t button_init(int button_pin);

status_t button_assign_cb(button_handle_t handle, button_cb_t cb);

static inline button_state_t button_state_from_int(int i)
{
    if (i == 0) {
        return BUTTON_STATE_ON;
    }

    return BUTTON_STATE_OFF;
};

#endif /*BUTTON_H_*/