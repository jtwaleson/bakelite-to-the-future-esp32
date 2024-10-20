#define MY_TAG "BUTTON_HANDLER"

#include "button_handlers.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "esp_log.h"
#include "esp_hf_client_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint8_t rotary_state = 0;
static uint8_t rotary_tick_count = 0;

static void red_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "RED_BUTTON_SINGLE_RELEASE");
}
static void red_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "RED_BUTTON_SINGLE_CLICK");
}
static void white_1_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_1_BUTTON_SINGLE_CLICK");
    gpio_set_level(INDICATOR_1_PIN, 1);
}
static void white_1_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_1_BUTTON_SINGLE_RELEASE");
    gpio_set_level(INDICATOR_1_PIN, 0);
}
static void white_2_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_2_BUTTON_SINGLE_CLICK");
    gpio_set_level(INDICATOR_2_PIN, 1);
}
static void white_2_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_2_BUTTON_SINGLE_RELEASE");
    gpio_set_level(INDICATOR_2_PIN, 0);
}
static void white_3_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_3_BUTTON_SINGLE_CLICK");
    gpio_set_level(BUZZER_PIN, 1);
}
static void white_3_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_3_BUTTON_SINGLE_RELEASE");
    gpio_set_level(BUZZER_PIN, 0);
}
static void white_4_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_4_BUTTON_SINGLE_CLICK");
}
static void white_4_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_4_BUTTON_SINGLE_RELEASE");
}
static void black_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "BLACK_BUTTON_SINGLE_CLICK");
}
static void black_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "BLACK_BUTTON_SINGLE_RELEASE");
}
static void horn_lift_button_slam_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "HORN_LIFT_BUTTON_SLAM");
    esp_hf_client_reject_call();
}
static void horn_lift_button_lift_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "HORN_LIFT_BUTTON_LIFT");
    stop_ringing_timer();
    // TODO set a timer to accept or auto reject if slammed
    esp_hf_client_answer_call();
}
static void rotary_on_single_click_cb(void *arg,void *usr_data) {
    ESP_LOGI(MY_TAG, "ROTARY_ON_SINGLE_CLICK");
    rotary_state = 1;
    rotary_tick_count = 0;
}
static void rotary_on_single_release_cb(void *arg,void *usr_data) {
    ESP_LOGI(MY_TAG, "ROTARY_ON_SINGLE_RELEASE");
    ESP_LOGI(MY_TAG, "ROTARY_TICK_COUNT: %d", rotary_tick_count);
    rotary_tick_count = 0;
    rotary_state = 0;
}
static void rotary_tick_single_click_cb(void *arg,void *usr_data) {
    ESP_LOGI(MY_TAG, "ROTARY_TICK_SINGLE_CLICK");
    if (rotary_state == 1) {
        rotary_tick_count++;
    }
}
static void rotary_tick_single_release_cb(void *arg,void *usr_data) {
    ESP_LOGI(MY_TAG, "ROTARY_TICK_SINGLE_RELEASE");
}

static TaskHandle_t ringing_timer_handle = NULL;
static void ringing_timer_cb(void *arg) {
    for (;;) {
        gpio_set_level(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(BUZZER_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(700));
    }
}
void start_ringing_timer(void) {
    xTaskCreate(ringing_timer_cb, "RingingTimer", 2048, NULL, 5, &ringing_timer_handle);
}
void stop_ringing_timer(void) {
    if (ringing_timer_handle) {
        vTaskDelete(ringing_timer_handle);
        ringing_timer_handle = NULL;
    }
    gpio_set_level(BUZZER_PIN, 0);
}


static void internal_led_timer_cb(void *arg) {
    for (;;) {
        gpio_set_level(INTERNAL_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(INTERNAL_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void init_button_handlers() {
    // if(NULL == gpio_btn) {
    //     ESP_LOGE(MY_TAG, "Button create failed");
    //     printf("Button create failed\n");
    // }
    // TODO just assume everything is ok
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config = {
            .gpio_num = BUTTON_RED_PIN,
            .active_level = 0,
        },
    };
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, red_button_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, red_button_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = BUTTON_WHITE_1_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, white_1_button_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, white_1_button_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = BUTTON_WHITE_2_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, white_2_button_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, white_2_button_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = BUTTON_WHITE_3_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, white_3_button_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, white_3_button_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = BUTTON_WHITE_4_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, white_4_button_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, white_4_button_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = BUTTON_BLACK_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, black_button_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, black_button_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = HORN_LIFT_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, horn_lift_button_lift_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, horn_lift_button_slam_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = ROTARY_ON_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, rotary_on_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, rotary_on_single_release_cb,NULL);

    gpio_btn_cfg.gpio_button_config.gpio_num = ROTARY_TICK_PIN;
    gpio_btn = iot_button_create(&gpio_btn_cfg);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_DOWN, rotary_tick_single_click_cb,NULL);
    iot_button_register_cb(gpio_btn, BUTTON_PRESS_UP, rotary_tick_single_release_cb,NULL);

    // set out gpio output for indicator 1
    gpio_set_direction(INDICATOR_1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(INDICATOR_1_PIN, 0);
    // set out gpio output for indicator 2
    gpio_set_direction(INDICATOR_2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(INDICATOR_2_PIN, 0);
    // set out gpio output for buzzer
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_PIN, 0);
    // set out gpio output for internal led
    gpio_set_direction(INTERNAL_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(INTERNAL_LED_PIN, 0);


    xTaskCreate(internal_led_timer_cb, "InternalLedTimer", 2048, NULL, 5, NULL);
}
