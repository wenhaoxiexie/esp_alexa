// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <esp_console.h>
#include <esp_log.h>

#include <voice_assistant.h>
#include <alexa.h>
#include "nvs_flash.h"

static char *TAG = "[app-auth]";

static int app_auth_sign_in_handler(int argc, char *argv[])    
{
    /* Just to go to the next line */
    printf("\n");
    if (argc != 4) {
        ESP_LOGE(TAG, "Invalid parameters");
        return -1;
    }

    auth_delegate_config_t cfg = {0};
    cfg.type = auth_type_comp_app;
    cfg.u.comp_app.redirect_uri = argv[1];
    cfg.u.comp_app.auth_code = argv[2];
    cfg.u.comp_app.client_id = argv[3];
    cfg.u.comp_app.code_verifier = "abcd1234";
    alexa_auth_delegate_signin(&cfg);                        
    return 0;
}

int app_auth_sign_in(void)    
{
    char redirect_uri_buf[50]={0};
    char auth_code_buf[50]={0};

    char client_id_buf[100]={0};
    auth_delegate_config_t cfg = {0};
     nvs_handle NVS_handle;

    esp_err_t err = nvs_open("custom_data", NVS_READWRITE, &NVS_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        size_t buf_len = sizeof(redirect_uri_buf);
        err =  nvs_get_str(NVS_handle, "redirect_uri", (char *)&redirect_uri_buf,&buf_len);
        if(err != ESP_OK){
             ESP_LOGE(TAG, "read redirect_uri parameters");
        }

        buf_len = sizeof(auth_code_buf);
        err =  nvs_get_str(NVS_handle, "auth_code", (char *)&auth_code_buf,&buf_len);
        if(err != ESP_OK){
             ESP_LOGE(TAG, "read auth_code parameters");
        }

        buf_len = sizeof(client_id_buf);
        err =  nvs_get_str(NVS_handle, "client_id", (char *)&client_id_buf,&buf_len);
        if(err != ESP_OK){
             ESP_LOGE(TAG, "read client_id parameters");
        }

        cfg.type = auth_type_comp_app;
        cfg.u.comp_app.redirect_uri = redirect_uri_buf;
        cfg.u.comp_app.auth_code = auth_code_buf;
        cfg.u.comp_app.client_id = client_id_buf;
        cfg.u.comp_app.code_verifier = "abcd1234";
        alexa_auth_delegate_signin(&cfg);   
    }                     
    return 0;
}

static int app_auth_sign_out_handler(int argc, char *argv[])
{
    /* Just to go to the next line */
    printf("\n");
    alexa_auth_delegate_signout();
    return 0;
}

static esp_console_cmd_t auth_cmds[] = {
    {
        .command = "sign-in",
        .help = "sign-in <redirect uri> <auth code> <client ID>",
        .func = app_auth_sign_in_handler,                             
    },
    {
        .command = "sign-out",
        .help = "sign-out",
        .func = app_auth_sign_out_handler,
    },
};

int app_auth_register_cli()
{
    int cmds_num = sizeof(auth_cmds) / sizeof(esp_console_cmd_t);
    int i;
    for (i = 0; i < cmds_num; i++) {
        ESP_LOGI(TAG, "Registering command: %s", auth_cmds[i].command);
        esp_console_cmd_register(&auth_cmds[i]);
    }
    return 0;
}
