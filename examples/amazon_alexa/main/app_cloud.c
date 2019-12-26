// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include "sdkconfig.h"

// #ifdef CONFIG_ALEXA_ENABLE_OTA

#include <esp_cloud_storage.h>
#include <esp_cloud_ota.h>
#include <esp_https_ota.h>

#include <esp_log.h>
#include <esp_timer.h>
#include <voice_assistant.h>
#include <va_led.h>
#include <alexa_local_config.h>
#include "app_cloud.h"

#include "cJSON.h"
#include "nvs_flash.h"
#include "app_auth_user.h"
#include "va_button.h"
#include "app_driver.h"
#include "app_prov_handlers.h"
static const char *TAG = "[app_cloud]";

static esp_cloud_handle_t esp_cloud_handle;
int  ota_size = 0;
char ota_ver[10] = {0};
char ota_url[255] = {0};
esp_err_t app_cloud_perform(esp_cloud_ota_handle_t ota_handle, const char *url, void *priv)
{
    va_led_set(LED_OFF);
    alexa_local_config_stop();
    va_reset();
    va_led_set(LED_OTA);
    ESP_LOGI(TAG, "app_cloud_perform called");
    if (!url) {
        return ESP_FAIL;
    }
    char *ota_server_cert = esp_cloud_storage_get("ota_server_cert");
    if (!ota_server_cert) {
        // esp_cloud_report_ota_status(ota_handle, OTA_STATUS_FAILED, "Server Certificate Absent");
        ota_report_msg_status_val_to_app(OTA_FAIL_1);
        return ESP_FAIL;
    }
    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = ota_server_cert,
        .buffer_size = 1024,
        .timeout_ms = 60000
    };

    esp_err_t err = esp_https_ota(&config);
    
    free(ota_server_cert);
    va_led_set(LED_OFF);
    if (err == ESP_OK) {
        ota_report_msg_status_val_to_app(OTA_FINISH_1);
        if(ota_update_handle.type == FORCE_OTA_START){
            printf("set FORCE_OTA_FINISH:2\r\n");
            custom_config_storage_set_u8("OTA_F",FORCE_OTA_FINISH);
        }else{
            custom_config_storage_set_u8("OTA_F",APP_OTA_OK);
        }
        printf("set ota update flag\r\n");
    } else {
        char info[50];
        snprintf(info, sizeof(info), "esp_https_ota failed with %d: %s", err, esp_err_to_name(err));
        ota_report_msg_status_val_to_app(OTA_FAIL_1);
        
        if(ota_update_handle.type == FORCE_OTA_START){
            printf("set FORCE_OTA_START:2\r\n");
            custom_config_storage_set_u8("OTA_F",FORCE_OTA_START);
        }else{
            report_device_info_to_server(OTA_UPDATE,SERVER_TYPE,ota_vertion,false,info);
            custom_config_storage_set_u8("OTA_F",APP_OTA_FAIL);
        }
        
        printf("set ota update  fail flag\r\n");
    }
    return err;
}

 extern uint32_t app_to_current_val;
 extern int app_set_volume;
static esp_err_t output_callback(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    ESP_LOGI(TAG, "********************** Device turned %s **********************", param->val.b? "ON" : "OFF");
    stm8_ply_set_state(param->val.b);
    return ESP_OK;
}

static esp_err_t alexa_status(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    
    return ESP_OK;
}

static esp_err_t connected_status(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    
    return ESP_OK;
}

static esp_err_t alexa_volume(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    if (priv_data) {
        ESP_LOGI(TAG, "%s-%s-%d\n",name,(char *)priv_data,param->val.i);
        app_set_volume = param->val.i;
        app_to_current_val = 2480;
    }

    return ESP_OK;
}

static esp_err_t device_restore_factory(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    if (priv_data) {
        ESP_LOGI(TAG, "%s-%s-%s\n",name,(char *)priv_data,(param->val.b == false)?"OK":"FAIL");
    }

    if(param->val.b == false){
        app_driver_set_state(false);
        esp_cloud_update_bool_param(esp_cloud_get_handle(), "power", false);
        button_factory_reset_bc_3sec();
    }

    return ESP_OK;
}

static esp_err_t ota_version_cb(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    ESP_LOGI(TAG,"%s-%s\n",name,param->val.s);
    strncpy(ota_ver,param->val.s,sizeof(ota_ver));
    return ESP_OK;
}

static esp_err_t ota_url_cb(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    ESP_LOGI(TAG, "%s-%s\n",name,param->val.s);
    strncpy(ota_url,param->val.s,sizeof(ota_url));
    return ESP_OK;
}


static esp_err_t ota_size_cb(const char *name, esp_cloud_param_val_t *param, void *priv_data)
{
    ESP_LOGI(TAG, "%s-%d\n",name,param->val.i);
    ota_size = param->val.i;
    if(ota_url!=0){
        uint8_t ota_flag = custom_config_storage_get_u8("OTA_F");
        if(ota_flag == FORCE_OTA_FINISH){

        }else{
            ota_update_handle.type = FORCE_OTA_INIT;
        }
    }
        return ESP_OK;
}



void app_cloud_init()
{
    ESP_LOGI(TAG, "Initializing Cloud Agent");
    esp_cloud_config_t cloud_cfg = {
        .id = {
            .name = "Alexa Smart Switch",
            .type = "Speaker with Switch",
            .model = "ESP-Alexa-Switch",
            .fw_version = "1.0.15",
        },
        .enable_time_sync = false,
        .reconnect_attempts = 65535,
        .dynamic_cloud_params_count = 10,
    };
    esp_cloud_init(&cloud_cfg, &esp_cloud_handle);

    ESP_LOGI(TAG, "Cloud Agent Initialized");
    ESP_LOGI(TAG, "Version:%s",cloud_cfg.id.fw_version);
}

void app_cloud_start()
{
    ESP_LOGI(TAG, "Connecting to Cloud");
    esp_cloud_enable_ota(esp_cloud_handle, app_cloud_perform, NULL);

    esp_cloud_add_dynamic_bool_param(esp_cloud_handle, "power", false, output_callback, NULL);
    esp_cloud_add_dynamic_bool_param(esp_cloud_handle, "init", true, device_restore_factory, "device restore factory");
    esp_cloud_add_dynamic_bool_param(esp_cloud_handle, "connected", true, connected_status, "connected");
    esp_cloud_add_dynamic_bool_param(esp_cloud_handle, "alexa", false, alexa_status, "alexa");
    esp_cloud_add_dynamic_int_param(esp_cloud_handle, "alexa_volume", 50, alexa_volume, "alexa_volume");
    strncpy(ota_url, "", sizeof(ota_url));
    esp_cloud_add_dynamic_string_param(esp_cloud_handle, "ota_url", ota_url, sizeof(ota_url), ota_url_cb, NULL);
    strncpy(ota_ver, "", sizeof(ota_ver));
    esp_cloud_add_dynamic_string_param(esp_cloud_handle, "ota_version", ota_ver, sizeof(ota_ver), ota_version_cb, NULL);
    esp_cloud_add_dynamic_int_param(esp_cloud_handle, "ota_size", ota_size, ota_size_cb, NULL);
    esp_cloud_start(esp_cloud_handle);
}

// #endif /* CONFIG_ALEXA_ENABLE_OTA */
