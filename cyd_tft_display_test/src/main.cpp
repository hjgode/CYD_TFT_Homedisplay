#include "driver/ledc.h"

#include <XPT2046_Touchscreen.h>
#include "SPI.h"
#include "TFT_eSPI.h"
//#include <TFT_eWidget.h>
//#include "Extensions/Button.h"

#include <WiFi.h>
#include <PubSubClient.h>

//#include "FreeMonoBold20pt8b.h"
//#include "FreeMonoBold18pt8b.h"
#include "FreeMonoBold16pt8b.h"
#include "FreeMonoBold10pt8b.h"
#include "FreeMonoBold8pt8b.h"

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen xpt(XPT2046_CS, XPT2046_IRQ);

uint16_t xptX, xptY, tftX, tftY; 
uint8_t xptZ;

//#include "ringmeter.h"
#include "toggle_switch.h"
toggle_switch tSwitch;
// Licht Terasse1
bool stateLichtTerasse1 = false;

// Touch
#define CS_PIN XPT2046_CS
// The TIRQ interrupt signal must be used for this example.
#define TIRQ_PIN    XPT2046_IRQ
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling

#ifndef LOAD_GXFF
#define LOAD_GFXFF
#endif

#ifdef LOAD_GFXFF // Only include the fonts if LOAD_GFXFF is defined in User_Setup.h

    // Use these when printing or drawing text in GLCD and high rendering speed fonts
    #define GFXFF 1
    #define GLCD  0
    #define FONT2 2
    #define FONT4 4
    #define FONT6 6
    #define FONT7 7
    #define FONT8 8
    #define FF7 &FreeMonoBold18pt7b
    #define sFF7 "Mono bold 18"
#endif

const char *ssid="Horst1";
const char *passwd = "1234567890123";

String myText1=""; //Aussen
String myText2=""; //Schlaf
String myText3=""; //Wohnz.
String myText4=""; //Terrasse
String myText5=""; //Andreas
String myText6=""; //Bad
String myClockText="clock";
String myIPText="ip";

String tempStrings[]={"myText1","myText2","myText3","myText4","myText5","myText6"};

int currentRow=0;
int currentScreen=1;

// read position of XPT digitizer and corresponding TFT position
void xptPosition (uint16_t *xptX, uint16_t *xptY, uint8_t *xptZ, uint16_t *tftX, uint16_t *tftY) {
  uint16_t x, y; uint8_t z;  // XPT
  uint16_t tx, ty;           // TFT

  float xx = (XPT2046_XMAX - XPT2046_XMIN); // width XPT-Points
  float yy = (XPT2046_YMAX - XPT2046_YMIN); // height XPT-Points

  xpt.readData(&x, &y, &z);

  // calc position for TFT display from digitizer position
  tx = (x / xx * TFT_HEIGHT) - (XPT2046_XMIN / xx * TFT_HEIGHT); // TFT_HEIGHT=320
  ty = (y / yy * TFT_WIDTH) - (XPT2046_YMIN / yy * TFT_WIDTH);   // TFT_WIDTH=240

  // avoid invalid values
  if (tx < 0) tx = 0;
  if (ty < 0) ty = 0;
  if (tx > TFT_HEIGHT-1) tx = TFT_HEIGHT-1;
  if (ty > TFT_WIDTH-1) ty = TFT_WIDTH-1;

	*xptX = x;
  *xptY = y;
  *xptZ = z;
  *tftX = tx;
  *tftY = ty;
}

void printMsg(String msg){
    Serial.printf("printMsg: '%s'", msg);
  tft.drawString(msg, 10, 10 + currentRow * 30, GFXFF);// Print the string name of the font
  currentRow++;
  if (currentRow>6)
    currentRow=0;
  //tft.setCursor(0,15);
  //tft.print(msg);
//  tft.println(42);
}

void printMsg(String msg, int line){
    Serial.printf("printMsg: '%s'", msg);
  tft.drawString(msg, 10, 10 + line * 30, GFXFF);// Print the string name of the font
  //tft.setCursor(0,15);
  //tft.print(msg);
//  tft.println(42);
}

void setFontNormal(){
  tft.setFreeFont(&FreeMonoBold16pt8b);
}
void setFontSmall(){
  tft.setFreeFont(&FreeMonoBold8pt8b);
}

void updateScreen1Line(int line){

  tft.setFreeFont(&FreeMonoBold16pt8b);
  tft.drawString(tempStrings[line], 10, 10 + line * 30, GFXFF);// Print the string name of the font
}

void updateClockLine(){

  setFontSmall();
  int i=7;
  tft.drawString(myIPText, 10, 10 + i * 30, GFXFF);// Print the string name of the font
  
  tft.drawString(myClockText, 160, 10 + i * 30, GFXFF);// Print the string name of the font

}

