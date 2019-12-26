#pragma once

 #define   LEDC_SET_UP_NET_MODE  1
 #define   LEDC_ADSL_MODE    2      
 #define   LEDC_SUCCESSFUL_MODE   3 

extern uint8_t led_status;

void led_task();