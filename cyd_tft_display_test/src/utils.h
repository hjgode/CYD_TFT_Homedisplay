#include <Arduino.h>

//from https://github.com/ropg/LVGL_CYD/blob/main/src/LVGL_CYD.cpp
// RGB-LED
#define LED_R         4
#define R_CORRECTION  0.25
#define LED_G         16
#define G_CORRECTION  1
#define LED_B         17
#define B_CORRECTION  0.4
#define LED_FREQ      5000
#define LED_RES       8

// prevent brief PWM startup flash when LED is never used.
class utils{
    public:
        static void led(uint8_t red, uint8_t green, uint8_t blue, bool true_color);
        static void set_BL(uint8_t brightness);

};