void drawScreen1(){
//  if (currentScreen==1)
//    return;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  int xOffset=0;
  int yOffset=0;
  int linespace=30;
  int linenumber=0;
  tft.setFreeFont(&FreeMonoBold16pt8b);
  for (int i=0;i<6;i++){
    tft.drawString(tempStrings[i], 10, 10 + i * 30, GFXFF);// Print the string name of the font
  }
  setFontSmall();
  int i=7;
  tft.drawString(myIPText, 10, 10 + i * 30, GFXFF);// Print the string name of the font
  
  tft.drawString(myClockText, 160, 10 + i * 30, GFXFF);// Print the string name of the font
  currentScreen=1;
}


void drawScreen2(){
  if (currentScreen==2)
    return;
  tft.fillScreen(TFT_BLACK);
  setFontNormal();
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 30);
  tft.print("Terrasse 1");

//  toggle_switch tSwitch;
  tSwitch.initSwitch(&tft, 216, 12, 50, 20, TFT_WHITE, TFT_WHITE, TFT_BLUE);
  tSwitch.drawButton(stateLichtTerasse1);
  /*
  //x, y, w, h, radius, color
  tft.fillRoundRect(216, 12, 50, 20, 10, TFT_WHITE);
  tft.drawRoundRect(216, 12, 50, 20, 10, TFT_WHITE);
  tft.fillCircle(226, 22, 8, TFT_SKYBLUE);
  */
/*
bool TFT_eSPI_Button::contains(int16_t x, int16_t y) {
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}
*/
  currentScreen=2;
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print ("Message arrived on Topic:");
    Serial.print (topic);

    // Convert payload to string    
    String payloadStr = String((char*)payload);
    
    String msgStr;

    for(int i=0;i<length;i++){
        msgStr += (char)payload[i];
    }
    Serial1.print(msgStr);
    Serial1.println();

    //assign text lines?
    String topicS="";
    topicS+=topic;

    // shellies/shelly1-ABF975/relay/0
    if (topicS.indexOf("shelly1-ABF975")>0){
      Serial.printf("\nlichtTerasse1 %s\n", msgStr.c_str());
      if (msgStr.endsWith("off")){
        Serial.println("lichtTerasse1 ist OFF");
        stateLichtTerasse1=false;
      }else{
        Serial.println("lichtTerasse1 ist ON");
        stateLichtTerasse1=true;
      }
      try
      {
        /* code */
        tSwitch.setState(stateLichtTerasse1);
      }
      catch(const std::exception& e)
      {
        Serial.println(e.what());
      }
      
      return;
    }

    if(topicS.indexOf( "clock" )>0){
        myClockText=msgStr;
        myIPText=WiFi.localIP().toString();
        updateClockLine();
    }

    //get last char
    topicS.toInt();
    char lastChar=topicS[strlen(topic) - 1];
    int tIndex = String(lastChar).toInt();
    //store string in list
    if (tIndex<=6 && tIndex>0){
      tempStrings[tIndex-1]=msgStr;
      //update text on Screen 1?

    }

    //draw data line
//    if (topicS.indexOf("text")>0)
//        printMsg(msgStr,tIndex-1);
    if(currentScreen==1){
      //drawScreen1();
      updateScreen1Line(tIndex-1);
    }
/*
    if(topicS.indexOf( "text1" )>0)
        myText1=msgStr;
    else if(topicS.indexOf( "text2" )>0)
        myText2=msgStr;
    else if(topicS.indexOf( "text3" )>0)
        myText3=msgStr;
    else if(topicS.indexOf( "text4" )>0)
        myText4=msgStr;
    else if(topicS.indexOf( "text5" )>0)
        myText5=msgStr;
    else if(topicS.indexOf( "text6" )>0)
        myText6=msgStr;
    else if(topicS.indexOf( "clock" )>0)
        myClockText=msgStr;
//    else if(topicS.indexOf( "ip" )>0)
//        myIPText=msgStr;
*/
}

WiFiClient wlanclient;
PubSubClient mqttClient(wlanclient);

void connectWiFi(){
    Serial.print("Connecting to Wifi");

    WiFi.begin(ssid,passwd);

    while(WiFi.status()!=WL_CONNECTED) {
        Serial.print (".");
        delay(100);
    }
    Serial.print ("Connected to WiFi AP, Got an IP address :");
    Serial.print (WiFi.localIP());

    mqttClient.setServer ("192.168.0.40",1883);
    mqttClient.setCallback(mqttCallback);

    if (mqttClient.connect ("CYD-Client",NULL,NULL))
    {
        Serial.print ("Connected to MQTT Broker");
    } else {
        Serial.print("MQTT Broker connection failed");
        Serial.print (mqttClient.state());
        delay(200);
    }
    mqttClient.subscribe("display1/#");
    mqttClient.subscribe("shellies/shelly1-ABF975/relay/0");// ("fhem/licht_terasse1"); 
}

void mqttSendFHEMcmnd(String cmnd){
  //state in mqtt fhem/licht_terasse1
  Serial.printf("\nmqttSendFHEMcmnd '%s'\n",cmnd.c_str());
  if (mqttClient.connected()){
    Serial.println("mqttClient connected send FHEM cmnd");
    mqttClient.publish("fhem/cmnd", cmnd.c_str(), true);
  }
  else{
    bool bRes = mqttClient.connect("cyd");
    if(bRes){
      Serial.println("mqttClient connected2 send FHEM cmnd");
      mqttClient.publish("fhem/cmnd", cmnd.c_str(), true);
    }else{
      Serial.println("mqttClient connect failed");
    }
  }
}

