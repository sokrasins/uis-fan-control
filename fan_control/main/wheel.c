#include "wheel.h"
#include "board.h"
#include "log.h"

#include "driver/gpio.h"

static uint32_t led_pin[] = {
    BOARD_LED1_PIN,
    BOARD_LED2_PIN,
    BOARD_LED3_PIN,
    BOARD_LED4_PIN,
    BOARD_LED5_PIN,
    BOARD_LED6_PIN,
    BOARD_LED7_PIN,
    BOARD_LED8_PIN,
    BOARD_LED9_PIN,
    BOARD_LED10_PIN,
};

status_t wheel_init(void)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 0;

    for (int i=0; i<10; i++)
    {
        io_conf.pin_bit_mask |= 1 << led_pin[i];    
    }
    
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    return STATUS_OK;
}

status_t wheel_update_level(fan_speed_t speed)
{
    for (int i=0; i<10; i++)
    {
        if (i < speed)
        {
            // Turns LED on
            gpio_set_level(led_pin[i], 0);
        }
        else
        {
            // Turns LED off
            gpio_set_level(led_pin[i], 1);
        }
    }

    return STATUS_OK;
}