#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include <esp_wifi.h>
#include <tcpip_adapter.h>

#include <wifi_provisioning/wifi_config.h>
#include <custom_provisioning/custom_config.h>

#include "app_prov.h"
#include "cJSON.h"
#include "nvs_flash.h"
#include <freertos/event_groups.h>
#include "esp_cloud_mem.h"
#include "app_prov_handlers.h"
extern void app_uid_done_cb();
static const char* TAG = "app_prov_handler";
/****************** Handler for Custom Configuration *******************/
static esp_err_t custom_config_handler(const custom_config_t *config)
{
    nvs_handle NVS_custom_handle;

    esp_err_t err = nvs_open("custom_data", NVS_READWRITE, &NVS_custom_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Custom config received :\n\tInfo : %s\n\tVersion : %d",
                config->info, config->version);

        cJSON*root = cJSON_Parse(config->info);

        if(!root){
            ESP_LOGE(TAG, " cJSON_Parse fail");
            return ESP_FAIL;
        } 

        cJSON*item=cJSON_GetObjectItem(root,"uid");
        if(item) {
            err = nvs_set_i32(NVS_custom_handle, "uid_data", item->valueint);
            if(err != ESP_OK){
                ESP_LOGE(TAG, " nvs set uid date fail");
            }
            printf("uid_data:%d\r\n",item->valueint);
            item = NULL;
        }else{
            ESP_LOGE(TAG, "uid item fail");
            nvs_close(NVS_custom_handle);
            return ESP_FAIL;
        }

        item = cJSON_GetObjectItem(root,"https_url");
        if(item) {
            err = nvs_set_str(NVS_custom_handle, "https_url", item->valuestring);
            if(err != ESP_OK){
                ESP_LOGE(TAG, " nvs set https_url fail");
                return ESP_FAIL;
            }
            printf("https_url:%s\r\n",item->valuestring);
            item = NULL;
        }else{
            ESP_LOGE(TAG, "https_url item fail");
            nvs_close(NVS_custom_handle);
            return ESP_FAIL;
        }

        item = cJSON_GetObjectItem(root,"app_topic");
        if(item) {
            err = nvs_set_str(NVS_custom_handle, "app_topic", item->valuestring);
            if(err != ESP_OK){
                ESP_LOGE(TAG, " nvs set app_topic fail");
                return ESP_FAIL;
            }
            printf("app_topic:%s\r\n",item->valuestring);
            item = NULL;
        }else{
            ESP_LOGE(TAG, "app_topic item fail");
            nvs_close(NVS_custom_handle);
            return ESP_FAIL;
        }

        cJSON_Delete(root);
        nvs_commit(NVS_custom_handle);
        nvs_close(NVS_custom_handle);

        app_uid_done_cb();
        
        return ESP_OK;
    }

    return ESP_FAIL;
}

custom_prov_config_handler_t custom_prov_handler = custom_config_handler;

/****************** Handlers for Wi-Fi Configuration *******************/
static esp_err_t get_status_handler(wifi_prov_config_get_data_t *resp_data)
{
    /* Initialise to zero */
    memset(resp_data, 0, sizeof(wifi_prov_config_get_data_t));

    if (app_prov_get_wifi_state(&resp_data->wifi_state) != ESP_OK) {
        ESP_LOGW(TAG, "Prov app not running");
        return ESP_FAIL;
    }

    if (resp_data->wifi_state == WIFI_PROV_STA_CONNECTED) {
        ESP_LOGI(TAG, "Connected state");

        /* IP Addr assigned to STA */
        tcpip_adapter_ip_info_t ip_info;
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
        char *ip_addr = ip4addr_ntoa(&ip_info.ip);
        strcpy(resp_data->conn_info.ip_addr, ip_addr);

        /* AP information to which STA is connected */
        wifi_ap_record_t ap_info;
        esp_wifi_sta_get_ap_info(&ap_info);
        memcpy(resp_data->conn_info.bssid, (char *)ap_info.bssid, sizeof(ap_info.bssid));
        memcpy(resp_data->conn_info.ssid,  (char *)ap_info.ssid,  sizeof(ap_info.ssid));
        resp_data->conn_info.channel   = ap_info.primary;
        resp_data->conn_info.auth_mode = ap_info.authmode;
    } else if (resp_data->wifi_state == WIFI_PROV_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected state");

        /* If disconnected, convey reason */
        app_prov_get_wifi_disconnect_reason(&resp_data->fail_reason);
    } else {
        ESP_LOGI(TAG, "Connecting state");
    }
    return ESP_OK;
}

wifi_config_t *wifi_cfg;

static esp_err_t set_config_handler(const wifi_prov_config_set_data_t *req_data)
{
    if (wifi_cfg) {
        free(wifi_cfg);
        wifi_cfg = NULL;
    }

    wifi_cfg = (wifi_config_t *) calloc(1, sizeof(wifi_config_t));
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Unable to alloc wifi config");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "WiFi Credentials Received : \n\tssid %s \n\tpassword %s",
             req_data->ssid, req_data->password);
    memcpy((char *) wifi_cfg->sta.ssid, req_data->ssid,
           strnlen(req_data->ssid, sizeof(wifi_cfg->sta.ssid)));
    memcpy((char *) wifi_cfg->sta.password, req_data->password,
           strnlen(req_data->password, sizeof(wifi_cfg->sta.password)));
    return ESP_OK;
}

static esp_err_t apply_config_handler(void)
{
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "WiFi config not set");
        return ESP_FAIL;
    }

    app_prov_configure_sta(wifi_cfg);
    ESP_LOGI(TAG, "WiFi Credentials Applied");

    free(wifi_cfg);
    wifi_cfg = NULL;
    return ESP_OK;
}

wifi_prov_config_handlers_t wifi_prov_handlers_gosund = {    //lin
    .get_status_handler   = get_status_handler,
    .set_config_handler   = set_config_handler,
    .apply_config_handler = apply_config_handler,
};

char *custom_config_storage_get(const char *key)
{
    nvs_handle handle;
    esp_err_t err;
    if ((err = nvs_open("custom_data", NVS_READONLY, &handle)) != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed with error %d", err);
        return NULL;
    }
    size_t required_size = 0;
    if ((err = nvs_get_str(handle, key, NULL, &required_size)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read key %s with error %d size %d", key, err, required_size);
        return NULL;
    }
    char *value = esp_cloud_mem_calloc(required_size + 1, 1); /* + 1 for NULL termination */
    if (value) {
        nvs_get_str(handle, key, value, &required_size);
    }
    nvs_close(handle);
    return value;
}

void custom_config_storage_set_u8(const char *key,uint8_t value)
{
    nvs_handle handle;
    esp_err_t err;
    if ((err = nvs_open("custom_data", NVS_READWRITE, &handle)) != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed with error %d", err);
        return NULL;
    }
    size_t required_size = 0;
    if ((err = nvs_set_u8(handle, key, value)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read key %s with error %d size %d", key, err, required_size);
        return NULL;
    }
    nvs_close(handle);
    return NULL;
}

uint8_t custom_config_storage_get_u8(const char *key)
{
    nvs_handle handle;
    uint8_t value;
    esp_err_t err;
    if ((err = nvs_open("custom_data", NVS_READONLY, &handle)) != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed with error %d", err);
        return CUSTOM_INVALID;
    }
    size_t required_size = 0;
    if ((err = nvs_get_u8(handle, key,&value)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read key %s with error %d ", key, err);
        return CUSTOM_INVALID;
    }

    nvs_close(handle);
    return value;
}
