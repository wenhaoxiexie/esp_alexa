// Copyright 2019 Gosund
// All rights reserved.

#pragma once

enum{
    BIND_WAIT,
    BIND_OK,
    BIND_FAIL,
    IOT_OK,
    IOT_FAIL,
    DEV_MAX_NUM
};

enum{
    AUTH_INFO,
    OTA_UPDATE,
    APP_TYPE,
    SERVER_TYPE,
    REPORT_MAX_NUM
};


extern int dev_states;

void app_user_bind();
void app_user_unbind_report(void);
void report_device_info_to_server(int info_type,int ota_type,char *version,bool result,char *add_info);
void app_auth_done_cb();

#define NO_BIND 0
#define BINDED 1
#define NOTICE_BINDED 2

#define NOT_LOG_IN  0
#define LOGED_IN  1
#define LOGED_IN_NOTIVE  2
#define LOGED__OUT  0
#define NOT_LOG_OUT  1

extern int32_t user_bind_flag;
extern uint8_t Wait_for_alexa_in;
extern uint8_t Wait_for_alexa_out;