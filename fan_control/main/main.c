#include <stdio.h>

#include "board.h"
#include "storage.h"
#include "button.h"
#include "wheel.h"
#include "fan.h"
#include "sys.h"
#include "log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LONG_PRESS_TIME     1000U //ms
#define LOOP_TIME           100U  //ms

#define BUTTON_QUEUE_DEPTH  10U

#define FAN_SPEED_KEY       "fan_speed"

typedef enum {
    APP_FAN_ON,
    APP_FAN_OFF,
} app_state_t;

// Local functions
void button_cb(button_handle_t handle, button_state_t state);
void advance_fan_speed(fan_speed_t *speed);

// App state
fan_speed_t speed = FAN_SPEED_1; // TODO: store in NVS
app_state_t app_state = APP_FAN_OFF;

// Queue for button change events
QueueHandle_t button_queue = NULL;

// Button state
bool pushed = false;
int push_time = 0;

// Objects
fan_handle_t *fan = NULL;
button_handle_t *button = NULL;

void app_main(void) 
{
    status_t status;

    // Set up queue
    button_queue = xQueueCreate(BUTTON_QUEUE_DEPTH, sizeof(uint8_t));

    // Set up storage
    status = storage_init();
    if (status != STATUS_OK)
    {
        // I don't think we return here, storage isn't a critical part
        ERROR("ERROR initializing storage");
    }

    // Load stored fan speed if it's there
    uint32_t fan_speed_data = 0;
    status = storage_read_u32(FAN_SPEED_KEY, &fan_speed_data);
    if (status == STATUS_OK)
    {
        INFO("Loading saved fan speed %lu", fan_speed_data);
        speed = (fan_speed_t) fan_speed_data;
    }

    // Set up the fan
    fan = fan_init(FAN_CHAN_1, BOARD_FAN_OUT_PIN);
    if (fan == NULL)
    {
        ERROR("ERROR initializing the fan controller");
        return;
    }

    // Init LED wheel
    wheel_init();
    wheel_update_level(FAN_SPEED_OFF);

    // Set up button
    button = button_init(BOARD_BUTTON_IN_PIN, BUTTON_POL_NEG);
    if (button == NULL)
    {
        ERROR("ERROR initializing the button controller");
        return;
    }

    status = button_assign_cb(button, button_cb);
    if (status != STATUS_OK)
    {
        ERROR("ERROR assigning button callback");
    }

    INFO("Init done!");

    while (1)
    {
        uint8_t button_state;
        if (xQueueReceive(button_queue, &button_state, pdMS_TO_TICKS(LOOP_TIME))) 
        {
            switch (app_state)
            {
                case APP_FAN_OFF:
                    if (button_state == BUTTON_STATE_OFF)
                    {
                        // If the fan is off, then turn it on as soon as the 
                        // button is pressed
                        INFO("Turning Fan ON with speed: %d", speed);
                        status = fan_set_speed(fan, speed);
                        wheel_update_level(speed);
                        app_state = APP_FAN_ON;
                    }
                    break;

                case APP_FAN_ON:
                    if(button_state == BUTTON_STATE_OFF)
                    {
                        // If button push has started, then record the time the 
                        // button press was started: we need to figure out if 
                        // its a short or long press
                        pushed = true;
                        push_time = millis();
                    }
                    else if (button_state == BUTTON_STATE_ON)
                    {
                        // If button push hasn't already been consumed (i.e. 
                        // long press)
                        if (pushed)
                        {
                            // Consume button push
                            pushed = false;

                            // Change fan speed and set
                            advance_fan_speed(&speed);
                            status = fan_set_speed(fan, speed);
                            wheel_update_level(speed);
                            if (status != STATUS_OK)
                            {
                                ERROR("ERROR setting fan speed: %u", (int)status);
                            }
                            INFO("New fan speed: %d", speed);
                        }
                    }
                    break;
            }
        }     

        // If button has been pressed more than the long press time, then it's 
        // a long press
        if (pushed && (millis() - push_time) >= LONG_PRESS_TIME)
        {
            // Consume button push
            pushed = false;
            app_state = APP_FAN_OFF;
            INFO("Turning fan OFF");

            // Set speed off
            status = fan_set_speed(fan, FAN_SPEED_OFF);
            wheel_update_level(FAN_SPEED_OFF);
            if (status != STATUS_OK)
            {
                ERROR("ERROR setting fan speed: %u", (int)status);
            }
        }

    }
}

void button_cb(button_handle_t handle, button_state_t state) 
{
    // Send button event to our state machine
    uint8_t byte_state = state & 0xFF;
    xQueueSend(button_queue, &byte_state, 0);
}

void advance_fan_speed(fan_speed_t *speed) 
{
    switch (*speed) 
    {
        case FAN_SPEED_ON:
            // Go back to the beginning
            *speed = FAN_SPEED_1;
            break;

        default:
            // Increment the fan speed by one
            *speed = *speed + 1;
            break;
    }

    // Save the modified fan speed
    status_t status = storage_write_u32(FAN_SPEED_KEY, (uint32_t) *speed);
    if (status != STATUS_OK)
    {
        ERROR("Error (%d) writing fan speed to storage", (int) status);
    }
}