void SwitchOn(){
  String msg="set shelly1_Terasse on";
  Serial.print(msg);
  mqttSendFHEMcmnd(msg);
}
void SwitchOff(){
  String msg="set shelly1_Terasse off";
  Serial.print(msg);
  mqttSendFHEMcmnd(msg);
}
    

void set_BL(uint8_t brightness=50){
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
static bool led_used_r = false;
static bool led_used_g = false;
static bool led_used_b = false;
void led(uint8_t red, uint8_t green, uint8_t blue, bool true_color=true) {
  // Serial.printf("LED: r=%i, g=%i, b=%i, true=%i\n", red, green, blue, true);
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

int get_LDR(){
  pinMode(LDR_PIN, INPUT);
  analogSetAttenuation(ADC_0db);
  int value=analogRead(LDR_PIN);
  Serial.printf("\nLDR is %i\n");
  return value;
}

TFT_eSPI_Button button1;

void setup(void) {
  Serial.begin (115200);

  tft.begin();
  
  set_BL(50);

  led(0,100,0,false);
//  set_RGB_LED(0,1,0);


  /*
  TFT_eSPI *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h,
  uint16_t outline, uint16_t fill, uint16_t textcolor,
  char *label, uint8_t textsize)
  {
    _x1           = x1;
    _y1           = y1;
    _w            = w;
    _h            = h;
    _outlinecolor = outline;
    _fillcolor    = fill;
    _textcolor    = textcolor;
    _textsize     = textsize;
    _gfx          = gfx;
    strncpy(_label, label, 9);
    */
    //button1.initButton(&tft, tft.width() / 2 - 40, 100, 70, 40, CYAN, BLUE, YELLOW, "Reset", 2);

    button1.initButtonUL(
        &tft, tft.width() / 2 - 40, tft.height()-30 , //gfx x1 y1
        60, 20,  //w h
        TFT_WHITE, //outline
        TFT_BLUE, //fill
        TFT_WHITE, //textcolor
        (char*)"btnText",  1) //text, size (1 to x)
        ;
    button1.drawButton();

  /*
  // configure LED PWM
  int ledchannel=4;
  uint32_t uRes=0;
  uRes=ledcSetup(ledchannel, 5000, 12);
  Serial.printf("ledcsetup=%i", uRes);
  ledcAttachPin(TFT_BL, ledchannel);
  ledcWrite(TFT_BL, 255);
  */

  tft.setRotation(1);

  // Set text colour to white with black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  tft.fillScreen(TFT_BLACK);            // Clear screen

  //tft.setFreeFont(&FreeMonoBold20pt8b);                 // Select the font
  //tft.setFreeFont(&FreeMonoBold18pt8b);
  tft.setFreeFont(&FreeMonoBold16pt8b);
  
  // Change the text padding to 20 pixels
  tft.setTextPadding(310);


  printMsg("ESP32 MQTT Mon");
  //  tft.setFreeFont(NULL);

    connectWiFi();

    mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    xpt.begin(mySpi);
    xpt.setRotation(1);
    currentScreen=0;
    drawScreen1();
}

unsigned long lastMillis=millis();
bool terasse1_last_state=false;
bool terasse1_curr_state=false;
const int DEBOUNCE_DELAY = 500;
unsigned long lastDebounceTime = 0;

bool toggleSwitch1(){
  //only toggle if time elapsed...
  bool currState=tSwitch.getState();
  Serial.printf("\ntoggleSwitch: state=%i\n", currState);
  if((millis()-lastDebounceTime)> DEBOUNCE_DELAY){
    Serial.printf("\ntoggleSwitch: time OK: %i\n", lastDebounceTime);
    lastDebounceTime=millis();
    bool newState = tSwitch.toggle();
    if(newState){
      SwitchOn();
    }else
    {
      SwitchOff();
    } 
  }
  else{
    Serial.printf("\ntoggleSwitch: time not OK!: %i\n", lastDebounceTime);
  }
  return currState;
}

void loop() {
    mqttClient.loop();
  delay(5);
  //get_LDR(); always 6
  if(xpt.touched()){
    // get position for XPT digitizer and TFT
    xptPosition (&xptX, &xptY, &xptZ, &tftX, &tftY);
    Serial.printf("currenScreen=%i\n", currentScreen);
    if(currentScreen==2){
        if (tSwitch.contains(tftX, tftY)){
          Serial.println("Toggle Switch hit");
          toggleSwitch1();
          
        }
    }
    if(tftY>210)
    {
      if (tftX>TFT_WIDTH/2)
      {
        if (currentScreen==1){
          
          drawScreen2();
        }
      }else{
        if (currentScreen==2){
          drawScreen1();
        }
      }
  }
    Serial.printf("touch at %i / %i with pressure %i\n", tftX, tftY,xptZ); 
  }
}
