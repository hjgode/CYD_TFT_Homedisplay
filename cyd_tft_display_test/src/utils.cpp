#include "utils.h"
#include <driver/ledc.h>

std::string utils::padLeft(const std::string& str, size_t totalLength, char padChar) {    
    if (str.length() >= totalLength) 
        return str;    
    return std::string(totalLength - str.length(), padChar) + str;
}

std::string utils::padTo(std::string &str, const size_t num, const char paddingChar)
{
    if(num > str.size())
        str.insert(0, num - str.size(), paddingChar);
    return str;
}

void utils::getLeftPaddingString(std::string &str, int n, char padChar = ' ')
{
	if (n <= str.size()) {
		return;
	}
    try{
    	str.insert(0, n - str.size(), padChar);
    }      
    catch(const std::exception& e)
    {
        Serial.println(e.what());
    }

}

void utils::led(uint8_t red, uint8_t green, uint8_t blue, bool true_color=true){
    // Serial.printf("LED: r=%i, g=%i, b=%i, true=%i\n", red, green, blue, true);
    bool led_used_r=false;
    bool led_used_g=false;
    bool led_used_b=false;
  if (true_color) {
    red   = 255 - (red   * R_CORRECTION);
    green = 255 - (green * G_CORRECTION);
    blue  = 255 - (blue  * B_CORRECTION);
  } else {
    red   = 255 - red;
    green = 255 - green;
    blue  = 255 - blue;
  }
  if (red < 255)   led_used_r = true;
  if (green < 255) led_used_g = true;
  if (blue < 255)  led_used_b = true;
  if (led_used_r) analogWrite(LED_R, red);
  if (led_used_g) analogWrite(LED_G, green);
  if (led_used_b) analogWrite(LED_B, blue);    

}

void utils::set_BL(uint8_t brightness=50){
      //PWM on TFT_BL ????
/*  pinMode(TFT_BL, OUTPUT);
  uint8_t ledChannel = SOC_LEDC_CHANNEL_NUM;
  ledcAttachPin(TFT_BL, ledChannel);
  ledcSetup(1, 25000, 8);
  ledcWrite(1, 128);
*/  
    esp_err_t err;
      ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 4000,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    //ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    // Prepare and then apply the LEDC PWM channel configuration
    err=ledc_timer_config(&ledc_timer);
    if (err != ESP_OK)
      Serial.println("ledc_timer_config FAILD"); 

    ledc_channel_config_t ledc_channel = {
        .gpio_num       = TFT_BL,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags          = { .output_invert = false },
    };
//   ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK)
      Serial.println("ledc_channel_config FAILD"); 
       // Set duty to 50%
    uint32_t duty = 8192;
    if (brightness<0)
        duty=0;
    else if(brightness<=25)
      duty=2048;
    else if (brightness<=50)
      duty=4096;
    else if(brightness<=75)
      duty=4096+2048;
    else 
      duty=4096;

    duty = 8192 * (brightness / 100.0); //with 100 instead of 100.0 always get duty=0 !!!
 //    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    err = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    if (err != ESP_OK)
      Serial.println("ledc_set_duty FAILD"); 
    else
      Serial.printf("\nduty cycle set to %i\n", duty);
    //(4096) // Set duty to 50%. (2 ** 13) * 50% = 4096

    // Update duty to apply the new value
    //ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    err=ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    if (err != ESP_OK)
      Serial.println("ledc_update_duty FAILD"); 

}