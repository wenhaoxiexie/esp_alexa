// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>
#include <esp_log.h>

#include <speech_recognizer.h>
#include <voice_assistant.h>
#include <va_mem_utils.h>
#include <va_nvs_utils.h>
#include <media_hal.h>

#include "va_dsp.h"
#include "va_button.h"
#include "hollow_dsp_init.h"
#include "esp_pm.h"
#include "driver/i2s.h"

#define EVENTQ_LENGTH   10
#define STACK_SIZE      16 * 1024
#define DSP_NVS_KEY "dsp_mute"

static const char *TAG = "[va_dsp]";

static int8_t dsp_mute_en;

#ifdef CONFIG_PM_ENABLE
#define LOW_POWER_TIMER_SECS 10  //Custom DSP will be put into low power mode if no audio playback has occurred in last LOW_POWER_TIMER_SECS
static bool is_first_playback;
static bool do_power_switch;
static TimerHandle_t playback_timer;
static esp_pm_lock_handle_t playback_pm;
#endif
//static int ww_cnt;

enum va_dsp_state {
    STREAMING,
    STOPPED,
    MUTED,
};

static enum va_dsp_state dsp_state;
static QueueHandle_t cmd_queue;
static uint8_t audio_buf[AUDIO_BUF_SIZE];
static bool va_dsp_booted = false;

int va_app_playback_starting();

static inline void _enable_power_switch()
{
#ifdef CONFIG_PM_ENABLE
    do_power_switch = true;
#endif
}

static inline void _disable_power_switch()
{
#ifdef CONFIG_PM_ENABLE
    do_power_switch = false;
#endif
}

static inline void _va_dsp_stop_streaming()
{
    custom_dsp_stop_capture();
    dsp_state = STOPPED;
}

static inline void _va_dsp_start_streaming()
{
    custom_dsp_start_capture();
    dsp_state = STREAMING;
}

static inline void _va_dsp_mute_mic()
{
    if (dsp_state == STREAMING) {
        custom_dsp_stop_capture();
    }
    custom_dsp_mic_mute();
    _disable_power_switch();
    dsp_state = MUTED;
}

static inline void _va_dsp_unmute_mic()
{
    custom_dsp_mic_unmute();
    dsp_state = STOPPED;
}

static inline void _va_dsp_low_power()
{
#ifdef CONFIG_PM_ENABLE
    printf("Entering DSPG Low Power\n");
    if (dsp_state != MUTED) {
        custom_dsp_enter_low_power();
    }
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_GPIO0);
    gpio_set_level(GPIO_NUM_0, 0);
    _enable_power_switch();
    is_first_playback = true;
    esp_pm_lock_release(playback_pm);
#else
    printf("Error: Low Power Mode disabled\n");
#endif
}

#ifdef CONFIG_PM_ENABLE
static void timer_cb(TimerHandle_t xTimer)
{
    printf("One Shot Timer Initiated\n");
    struct dsp_event_data new_event = {
        .event = POWER_SAVE
    };
    xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
}
#endif

static void va_dsp_thread(void *arg)
{
    struct dsp_event_data event_data;
    //uint32_t timeout = portMAX_DELAY;
    while(1) {
        xQueueReceive(cmd_queue, &event_data, portMAX_DELAY);
        //timeout = 0;
        switch (dsp_state) {
            case STREAMING:
                switch (event_data.event) {
                    case TAP_TO_TALK:
                        /* Stop the streaming */
                        _va_dsp_stop_streaming();
                        break;
                    case GET_AUDIO: {
                        int read_len = custom_dsp_stream_audio(audio_buf, AUDIO_BUF_SIZE);
                        if (read_len > 0) {
                            speech_recognizer_record(audio_buf, read_len);
                            struct dsp_event_data new_event = {
                                .event = GET_AUDIO
                            };
                            xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
                        } else {
                            _va_dsp_stop_streaming();
                        }
                        break;
                    }
                    case STOP_MIC:
                        _va_dsp_stop_streaming();
                        break;
                    case MUTE:
                        _va_dsp_mute_mic();
                        break;
                    case POWER_SAVE:
                        va_app_playback_starting();
                    case WW:
                    case START_MIC:
                    case UNMUTE:
                    default:
                        printf("%s: Event %d unsupported in STREAMING state\n", TAG, event_data.event);
                        break;
                }
                break;
            case STOPPED:
                switch (event_data.event) {
                    case WW: {
                        size_t phrase_length = custom_dsp_get_ww_len();
                        if (phrase_length == 0) {
                            /*XXX: Should we close the stream here?*/
                            break;
                        }
                        _disable_power_switch();
                        if (speech_recognizer_recognize(phrase_length, WAKEWORD) == 0) {
                            struct dsp_event_data new_event = {
                                .event = GET_AUDIO
                            };
                            xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
                            dsp_state = STREAMING;
                        } else {
                            printf("%s: Error starting a new dialog..stopping capture\n", TAG);
                            custom_dsp_stop_capture();
                            _enable_power_switch();
                        }
                        break;
                    }
                    case TAP_TO_TALK:
                        _disable_power_switch();
                        if (speech_recognizer_recognize(0, TAP) == 0) {
                            _va_dsp_start_streaming();
                            struct dsp_event_data new_event = {
                                .event = GET_AUDIO
                            };
                            xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
                        } else {
                            printf("%s: Error starting a new dialog\n", TAG);
                            _enable_power_switch();
                        }
                        break;
                    case START_MIC:
                        _va_dsp_start_streaming();
                        struct dsp_event_data new_event = {
                            .event = GET_AUDIO
                        };
                        xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
                        break;
                    case MUTE:
                        _va_dsp_mute_mic();
                        break;
                    case POWER_SAVE:
                        _va_dsp_low_power();
                        break;
                    case GET_AUDIO:
                    case STOP_MIC:
                    case UNMUTE:
                    default:
                        printf("%s: Event %d unsupported in STOPPED state\n", TAG, event_data.event);
                        break;
                }
                break;
            case MUTED:
                switch (event_data.event) {
                    case UNMUTE:
                        _va_dsp_unmute_mic();
                        break;
                    case POWER_SAVE:
                        _va_dsp_low_power();
                        break;
                    case WW:
                    case TAP_TO_TALK:
                    case GET_AUDIO:
                    case START_MIC:
                    case STOP_MIC:
                    case MUTE:
                    default:
                        printf("%s: Event %d unsupported in MUTE state\n", TAG, event_data.event);
                        break;
                }
                break;

            default:
                printf("%s: Unknown state %d with Event %d\n", TAG, dsp_state, event_data.event);
                break;
        }
    }
}

