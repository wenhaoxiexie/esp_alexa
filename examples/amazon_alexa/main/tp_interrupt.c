/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#include "driver/touch_pad.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "va_button.h"
#include "app_auth_user.h"
static const char* TAG = "Touch pad";
#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESH_PERCENT  (94)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (15)

static bool s_pad_activated[TOUCH_PAD_MAX];
static uint32_t s_pad_init_val[TOUCH_PAD_MAX];

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_3;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

static void check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void temperatur_esensor()
{
    static uint32_t timer;
    timer++;
    if(timer>=10){
        timer = 0;
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        // printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    }
}

static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;
    touch_pad_read_filtered(TOUCH_PAD_NUM5, &touch_value);
    s_pad_init_val[TOUCH_PAD_NUM5] = touch_value;
    printf("test init: touch pad [%d] val is %d\r\n", TOUCH_PAD_NUM5, touch_value);
    ESP_ERROR_CHECK(touch_pad_set_thresh(TOUCH_PAD_NUM5, touch_value * 2 / 3));
}

extern uint32_t app_to_current_val;
int ota_update_per;
int ota_filesize = 0;
extern void ota_report_progress_val_to_app(int progress_val);
static void tp_example_read_task(void *pvParameter)
{
    static int show_message = 0;
    while (1) {
        uint16_t value = 0;
        touch_pad_read_filtered(TOUCH_PAD_NUM5, &value);
        //  ESP_LOGI(TAG,"value: %d; init val: %d\r\n", value, s_pad_init_val[TOUCH_PAD_NUM5]);
        if (value < s_pad_init_val[TOUCH_PAD_NUM5] * TOUCH_THRESH_PERCENT / 100) 
        if (value < s_pad_init_val[TOUCH_PAD_NUM5] * TOUCH_THRESH_PERCENT / 100) {
            printf("value: %d; init val: %d\r\n", value, s_pad_init_val[TOUCH_PAD_NUM5]);
            touch_pad_read_filtered(TOUCH_PAD_NUM5, &value);
            if (value < s_pad_init_val[TOUCH_PAD_NUM5] * TOUCH_THRESH_PERCENT / 100) {
                for(int i=0;i<310;i++){
                    vTaskDelay(30/ portTICK_PERIOD_MS);
                    touch_pad_read_filtered(TOUCH_PAD_NUM5, &value);

                    // if(i>10){
                        if(value > s_pad_init_val[TOUCH_PAD_NUM5] * TOUCH_THRESH_PERCENT / 100){
                            vTaskDelay(30/ portTICK_PERIOD_MS);
                            ESP_LOGI(TAG,"%d-%d\r\n",i,value);
                            touch_pad_read_filtered(TOUCH_PAD_NUM5, &value);
                            vTaskDelay(30/ portTICK_PERIOD_MS);
                            ESP_LOGI(TAG,"%d-%d\r\n",i,value);
                            touch_pad_read_filtered(TOUCH_PAD_NUM5, &value);
                            vTaskDelay(30/ portTICK_PERIOD_MS);
                            ESP_LOGI(TAG,"%d-%d\r\n",i,value);
                            touch_pad_read_filtered(TOUCH_PAD_NUM5, &value);
                        }
                    // }

                    if (value < s_pad_init_val[TOUCH_PAD_NUM5] * TOUCH_THRESH_PERCENT / 100) {
                        printf("%d-%d\r\n",i,value);
                        if(i>=300){
                            app_user_unbind_report();
                            factory_rst_en();
                        }


                        if((i>=5)&&(show_message == 0)){
                            show_message = 1;
                            if(Wait_for_alexa_in == LOGED_IN_NOTIVE){
                                 app_to_current_val = 600;
                            }
                        }
                    }else
                    {
                        show_message = 0;
                        // if(Wait_for_alexa_in == LOGED_IN_NOTIVE){
                        //     app_to_current_val = 600;
                        // }
                        printf("break:%d-%d\r\n",i,value);
                        break;
                    }
                }
            }
        }

        if(ota_update_per){
            static int num0,num1;
            num0 = ota_update_per/(ota_filesize/100);
            if(num0!=num1){
                num1 = num0;
                ota_report_progress_val_to_app(num0);
            }
        }
        temperatur_esensor();
        vTaskDelay(200/ portTICK_PERIOD_MS);
    }
}

static void tp_example_rtc_intr(void * arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    touch_pad_clear_status();
        if ((pad_intr >> TOUCH_PAD_NUM5) & 0x01) {
            s_pad_activated[TOUCH_PAD_NUM5] = true;
        }
}


void touch_driver_init()
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    printf("Initializing touch pad\r\n");
    touch_pad_init();
    // If use interrupt trigger mode, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    // touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // For most usage scenarios, we recommend using the following combination:
    // the high reference valtage will be 2.7V - 1V = 1.7V, The low reference voltage will be 0.5V.
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V5);
    // Init touch pad IO
    touch_pad_config(TOUCH_PAD_NUM5, TOUCH_THRESH_NO_USE);
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    // Set thresh hold
    tp_example_set_thresholds();
    // Register touch interrupt ISR
    // touch_pad_isr_register(tp_example_rtc_intr, NULL);
    // Start a task to show what pads have been touched

    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 3096, NULL, 6, NULL);
}
