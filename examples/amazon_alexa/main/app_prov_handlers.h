#pragma once

char *custom_config_storage_get(const char *key);
void custom_config_storage_set_u8(const char *key,uint8_t value);
uint8_t custom_config_storage_get_u8(const char *key);
enum{
    CUSTOM_INVALID,
    CUSTOM_INIT,
    APP_OTA_FAIL,
    APP_OTA_OK,
    FORCE_OTA_INIT,
    FORCE_OTA_START,
    FORCE_OTA_FINISH,
    FORCE_OTA_UPDATE,
    MAX_OTA_CUSTOM
};