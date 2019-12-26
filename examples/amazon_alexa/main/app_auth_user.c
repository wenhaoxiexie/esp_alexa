/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "app_wifi.h"
#include <freertos/event_groups.h>

#include "esp_http_client.h"
#include "app_prov_handlers.h"
#include <esp_cloud_storage.h>
#include "app_auth_user.h"
#include "va_button.h"


#define MAX_HTTP_RECV_BUFFER 512

uint8_t Wait_for_alexa_in = NOT_LOG_IN;
uint8_t Wait_for_alexa_out = LOGED__OUT;
static const char *TAG = "HTTP_CLIENT";
extern const int AUTH_DONE_BIT;
// extern EventGroupHandle_t cm_event_group;
int bind_status_code = 0;    /*!< status code (integer) */
// extern esp_cloud_internal_handle_t *g_cloud_handle;
int32_t user_bind_flag = 0; // value will default to 0, if not set yet in NVS
int32_t app_uid = 0; // value will default to 0, if not set yet in NVS
/* Root cert for howsmyssl.com, taken from howsmyssl_com_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const char howsmyssl_com_root_cert_pem_start[] asm("_binary_howsmyssl_com_root_cert_pem_start");
extern const char howsmyssl_com_root_cert_pem_end[]   asm("_binary_howsmyssl_com_root_cert_pem_end");


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                printf("Write out data:%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void report_device_info_to_server(int info_type,int ota_type,char *version,bool result,char *add_info)
{
    int _content_length = 0; /*!< data length */
    nvs_handle NVS_handle;
    int  reconnection_http=1;
    esp_err_t err;

   switch (info_type)
   {
        case AUTH_INFO:
                err = nvs_open("custom_data", NVS_READWRITE, &NVS_handle);
                if (err != ESP_OK) {
                    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
                } else {
                    err = nvs_get_i32(NVS_handle, "user_bind_flag", &user_bind_flag);
                    switch (err) {
                        case ESP_OK:
                            printf("user_bind_flag = %d\n", user_bind_flag);
                            app_auth_done_cb();
                            break;
                        case ESP_ERR_NVS_NOT_FOUND:
                            printf("user_bind_flag is not initialized yet!\n");
                            break;
                        default :
                            printf("Error (%s) reading!\n", esp_err_to_name(err));
                            break;
                    }

                    if(user_bind_flag == NO_BIND){
                        char clent_url[200] = {0};
                        char device_token[50]={0};
                        char post_data[100]={0};
                
                    printf("-------------------------------------------------------bind start------------------------------------------------------------\r\n");
                        char * p_device_id = esp_cloud_storage_get("device_id");
                        if (p_device_id) {
                            nvs_get_i32(NVS_handle, "uid_data", &app_uid);
                            sprintf(post_data, "device_id=%s&user_id=%d", p_device_id,app_uid);
                            free(p_device_id);
                        }else{
                            ESP_LOGE(TAG, "p_device_id: fail");
                            button_factory_reset_bc_3sec();
                            return;
                        }

                        char * p_device_token = esp_cloud_storage_get("device_token");
                        if (p_device_token) {
                        sprintf(device_token, "token %s", p_device_token);
                        free(p_device_token);
                        }else{
                            ESP_LOGE(TAG, "p_device_token: fail");
                            button_factory_reset_bc_3sec();
                            return;
                        }

                        char *https_url = custom_config_storage_get("https_url");

                        if(https_url){
                            sprintf(clent_url, "%s/device_cert/user_bind/",https_url);
                            free(https_url);
                        }else{
                            ESP_LOGE(TAG, "https_url: fail");
                            button_factory_reset_bc_3sec();
                            return;
                        }


                        printf("post_data:%s\r\n",post_data);
                        printf("device_token %s\r\n",device_token);
                        printf("clent_url %s\r\n",clent_url);

                        esp_http_client_config_t config = {
                        .url = clent_url,
                        .event_handler = _http_event_handler,
                        };
                        esp_http_client_handle_t client = esp_http_client_init(&config);
                        esp_http_client_set_url(client, clent_url);
                        esp_http_client_set_method(client, HTTP_METHOD_POST);
                        err = esp_http_client_set_header(client, "AUTHORIZATION", device_token);
                        esp_http_client_set_post_field(client, post_data, strlen(post_data));
                        while(reconnection_http){
                            err = esp_http_client_perform(client);
                            if (err == ESP_OK) {
                                bind_status_code = esp_http_client_get_status_code(client);
                                _content_length = esp_http_client_get_content_length(client);
                                ESP_LOGI(TAG, "HTTP POST Status---- = %d, content_length = %d",bind_status_code,_content_length);  
                                if(bind_status_code==200){
                                    reconnection_http=0; 
                                }
                            } else {
                                ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
                                printf("reconnection_http\r\n");
                            }

                            vTaskDelay(1000/ portTICK_PERIOD_MS);
                        }

                        if(bind_status_code == 200){
                            user_bind_flag = BINDED;
                            err = nvs_set_i32(NVS_handle, "user_bind_flag", user_bind_flag);
                            app_auth_done_cb();
                            dev_states = BIND_OK;
                        }else{
                            dev_states = BIND_FAIL;
                        }

                        err = nvs_commit(NVS_handle);
                        nvs_close(NVS_handle);
                        esp_http_client_cleanup(client); 
                        user_bind_flag = NOTICE_BINDED;
                        printf("-------------------------------------------------------bind stop------------------------------------------------------------\r\n");
                    }else{
                        app_auth_done_cb();

                    }
                }
            break;

            case OTA_UPDATE:
            err = nvs_open("custom_data", NVS_READWRITE, &NVS_handle);
                if (err != ESP_OK) {
                    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
                } else {
                   
                    char clent_url[200] = {0};
                    char device_token[50]={0};
                    char post_data[200]={0};
                
                    if(ota_type == APP_TYPE){
                        sprintf(post_data, "version=%s&result=%d&msg=%s", version,result,add_info);
                    }else if(ota_type == SERVER_TYPE){
                         sprintf(post_data, "force_ota=%d&version=%s&result=%d&msg=%s",true,version,result,add_info);
                    }

                    char * p_device_token = esp_cloud_storage_get("device_token");
                    if (p_device_token) {
                    sprintf(device_token, "token %s", p_device_token);
                    free(p_device_token);
                    }else{
                        ESP_LOGE(TAG, "p_device_token: fail");
                        return;
                    }

                    char *https_url = custom_config_storage_get("https_url");

                    if(https_url){
                        sprintf(clent_url, "%s/device_cert/ota_result/",https_url);
                        free(https_url);
                    }else{
                        ESP_LOGE(TAG, "https_url: fail");
                        return;
                    }
                  
                    printf("post_data:%s\r\n",post_data);
                    printf("device_token %s\r\n",device_token);
                    printf("clent_url %s\r\n",clent_url);

                    esp_http_client_config_t config = {
                    .url = clent_url,
                    .event_handler = _http_event_handler,
                    };
                    esp_http_client_handle_t client = esp_http_client_init(&config);
                    esp_http_client_set_url(client, clent_url);
                    esp_http_client_set_method(client, HTTP_METHOD_POST);
                    err = esp_http_client_set_header(client, "AUTHORIZATION", device_token);
                    esp_http_client_set_post_field(client, post_data, strlen(post_data));

                    err = esp_http_client_perform(client);
                    if (err == ESP_OK) {
                        esp_http_client_get_status_code(client);
                        _content_length = esp_http_client_get_content_length(client);
                        ESP_LOGI(TAG, "HTTP POST Status---- = %d, content_length = %d",bind_status_code,_content_length);  
                    } else {
                        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
                    }
                }
            break;
   
            default:
                break;
   }
}



