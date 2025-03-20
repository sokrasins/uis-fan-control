#include "button.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define BUTTON_QUEUE_DEPTH 10U

typedef struct {
    int pin;
    button_state_t last_state;
    button_cb_t cb;
} button_ctx_t;

static void button_task(void* arg);

static button_ctx_t _ctx;
QueueHandle_t _evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    // Handle the event in our task
    button_ctx_t *ctx = (button_ctx_t *) arg;
    xQueueSendFromISR(_evt_queue, &ctx, NULL);
}

button_handle_t button_init(int button_pin)
{
    _ctx.pin = button_pin;

    if (_evt_queue == NULL)
    {
        _evt_queue = xQueueCreate(BUTTON_QUEUE_DEPTH, sizeof(button_ctx_t *));
    }

    uint64_t pin_sel = (1ULL<<_ctx.pin);

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = pin_sel;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    _ctx.last_state = gpio_get_level(_ctx.pin);

    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    // Install gpio isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(button_pin, gpio_isr_handler, (void*) &_ctx);

    _ctx.cb = NULL;

    return (button_handle_t) &_ctx;
}

status_t button_assign_cb(button_handle_t handle, button_cb_t cb)
{
    if (handle == NULL)
    {
        return STATUS_BAD_PARAM;
    }

    button_ctx_t *ctx = (button_ctx_t *) handle;
    ctx->cb = cb;

    return STATUS_OK;
}

button_state_t button_get_state(button_handle_t handle)
{
    if (handle == NULL)
    {
        return STATUS_BAD_PARAM;
    }

    button_ctx_t *ctx = (button_ctx_t *) handle;
    return button_state_from_int(gpio_get_level(ctx->pin));
}

static void button_task(void* arg)
{
    button_ctx_t *ctx;
    for (;;) 
    {
        if (xQueueReceive(_evt_queue, &ctx, portMAX_DELAY)) 
        {
            button_state_t cur_state = button_get_state(ctx);
            if (ctx->last_state != cur_state) 
            {
                if (ctx->cb)
                {
                    ctx->cb((button_handle_t *)ctx, cur_state);
                }
                ctx->last_state = cur_state;
            }
        }
    }
}
