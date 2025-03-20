#include "sys.h"
#include "log.h"

#include "esp_timer.h"

int64_t millis(void)
{
    return esp_timer_get_time() / 1000;
}