int va_app_speech_stop()
{
    printf("%s: Sending stop command\n", TAG);
    struct dsp_event_data new_event = {
        .event = STOP_MIC
    };
    xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
    return 0;
}

int va_app_speech_start()
{
    printf("%s: Sending start speech command\n", TAG);
    struct dsp_event_data new_event = {
        .event = START_MIC
    };
    xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
    return 0;
}

int va_app_playback_starting()
{
#ifdef CONFIG_PM_ENABLE
    if (is_first_playback) {
        esp_pm_lock_acquire(playback_pm);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        is_first_playback = false;
    }

    if (do_power_switch) {
        custom_dsp_aec_init();
        _disable_power_switch();
    }

    if (!playback_timer) {
        ESP_LOGE(TAG, "Playback timer not created");
        return -1;
    }

    xTimerReset(playback_timer, 0);
#endif
    return 0;
}

int va_dsp_tap_to_talk_start()
{
    printf("%s: Sending start for tap to talk command\n", TAG);
    struct dsp_event_data new_event = {
        .event = TAP_TO_TALK
    };
    xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
    return ESP_OK;
}

void va_dsp_reset()
{
    if (va_dsp_booted == true) {
        custom_dsp_reset();
    }
}

void va_dsp_mic_mute(bool mute)
{
#ifdef CONFIG_PM_ENABLE
    if (is_first_playback) {
        esp_pm_lock_acquire(playback_pm);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
        _disable_power_switch();
        is_first_playback = false;
    }
#endif

    struct dsp_event_data new_event;
    if (mute)
        new_event.event = MUTE;
    else
        new_event.event = UNMUTE;
    va_nvs_set_i8(DSP_NVS_KEY, mute);
    xQueueSend(cmd_queue, &new_event, portMAX_DELAY);
}

void va_dsp_init(void)
{
    TaskHandle_t xHandle = NULL;
    StackType_t *task_stack = (StackType_t *)va_mem_alloc(STACK_SIZE, VA_MEM_INTERNAL);
    static StaticTask_t task_buf;

    cmd_queue = xQueueCreate(10, sizeof(struct dsp_event_data));
    if (!cmd_queue) {
        ESP_LOGE(TAG, "Error creating va_dsp queue");
        return;
    }

    custom_dsp_init(cmd_queue);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "Playback Lock", &playback_pm);
    playback_timer = xTimerCreate("playback_timer", LOW_POWER_TIMER_SECS * 1000 / portTICK_RATE_MS, pdFALSE, NULL, timer_cb);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_GPIO0);
    gpio_set_level(GPIO_NUM_0, 0);
    _enable_power_switch();
    is_first_playback = true;
#else
    custom_dsp_aec_init();
#endif
    //DSP state is stopped while device reboot / initialization 
    dsp_state = STOPPED;
    //Check if device was Mute or Unmute before power off / reboot and set the last state device was in
    if (va_nvs_get_i8(DSP_NVS_KEY, &dsp_mute_en) == ESP_OK) {
        if (dsp_mute_en) {
            va_dsp_mic_mute(dsp_mute_en);
            va_button_notify_mute(dsp_mute_en);
        }
    }

    xHandle = xTaskCreateStatic(va_dsp_thread, "dsp-thread", STACK_SIZE,
                                NULL, CONFIG_ESP32_PTHREAD_TASK_PRIO_DEFAULT, task_stack, &task_buf);
    if (xHandle == NULL) {
        ESP_LOGE(TAG, "Couldn't create thead");
    }
    //Notify Alexa layer that DSP initialization is finished
    va_boot_dsp_signal();
    va_dsp_booted = true;
}
