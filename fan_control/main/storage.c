#include "storage.h"

#include "log.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

typedef struct {
    nvs_handle_t nvs;
} storage_ctx_t;

storage_ctx_t _ctx;

status_t storage_init(void)
{
    DEBUG("Initializing NVS flash");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    DEBUG("Opening Non-Volatile Storage (NVS) handle");
    err = nvs_open("storage", NVS_READWRITE, &_ctx.nvs);
    if (err != ESP_OK) 
    {
        ERROR("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    DEBUG("Storage init done\n");

    return STATUS_OK;
}

status_t storage_read_u32(char *key, uint32_t *data)
{
    status_t ret;

    esp_err_t err = nvs_get_u32(_ctx.nvs, key, data);
    switch (err) {
        case ESP_OK:
            ret = STATUS_OK;
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ret = STATUS_NOT_FOUND;
            ERROR("Storage key is not initialized yet!");
            break;
        default:
            ret = STATUS_BSP_ERR;
            ERROR("Error (%s) reading storage key %s", esp_err_to_name(err), key);
    }

    return ret;
}

status_t storage_write_u32(char *key, uint32_t data)
{
    esp_err_t err = nvs_set_u32(_ctx.nvs, key, data);
    if (err != ESP_OK)
    {
        ERROR("Error (%s) writing storage key %s", esp_err_to_name(err), key);
        return STATUS_BSP_ERR;
    }

    // Commit written value
    // TODO: This could be done in a task to "debounce" the changes
    DEBUG("Committing updates in NVS");
    err = nvs_commit(_ctx.nvs);
    if (err != ESP_OK)
    {
        ERROR("Error (%s) committing update", esp_err_to_name(err));
        return STATUS_BSP_ERR;
    }

    return STATUS_OK;
}