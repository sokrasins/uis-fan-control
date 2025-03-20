#include <stdio.h>

#include "board.h"
#include "button.h"
#include "fan.h"
#include "sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LONG_PRESS_TIME 1000U //ms
#define LOOP_TIME       100U  //ms

// Local functions
void button_cb(button_handle_t handle, button_state_t state);
void advance_fan_speed(fan_speed_t *speed);

// App state
fan_speed_t speed = FAN_SPEED_OFF;

// Button state
bool pushed = false;
int push_time = 0;

// Objects
fan_handle_t *fan = NULL;
button_handle_t *button = NULL;

void app_main(void) 
{
    status_t status;

    // Set up the fan
    fan = fan_init(FAN_CHAN_1, BOARD_FAN_OUT_PIN);
    if (fan == NULL)
    {
        printf("ERROR initializing the fan controller\r\n");
        return;
    }

    // Set up button
    button = button_init(BOARD_BUTTON_IN_PIN);
    if (button == NULL)
    {
        printf("ERROR initializing the button controller\r\n");
        return;
    }

    status = button_assign_cb(button, button_cb);
    if (status != STATUS_OK)
    {
        printf("ERROR assigning button callback\r\n");
    }

    printf("Init done!\r\n");

    while (1)
    {
        // Check for a long button press here
        if (pushed && (millis() - push_time) >= LONG_PRESS_TIME)
        {
            printf("Long button press detected, shutting off fan\r\n");
            pushed = false;
            speed = FAN_SPEED_OFF;

            status = fan_set_speed(fan, speed);
            if (status != STATUS_OK)
            {
                printf("ERROR setting fan speed: %u\r\n", (int)status);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_TIME));
    }
}

void button_cb(button_handle_t handle, button_state_t state) 
{
    status_t status;
    if(state == BUTTON_STATE_OFF)
    {
        pushed = true;
        push_time = millis();
    }
    else if (state == BUTTON_STATE_ON)
    {
        if (pushed)
        {
            pushed = false;
            // Advance fan speed
            advance_fan_speed(&speed);
            printf("new fan speed: %d\r\n", speed);
        
            // Set the new fan speed
            status = fan_set_speed(fan, speed);
            if (status != STATUS_OK)
            {
                printf("ERROR setting fan speed: %u\r\n", (int)status);
            }
        }
    }
}

void advance_fan_speed(fan_speed_t *speed) 
{
    switch (*speed) 
    {
        case FAN_SPEED_ON:
            // Go back to the beginning
            *speed = FAN_SPEED_OFF;
            break;

        default:
            // Increment the fan speed by one
            *speed = *speed + 1;
            break;
    }
}
