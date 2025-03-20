#ifndef LOG_H_
#define LOG_H_

#include "esp_log.h"

#define INFO(format, ...)   ESP_LOGI(__FILE__, format, ##__VA_ARGS__)
#define ERROR(format, ...)  ESP_LOGE(__FILE__, format, ##__VA_ARGS__)
#define DEBUG(format, ...)  ESP_LOGD(__FILE__, format, ##__VA_ARGS__)

#endif /*LOG_H_*/