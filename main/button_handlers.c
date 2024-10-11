// 5 17 18 23 19 21 22
#define BUTTON_RED_PIN 19 // yes
#define BUTTON_WHITE_1_PIN 23 // yes
#define BUTTON_WHITE_2_PIN 4 // TODO
#define BUTTON_WHITE_3_PIN 22 //yes     
#define BUTTON_WHITE_4_PIN 21 // yes
#define BUTTON_BLACK_PIN 5 // yes
#define HORN_LIFT_PIN 18 // yes

#define INDICATOR_1_PIN 33
#define INDICATOR_2_PIN 32
#define BUZZER_PIN 26

#define MY_TAG "BUTTON_HANDLER"

#include "driver/gpio.h"
#include "iot_button.h"
#include "esp_log.h"


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
}
static void horn_lift_button_lift_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "HORN_LIFT_BUTTON_LIFT");
}


void init_button_handlers() {
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config = {
            .gpio_num = BUTTON_RED_PIN,
            .active_level = 0,
        },
    };
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    // if(NULL == gpio_btn) {
    //     ESP_LOGE(MY_TAG, "Button create failed");
    //     printf("Button create failed\n");
    // }
    // TODO just assume everything is ok
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


    // set out gpio output for indicator 1
    gpio_set_direction(INDICATOR_1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(INDICATOR_1_PIN, 0);
    // set out gpio output for indicator 2
    gpio_set_direction(INDICATOR_2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(INDICATOR_2_PIN, 0);
    // set out gpio output for buzzer
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_PIN, 0);
}



// function
