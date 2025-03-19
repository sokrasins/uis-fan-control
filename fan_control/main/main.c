#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_LO            (2048) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_DUTY_HI            (2048 + 4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 4 kHz

#define DUTY_MAX                (8192U)

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
    FAN_SPEED_NUM,
} fan_speed_t;

int set_fan_speed(fan_speed_t speed);

float fan_speed_dict[] = {
    0.0,
    21.5,
    29.5,
    37.5,
    46.5,
    54.5,
    62.5,
    70.5,
    79.5,
    87.5,
    100.0
};

fan_speed_t speed = FAN_SPEED_OFF;

void app_main(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    
    while (1)
    {

        int rc = set_fan_speed(speed);
        if (rc != 0)
        {
            printf("ERROR setting fan speed\r\n");
        }

        // Update Duty cycle
        switch (speed)
        {
            case FAN_SPEED_ON:
                speed = FAN_SPEED_OFF;
                break;

            default:
                speed += 1;
                break;
        }
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

        // Wait
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

int set_fan_speed(fan_speed_t speed)
{
    if (speed >= FAN_SPEED_NUM)
    {
        return -1;
    }

    float pcnt = fan_speed_dict[speed];

    uint32_t duty = (uint32_t) (DUTY_MAX * (pcnt / 100.0));
    printf("Setting duty cycle to %lu\r\n", duty);

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    return 0;
}