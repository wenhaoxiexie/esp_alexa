// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#pragma once

#include "sdkconfig.h"

#ifdef CONFIG_ALEXA_ENABLE_OTA
void app_cloud_init();
void app_cloud_start();
#endif /* CONFIG_ALEXA_ENABLE_OTA */
