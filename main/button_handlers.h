#define BUTTON_RED_PIN 19
#define BUTTON_WHITE_1_PIN 23
#define BUTTON_WHITE_2_PIN 4
#define BUTTON_WHITE_3_PIN 22
#define BUTTON_WHITE_4_PIN 21
#define BUTTON_BLACK_PIN 2 // 5 is green led
#define HORN_LIFT_PIN 18

#define ROTARY_ON_PIN 35
#define ROTARY_TICK_PIN 27

#define INDICATOR_1_PIN 33
#define INDICATOR_2_PIN 32
#define BUZZER_PIN 26

#define INTERNAL_LED_PIN 5

void start_ringing_timer(void);
void stop_ringing_timer(void);
void init_button_handlers(void);
static void startup_buzz(void *arg);