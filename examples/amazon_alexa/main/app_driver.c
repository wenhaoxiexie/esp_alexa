/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <freertos/FreeRTOS.h>
#include <iot_button.h>
#include <esp_cloud.h>
#include "app_priv.h"
#include <nvs_flash.h>
#include "esp_system.h"
#include "app_auth_user.h"
#include "va_button.h"
#include "production_test.h"
#include "va_led.h"
/* This is the button that is used for toggling the output */
// #define BUTTON_GPIO     39
#define BUTTON_GPIO     34
#define BUTTON_ACTIVE_LEVEL  0
/* This is the GPIO on which the output will be set */
#define OUTPUT_GPIO    12

static bool g_output_state;
int btn_num = 0;
void Click_button(void){
    bool new_state = !g_output_state;
    app_driver_set_state(new_state);
    esp_cloud_update_bool_param(esp_cloud_get_handle(), "power", new_state);
}

extern uint32_t app_to_current_val;
void push_btn_exec(void){
    if(btn_num==1){
        vTaskDelay(pdMS_TO_TICKS(500));
        if(btn_num==2) {
            // if(Wait_for_alexa_in == LOGED_IN_NOTIVE){
            //     app_to_current_val = 1230;
            // }
        }else if(btn_num==3){
            // pro_test_item = 10;
        }else if(btn_num==4){
            // pro_test_item = 20;
        }
        else{
            Click_button();
        }
        printf("btn_num:%d\r\n",btn_num);
        btn_num = 0;
    }
}

bool btn_sec_cd = false;
static void push_btn_cb(void *arg)
{
    static int i = 0;
    printf("%i\r\n",i++);
    Click_button();
}

static void button_press_2sec_cb(void *arg)
{
    if(Wait_for_alexa_in == LOGED_IN_NOTIVE){
        app_to_current_val = 1230;
    }
}

static void set_output_state(bool target)
{
    gpio_set_level(OUTPUT_GPIO, target);
}

void app_driver_init()
{
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, push_btn_cb, "RELEASE");
        iot_button_add_custom_cb(btn_handle, 2, button_press_2sec_cb, NULL);
    }

    /* Configure output */
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    io_conf.pin_bit_mask = ((uint64_t)1 << OUTPUT_GPIO);
    /* Configure the GPIO */
    gpio_config(&io_conf);
    gpio_set_level(OUTPUT_GPIO,false);
}

int IRAM_ATTR app_driver_set_state(bool state)
{
    if(g_output_state != state) {
        g_output_state = state;
        stm8_ply_set_state(g_output_state);
    }
    return ESP_OK;
}

bool app_driver_get_state(void)
{
    return g_output_state;
}
