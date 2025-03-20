#include "sys.h"

#include "esp_log.h"
#include "esp_timer.h"

// Logging setup
static const char* TAG = "sys";
#define INFO(format, ...)  ESP_LOGI(TAG, format, __VA_ARGS__)
#define ERROR(format, ...) ESP_LOGE(TAG, format, __VA_ARGS__)
#define DEBUG(format, ...) ESP_LOGD(TAG, format, __VA_ARGS__)

int64_t millis(void)
{
    return esp_timer_get_time() / 1000;
}