void app_user_unbind_report(void){
        
        char clent_url[200] = {0};
        char device_token[50] = {0};

        char * p_device_id = esp_cloud_storage_get("device_id");
        if (!p_device_id) {
            ESP_LOGE(TAG, "p_device_id: fail");
            return;
        }

        char *https_url = custom_config_storage_get("https_url");
        if (!https_url) {
            ESP_LOGE(TAG, "https_url: fail");
            return;
        }else
        { 
            sprintf(clent_url, "%s/device_cert/%s/reset/",https_url,p_device_id);
        }
        
        char * p_device_token = esp_cloud_storage_get("device_token");
        if (!p_device_token) {
            ESP_LOGE(TAG, "p_device_token: fail");
            return;
        }else{
            sprintf(device_token, "token %s", p_device_token);
            // free(p_device_token);
        }
        
        // printf("clent_url:%s\r\n",clent_url);
        // printf("device_token %s\r\n",device_token);

        esp_http_client_config_t config = {
            .url = clent_url,
            .event_handler = _http_event_handler,
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_url(client,clent_url);
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_header(client, "AUTHORIZATION", device_token);
        esp_http_client_set_post_field(client, "post_data", strlen("post_data"));
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            bind_status_code = esp_http_client_get_status_code(client);
            printf("HTTP POST Status---- = %d",bind_status_code);   
        } else {
            printf( "HTTP POST request failed: %s", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client); 
}