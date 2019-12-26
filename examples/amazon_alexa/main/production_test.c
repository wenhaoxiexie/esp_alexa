// #include <stdio.h>
// #include <string.h>
// #include <esp_err.h>
// #include <esp_log.h>

// #include <esp_wifi.h>
// #include <tcpip_adapter.h>

// #include <wifi_provisioning/wifi_config.h>
// #include <custom_provisioning/custom_config.h>

// #include "app_prov.h"
// #include "cJSON.h"
// #include "nvs_flash.h"
// #include <freertos/event_groups.h>
// #include "esp_cloud_mem.h"
// #include "production_test.h"
// #include <freertos/FreeRTOS.h>
// #define TAG "production_test"
// nvs_handle NVS_pro_test_handle;

// // int pro_test_item;

// // char *_test_name[25] = {{"BOOTS_S"},
// //                         {"IOT_S"},
// //                         {"BTN_S"},
// //                         {"AWS_REC_S"},
// //                         {"ALEXA_IN_S"},
// //                         {"WAKE_S"}};

// typedef struct pro_test
// {
//     int32_t pro_test_buf[TEST_MAX_NUM];
//     char **test_name;

// }_pro_test;

// _pro_test sp_test;

// void production_test_dump(void){
//     sp_test.test_name = _test_name;
//     esp_err_t err = nvs_open("pro_test_data", NVS_READWRITE, &NVS_pro_test_handle);
//     if (err != ESP_OK) {
//         printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     } else {
//             printf("--------------------------------------------------------\r\n");
//             for(int i = 0;i< TEST_MAX_NUM;i++){
//                 err = nvs_get_i32 (NVS_pro_test_handle,sp_test.test_name[i],&sp_test.pro_test_buf[i]);
//                 if(err!=ESP_OK){
//                     ESP_LOGE(TAG,"Error --%s-- \n", sp_test.test_name[i]);
//                 }else{
//                     ESP_LOGI(TAG,"%s--%d",sp_test.test_name[i],sp_test.pro_test_buf[i]);
//                 }
//             }
//     }
//     nvs_commit(NVS_pro_test_handle);
//     nvs_close(NVS_pro_test_handle);
// }

// esp_err_t production_test_init(int init_flag){
//     int flag = init_flag;
//     esp_err_t err;
//     sp_test.test_name =  _test_name;
//     err = nvs_open("pro_test_data", NVS_READWRITE, &NVS_pro_test_handle);
//     if (err != ESP_OK) {
//         printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     } else{

//             nvs_get_i32 (NVS_pro_test_handle,"init_flag",&flag);
            
//             if(!flag){
//                 for(int i = 0;i<TEST_MAX_NUM;i++){
//                     err = nvs_set_i32(NVS_pro_test_handle, sp_test.test_name[i],0);
//                     if(err!=ESP_OK){
//                         printf("Error (%s) \n", sp_test.test_name[i]);
//                     }else{
//                         printf("set:%s \n", sp_test.test_name[i]);
//                     }
//                 }

//                 flag = 1;
//                 err = nvs_set_i32(NVS_pro_test_handle,"init_flag",flag);
//                 if(err!=ESP_OK){
//                     printf("Error  \n");
//                 }else{
//                     printf("set:%d \n", flag);
//                 }
//             }

//             production_test_dump();
//             nvs_commit(NVS_pro_test_handle);
//             nvs_close(NVS_pro_test_handle);
//     }

//     return err;
// }

// esp_err_t production_test(int32_t test_item)
// {
//     esp_err_t err ;
//     if(test_item != 100){
//         err = nvs_open("pro_test_data", NVS_READWRITE, &NVS_pro_test_handle);
//         if (err != ESP_OK) {
//             printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//         } else {
//                 if((test_item>=BOOTS_TIMES)&&(test_item<TEST_MAX_NUM)){
//                     sp_test.test_name = _test_name;
//                     err = nvs_get_i32 (NVS_pro_test_handle,sp_test.test_name[test_item],&sp_test.pro_test_buf[test_item]);
//                     if(err!=ESP_OK){
//                         printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//                     }
//                     sp_test.pro_test_buf[test_item] = sp_test.pro_test_buf[test_item] + 1;

//                     err = nvs_set_i32(NVS_pro_test_handle, sp_test.test_name[test_item],sp_test.pro_test_buf[test_item]);
//                     if(err!=ESP_OK){
//                         printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//                     }
//                 }else if(test_item==10){
//                     production_test_dump();
//                 }else if(test_item==20){
//                     nvs_set_i32(NVS_pro_test_handle,"init_flag",0);
//                     esp_restart();
//                 }

//                 nvs_commit(NVS_pro_test_handle);
//                 nvs_close(NVS_pro_test_handle);
//         }
//     }
//     // pro_test_item = 100;
//     return ESP_OK;
// }


