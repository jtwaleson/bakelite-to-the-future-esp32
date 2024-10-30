#define MY_TAG "BUTTON_HANDLER"

#include <string.h>
#include "esp_adc/adc_oneshot.h"
#include "button_handlers.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "esp_log.h"
#include "esp_hf_client_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint8_t rotary_state = 0;
static bool is_ringing = false;
static uint8_t rotary_tick_count = 0;
static uint8_t phone_number_dialing[16];
static uint8_t phone_number_current_writing = 0;

static TaskHandle_t dialing_handle = NULL;
static uint8_t amount_of_ticks_left = 0;
static TaskHandle_t pickup_handle = NULL;

extern esp_bd_addr_t peer_addr;
// provide env vars like BAKELITE_PHONE_MAC="{0x11, 0x11, 0x11, 0x11, 0x11, 0x11}"   
esp_bd_addr_t laptop_addr = BAKELITE_LAPTOP_MAC;
esp_bd_addr_t phone_addr = BAKELITE_PHONE_MAC;

static void clear_dialing_number() {
    phone_number_current_writing = 0;
    for (int i = 0; i < sizeof(phone_number_dialing)/sizeof(phone_number_dialing[0]); i++) {
        phone_number_dialing[i] = 0;
    }
}
static void start_dialing_routine(void *arg) {
    while (amount_of_ticks_left > 0) {
        gpio_set_level(INDICATOR_2_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(INDICATOR_2_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
        amount_of_ticks_left--;
    }
    clear_dialing_number();
    dialing_handle = NULL;
    vTaskDelete(NULL);
}
static void add_digit_to_dialing(uint8_t digit) {
    amount_of_ticks_left = 15;
    if (!dialing_handle) {
        xTaskCreate(start_dialing_routine, "DialingNumbers", 2048, NULL, configMAX_PRIORITIES - 3, &dialing_handle);
    }
    if (phone_number_current_writing > 15) {
        ESP_LOGE("DIALER", "PHONE NUMBER TOO BIG");
        amount_of_ticks_left = 0;
        if (!dialing_handle) {
            vTaskDelete(dialing_handle);
            dialing_handle = NULL;
        }
        gpio_set_level(INDICATOR_2_PIN, 0);
        xTaskCreate(startup_buzz, "Dial error", 1024, NULL, 5, NULL);
        return;
    }
    phone_number_dialing[phone_number_current_writing] = digit;
    phone_number_current_writing++;

    // Create a buffer to store the phone number as a string
    char phone_number_str[17]; // 16 digits + null terminator
    for (int i = 0; i < phone_number_current_writing; i++) {
        phone_number_str[i] = '0' + phone_number_dialing[i]; // Convert digit to char
    }
    phone_number_str[phone_number_current_writing] = '\0'; // Null-terminate the string

    // Log the current assembled phone number
    ESP_LOGI("DIALER", "Current number: %s", phone_number_str);
}
static void red_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "RED_BUTTON_SINGLE_RELEASE");
}
static void red_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "RED_BUTTON_SINGLE_CLICK");
    if (is_ringing) {
        ESP_LOGI(MY_TAG, "rejecting call");
        esp_hf_client_reject_call();
    } else {
        ESP_LOGI(MY_TAG, "connecting to phone");
        esp_hf_client_disconnect(peer_addr);
        memcpy(&peer_addr, &phone_addr, sizeof peer_addr);
        ESP_ERROR_CHECK(esp_hf_client_connect(peer_addr));
    }
}
static void white_1_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_1_BUTTON_SINGLE_CLICK");
    gpio_set_level(BUZZER_PIN, 1);
}
static void white_1_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_1_BUTTON_SINGLE_RELEASE");
    ESP_LOGI(MY_TAG, "Clearing memory num");
    gpio_set_level(BUZZER_PIN, 0);
    clear_dialing_number();
}
static void white_2_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_2_BUTTON_SINGLE_CLICK");
}
static void white_2_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_2_BUTTON_SINGLE_RELEASE");
}
static void white_3_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_3_BUTTON_SINGLE_CLICK");
}
static void white_3_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_3_BUTTON_SINGLE_RELEASE");
}
static void white_4_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_4_BUTTON_SINGLE_CLICK");
    esp_hf_client_start_voice_recognition();
}
static void white_4_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "WHITE_4_BUTTON_SINGLE_RELEASE");
    esp_hf_client_stop_voice_recognition();
}
static void black_button_single_click_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "BLACK_BUTTON_SINGLE_CLICK");
    ESP_LOGI(MY_TAG, "connecting to laptop");
    esp_hf_client_disconnect(peer_addr);
    memcpy(&peer_addr, &laptop_addr, sizeof peer_addr);
    ESP_ERROR_CHECK(esp_hf_client_connect(peer_addr));
}
static void black_button_single_release_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "BLACK_BUTTON_SINGLE_RELEASE");
}
static void horn_lift_button_slam_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "HORN_LIFT_BUTTON_SLAM");
    esp_hf_client_reject_call();
    esp_hf_client_stop_voice_recognition();
}
static void horn_lift_button_lift_cb(void *arg,void *usr_data)
{
    ESP_LOGI(MY_TAG, "HORN_LIFT_BUTTON_LIFT");
    stop_ringing_timer();
    // TODO set a timer to accept or auto reject if slammed
    if (phone_number_current_writing > 1) {
        ESP_LOGI("DIALER", "Dialing");
        char phone_number_str[17]; // 16 digits + null terminator
        for (int i = 0; i < phone_number_current_writing; i++) {
            phone_number_str[i] = '0' + phone_number_dialing[i]; // Convert digit to char
        }
        phone_number_str[phone_number_current_writing] = '\0'; // Null-terminate the string
        ESP_LOGI("DIALER", "Dialing %s", phone_number_str);
        esp_hf_client_dial(phone_number_str);
        clear_dialing_number();
    } else if (is_ringing) {
        ESP_LOGI("DIALER", "Answering call");
        esp_hf_client_answer_call();
    } else {
        ESP_LOGI("DIALER", "Starting voice recognition");
        esp_hf_client_start_voice_recognition();
    }
}
static void rotary_on_single_click_cb(void *arg,void *usr_data) {
    // ESP_LOGI(MY_TAG, "ROTARY_ON_SINGLE_CLICK");
    rotary_state = 1;
    rotary_tick_count = 0;
}
static void rotary_on_single_release_cb(void *arg,void *usr_data) {
    // ESP_LOGI(MY_TAG, "ROTARY_ON_SINGLE_RELEASE");
    ESP_LOGI(MY_TAG, "ROTARY_TICK_COUNT: %d", rotary_tick_count % 10);
    vTaskDelay(pdMS_TO_TICKS(500));
    add_digit_to_dialing(rotary_tick_count % 10);
    rotary_tick_count = 0;
    rotary_state = 0;
}
static void rotary_tick_single_click_cb(void *arg,void *usr_data) {
    // ESP_LOGI(MY_TAG, "ROTARY_TICK_SINGLE_CLICK");
    if (rotary_state == 1) {
        rotary_tick_count++;
    }
}
static void rotary_tick_single_release_cb(void *arg,void *usr_data) {
    // ESP_LOGI(MY_TAG, "ROTARY_TICK_SINGLE_RELEASE");
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
adc_oneshot_unit_handle_t adc1_handle;
adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
};
static void measure_battery_status(void *arg) {
    // TODO this doesn't work
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    int answer = 0;
    for (;;) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &answer));
        ESP_LOGI("BAT", "%d", answer);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
void start_ringing_timer(void) {
    is_ringing = true;
    xTaskCreate(ringing_timer_cb, "RingingTimer", 2048, NULL, 5, &ringing_timer_handle);
}
void stop_ringing_timer(void) {
    is_ringing = false;
    if (ringing_timer_handle) {
        vTaskDelete(ringing_timer_handle);
        ringing_timer_handle = NULL;
    }
    gpio_set_level(BUZZER_PIN, 0);
}


static void startup_buzz(void *arg) {
    for (int i = 0; i < 2; i++) {
        gpio_set_level(INDICATOR_1_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(INDICATOR_1_PIN, 0);
        gpio_set_level(INDICATOR_2_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(600));
        gpio_set_level(INDICATOR_2_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(600));
        // gpio_set_level(BUZZER_PIN, 1);
        // vTaskDelay(pdMS_TO_TICKS(200));
        // gpio_set_level(BUZZER_PIN, 0);
    }
    vTaskDelete(NULL);
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
    xTaskCreate(startup_buzz, "Startup buzz", 1024, NULL, 5, NULL);
    // xTaskCreate(measure_battery_status, "Measure battery", 1024, NULL, 5, NULL);
}
