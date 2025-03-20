#include "fan.h"
#include "status.h"

#include "driver/ledc.h"

#define FAN_TIMER               LEDC_TIMER_0
#define FAN_PWM_MODE            LEDC_LOW_SPEED_MODE
#define FAN_PWM_CHANNEL         LEDC_CHANNEL_0
#define FAN_DUTY_RES            LEDC_TIMER_13_BIT
#define FAN_FREQUENCY           (5000U)
#define FAN_DUTY_COUNT          (1 << FAN_DUTY_RES)

/// @brief Map from fan_speed_t to % duty
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

typedef struct {
    ledc_mode_t mode;
    ledc_channel_t chan;
} fan_ctx_t;

static fan_ctx_t _ctx = {0, };

fan_handle_t fan_init(fan_chan_t chan, int pin) {
    
    if (chan != FAN_CHAN_1)
    {
        return NULL;
    }

    _ctx.mode = FAN_PWM_MODE;
    _ctx.chan = FAN_PWM_CHANNEL;

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = _ctx.mode,
        .timer_num        = FAN_TIMER,
        .duty_resolution  = FAN_DUTY_RES,
        .freq_hz          = FAN_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = _ctx.mode,
        .channel        = _ctx.chan,
        .timer_sel      = FAN_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = pin,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    return (fan_handle_t *) &_ctx;
}

status_t fan_set_speed(fan_handle_t *handle, fan_speed_t speed) 
{
    if (handle == NULL)
    {
        return STATUS_BAD_PARAM;
    }

    if (speed >= FAN_SPEED_MAX)
    {
        return STATUS_BAD_PARAM;
    }

    fan_ctx_t *ctx = (fan_ctx_t *) handle;
    uint32_t duty = (uint32_t) (FAN_DUTY_COUNT * (fan_speed_dict[speed] / 100.0));

    ESP_ERROR_CHECK(ledc_set_duty(ctx->mode, ctx->chan, duty));
    ESP_ERROR_CHECK(ledc_update_duty(ctx->mode, ctx->chan));

    return STATUS_OK;
}