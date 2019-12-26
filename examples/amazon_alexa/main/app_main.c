// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_event_loop.h>
#include <esp_pm.h>
#include <nvs_flash.h>
#include <conn_mgr_prov.h>
#include <conn_mgr_prov_mode_ble.h>
#include <voice_assistant.h>
#include <alexa.h>
#include <alexa_local_config.h>
#include <va_mem_utils.h>
#include <scli.h>
#include <va_diag_cli.h>
#include <wifi_cli.h>
#include <media_hal.h>
#include <tone.h>
#include <avs_config.h>
#include <speech_recognizer.h>
#include "va_board.h"
#include "app_auth.h"
#include "app_wifi.h"
#include "app_auth_user.h"
#include "app_prov.h"   
#include <va_nvs_utils.h>
#include "app_priv.h"
#include "app_led.h"
#include <esp_cloud_storage.h>
#include "lwip/apps/sntp.h"
#include "production_test.h"
#include <protocomm_security1.h>
#include <speech_recognizer.h>

#ifdef CONFIG_ALEXA_ENABLE_EQUALIZER
#include "alexa_equalizer.h"
#endif

#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#ifdef CONFIG_ALEXA_ENABLE_OTA
// #include "app_ota.h"
#include "app_cloud.h"
#endif

#define SOFTAP_SSID_PREFIX  "ESP-Alexa-"

static const char *TAG = "[app_main]";


static  EventGroupHandle_t cm_event_group;
const int CONNECTED_BIT = BIT0;
const int PROV_DONE_BIT = BIT1;
const int AUTH_DONE_BIT = BIT2;
const int UID_DONE_BIT =  BIT3;
const int AWS_IOT_DONE_BIT =  BIT4;

void app_prov_done_cb()
{
    xEventGroupSetBits(cm_event_group, PROV_DONE_BIT);
}

void app_auth_done_cb()
{
    xEventGroupSetBits(cm_event_group, AUTH_DONE_BIT);
}

void app_uid_done_cb()
{
    xEventGroupSetBits(cm_event_group, UID_DONE_BIT);
}

void app_aws_done_cb()
{
    xEventGroupSetBits(cm_event_group, AWS_IOT_DONE_BIT);
}

void alexa_log_in(void){
    Wait_for_alexa_in = LOGED_IN;
    printf("alexa_log_in\r\n");
}

void alexa_log_out(void){
    printf("alexa_log_out\r\n");
}

bool custom_lan_start = false;
char app_ip[15];
extern char *cus_ip;
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    /* Invoke Provisioning event handler first */
    app_prov_event_handler(ctx, event);

    switch(event->event_id) {
    case SYSTEM_EVENT_AP_START:
        ESP_LOGI(TAG, "SoftAP started");
        break;
    case SYSTEM_EVENT_AP_STOP:
        ESP_LOGI(TAG, "SoftAP stopped");
        break;
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        esp_wifi_set_storage(WIFI_STORAGE_FLASH);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        app_wifi_stop_timeout_timer();
        snprintf(app_ip,sizeof(app_ip),"%s",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        printf("app ip:%s\r\n",app_ip);
        if(custom_lan_start == true){
            cus_ip = app_ip;
        }
        xEventGroupSetBits(cm_event_group, CONNECTED_BIT);
        app_prov_done_cb();
        esp_wifi_set_storage(WIFI_STORAGE_FLASH);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    case SYSTEM_EVENT_STA_LOST_IP:
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        app_wifi_stop_timeout_timer();
        printf("%s: Disconnected. Event: %d. Connecting to the AP again\n", TAG, event->event_id);
        esp_wifi_connect();
        xEventGroupClearBits(cm_event_group, CONNECTED_BIT);
        break;

    default:
        
        break;
    }
    return ESP_OK;
}

static void wifi_init_sta()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_start() );
#ifdef CONFIG_PM_ENABLE
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MIN_MODEM));
#else
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
#endif
}

#define MEDIA_HAL_DEFAULT()     \
    {   \
        .op_mode    = MEDIA_HAL_MODE_SLAVE,              \
        .adc_input  = MEDIA_HAL_ADC_INPUT_LINE1,         \
        .dac_output = MEDIA_HAL_DAC_OUTPUT_ALL,          \
        .codec_mode = MEDIA_HAL_CODEC_MODE_BOTH,         \
        .bit_length = MEDIA_HAL_BIT_LENGTH_16BITS,       \
        .format     = MEDIA_HAL_I2S_NORMAL,              \
        .port_num = 0,                          \
    };




