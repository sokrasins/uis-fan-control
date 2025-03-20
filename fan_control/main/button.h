#ifndef BUTTON_H_
#define BUTTON_H_

#include "status.h"

/// @brief Instantaneous state of button
typedef enum {
    BUTTON_STATE_OFF,
    BUTTON_STATE_ON,
} button_state_t;

/// @brief Handle for button instance
typedef void *button_handle_t;

/**
 * @brief Called when the button associated with handle changes state
 * @param handle button for which the state changed
 * @param state new state of the button
 */
typedef void (*button_cb_t)(button_handle_t handle, button_state_t state);

/**
 * @brief Initialize a button
 * @param button_pin hardware pin the button is connected to
 * @return NULL if error, handle if successful
 */
button_handle_t button_init(int button_pin);

/**
 * @brief Assign a callback to a handle. When the button associated with handle
 * changes state, the callback will be called.
 * @param handle 
 * @param cb 
 * @return STATUS_OK if successful
 *         STATUS_BAD_PARAM if handle is NULL
 */
status_t button_assign_cb(button_handle_t handle, button_cb_t cb);

/**
 * @brief Get the instantaneous button state
 * @param handle of button
 * @return BUTTON_STATE_OFF if button is not pushed
 *         BUTTON_STATE_ON if button is pushed
 */
button_state_t button_get_state(button_handle_t handle);

/**
 * @brief Convert integer to button_state_t
 * @param i integer to convert
 * @return BUTTON_STATE_OFF if i is 0
 *         BUTTON_STATE_ON if i is not 0
 */
static inline button_state_t button_state_from_int(int i)
{
    if (i == 0) 
    {
        return BUTTON_STATE_ON;
    }

    return BUTTON_STATE_OFF;
};

#endif /*BUTTON_H_*/