extern protocomm_t *pc;
extern void touch_driver_init();
void app_main()
{
    ESP_LOGI(TAG, "==== Voice Assistant SDK version: %s ====", va_get_sdk_version());

    alexa_config_t *va_cfg = va_mem_alloc(sizeof(alexa_config_t), VA_MEM_EXTERNAL);
    if (!va_cfg) {
        ESP_LOGE(TAG, "Failed to alloc voice assistant config");
        abort();
    }
    va_cfg->product_id = CONFIG_ALEXA_PRODUCT_ID;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    // va_nvs_set_i8("d2_fw_debug", 1);
    // va_nvs_set_i8("d4p_fw_debug", 1);
    // va_nvs_set_i8("d2_audio_debug", 1);
    va_board_init();
    static media_hal_config_t media_hal_conf = MEDIA_HAL_DEFAULT();
    media_hal_init(&media_hal_conf);
    app_driver_init();
    va_board_button_init();
    va_stm8_led_init();
    scli_init();
    va_diag_register_cli();
    wifi_register_cli();
    app_wifi_reset_to_prov_init();
    app_auth_register_cli();
    cm_event_group = xEventGroupCreate();
    app_cloud_init();
    tcpip_adapter_init();
    touch_driver_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );
    
    printf("\r");       // To remove a garbage print ">>"
    alexa_auth_delegate_init(alexa_log_in, alexa_log_out);
    bool provisioned = false;
    
    if (app_prov_is_provisioned(&provisioned) != ESP_OK) {
        ESP_LOGE(TAG, "Error getting device provisioning state");
        abort();
    }

    if (app_wifi_get_reset_to_prov() > 0) {
        app_wifi_start_timeout_timer();
        provisioned = false;
        app_wifi_unset_reset_to_prov();
        esp_wifi_set_storage(WIFI_STORAGE_RAM);
    }

    if (!provisioned) {
        char ssid_with_mac[33];
        uint8_t eth_mac[6];
        esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
        snprintf(ssid_with_mac, sizeof(ssid_with_mac), "Gosund_Smart_Plug_%02X%02X%02X",
                eth_mac[3], eth_mac[4], eth_mac[5]);
        va_led_set(LED_RESET);
        ESP_LOGI(TAG, "Starting WiFi SoftAP provisioning");
       
        const protocomm_security_pop_t *pop = NULL;
        const static protocomm_security_pop_t app_pop = {
            .data = (uint8_t *)"abcd1234",
            .len = (sizeof("abcd1234")-1)
        };
        pop = &app_pop;
        app_prov_start_softap_provisioning(ssid_with_mac, "12345678",1, pop);
        custom_lan_start = true;
    } else {
        va_led_set(VA_CAN_START);
        xEventGroupSetBits(cm_event_group, UID_DONE_BIT);     //lin 2019-9-17
        ESP_LOGI(TAG, "Already provisioned, starting station"); 
        app_prov_done_cb();
        wifi_init_sta();
    }

    xEventGroupWaitBits(cm_event_group, CONNECTED_BIT|PROV_DONE_BIT|UID_DONE_BIT, false, true, portMAX_DELAY);
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "0.asia.pool.ntp.org");
    sntp_setservername(1, "0.cn.pool.ntp.org");
    sntp_setservername(2, "0.us.pool.ntp.org");
    sntp_init();
    /* Wait for time to get updated */
    time_t current_time = 0;
    while (current_time < ((2019 - 1970) * 365 * 24 * 3600)) {
        time(&current_time);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    sntp_stop();
    va_cfg->product_id = CONFIG_ALEXA_PRODUCT_ID;
    va_cfg->device_serial_num = esp_cloud_storage_get("device_id");
    if(va_cfg->device_serial_num){
        char serial_num[100]={0};
        snprintf(serial_num,sizeof(serial_num),"amzn://%s",va_cfg->device_serial_num);
        va_cfg->device_serial_num = serial_num;
        printf("%s--%s\r\n",va_cfg->product_id,va_cfg->device_serial_num);
    }

    ret = alexa_local_config_start(va_cfg, "Gosund_Smart_Plug_123456");
    if (ret != ESP_OK) {                                       
	    ESP_LOGE(TAG, "Failed to start local SSDP instance. Some features might not work.");
    }

    // if(custom_lan_start){
        if (protocomm_add_endpoint(pc, "custom-config",custom_data_cb,NULL)!= ESP_OK) {
            ESP_LOGE(TAG, "Failed to set custom provisioning endpoint");
        }
    // }
    report_device_info_to_server(AUTH_INFO,0,NULL,0,NULL);
    xEventGroupWaitBits(cm_event_group, AUTH_DONE_BIT, false, true, portMAX_DELAY);
    
    if (!provisioned) {
        va_led_set(VA_CAN_START);
    }

#ifdef CONFIG_ALEXA_ENABLE_EQUALIZER
    alexa_equalizer_init();
#endif
 
#ifdef ALEXA_BT
    alexa_bt_a2dp_sink_init();
#endif
    app_cloud_start();
    ret = alexa_init(va_cfg);
    
    if (ret != ESP_OK) {
        while(1) vTaskDelay(2);
    }

    /* This is a blocking call */
    va_dsp_init(speech_recognizer_recognize, speech_recognizer_record);

#ifdef CONFIG_ALEXA_ENABLE_OTA
    /* Doing OTA init after full alexa boot-up. */
    //  app_ota_init();
#endif
    alexa_tone_enable_larger_tones();
    set_default_volume();
#ifdef CONFIG_PM_ENABLE
    rtc_cpu_freq_t max_freq;
    rtc_clk_cpu_freq_from_mhz(CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ, &max_freq);
    esp_pm_config_esp32_t pm_config = {
            .max_cpu_freq = max_freq,
            .min_cpu_freq = RTC_CPU_FREQ_XTAL,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
            .light_sleep_enable = true
#endif
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config));
    gpio_wakeup_enable(GPIO_NUM_36, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_pm_dump_locks(stdout);
#endif
    return;
}
