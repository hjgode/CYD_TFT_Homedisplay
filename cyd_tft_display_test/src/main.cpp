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

#include "window30green.h"
#include "window30red.h"
String FensterSchlafzimmer="Schlafz. ist ";
String FensterBad="Bad ist ";

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen xpt(XPT2046_CS, XPT2046_IRQ);

uint16_t xptX, xptY, tftX, tftY; 
uint8_t xptZ;

//#include "ringmeter.h"
#include "toggle_switch.h"
toggle_switch tSwitch;
toggle_switch tSwitch2;
// Licht Terasse1
bool stateLichtTerasse1 = false;
// Licht Terasse2
bool stateLichtTerasse2 = false;

#include "utils.h"

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
int currentScreen=0;

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

void drawScreen0(){
//  if (currentScreen==0)
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
  currentScreen=0;
}

void drawScreen1(){
  int iLine=0, iLineSpace=40, iOffsetY=12;
  if (currentScreen==1)
    return;
  tft.fillScreen(TFT_BLACK);
  setFontNormal();
  tft.setTextColor(TFT_WHITE);

//  tft.setCursor(10, 30);
//  tft.print("Terrasse 1");
  tft.drawString("Terasse 1", 10, iLine*iLineSpace+iOffsetY);
//  toggle_switch tSwitch;
  tSwitch.initSwitch(&tft, 260, iLine*iLineSpace+iOffsetY, 50, 20, TFT_WHITE, TFT_WHITE, TFT_BLUE);
  tSwitch.setFHEMdevice("shelly1_Terasse");
  tSwitch.drawButton(stateLichtTerasse1);

  iLine++;
//  tft.setCursor(10, 30+30);
//  tft.print("Terrasse 2");
  tft.drawString("Terasse 2", 10, iLine*iLineSpace+iOffsetY);
  tSwitch2.initSwitch(&tft, 260, iLine*iLineSpace+iOffsetY, 50, 20, TFT_WHITE, TFT_WHITE, TFT_BLUE);
  tSwitch2.setFHEMdevice("shellyrgbw2_terasse2");
  tSwitch2.drawButton(stateLichtTerasse2);

iLine++;
//  setFontSmall();
  tft.drawString(FensterSchlafzimmer, 10, iLine*iLineSpace+iOffsetY);
  tft.pushImage(270,iLine*iLineSpace+iOffsetY,30,30,window30red); //draw 16Bit Image from Progmem

iLine++;
  tft.drawString(FensterBad, 10, iLine*iLineSpace+iOffsetY);
  tft.pushImage(270,iLine*iLineSpace+iOffsetY,30,30,window30green); //draw 16Bit Image from Progmem

  //tft.pushImage(x,y,w,h,data) //draw 16Bit Image from Progmem
  currentScreen=1;
}

void drawScreen2(){
  if (currentScreen==2)
    return;
  tft.fillScreen(TFT_BLACK);
  setFontNormal();
  tft.setTextColor(TFT_WHITE);

  tft.drawString(FensterSchlafzimmer, 10, 90-20);
  tft.drawString(FensterSchlafzimmer, 10, 120-20);

  tft.pushImage(180,90-20,30,30,window30red); //draw 16Bit Image from Progmem
  tft.pushImage(180,120-20,30,30,window30green); //draw 16Bit Image from Progmem
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

    // shellies/shellyrgbw2_E4CB31/color/0 Terasse2 RGBW
    if (topicS.indexOf("shellyrgbw2_E4CB31")>0){
      Serial.printf("\nlichtTerasse2 %s\n", msgStr.c_str());
      if (msgStr.endsWith("off")){
        Serial.println("lichtTerasse2 ist OFF");
        stateLichtTerasse1=false;
      }else{
        Serial.println("lichtTerasse1 ist ON");
        stateLichtTerasse1=true;
      }
      try
      {
        /* code */
        tSwitch2.setState(stateLichtTerasse1);
      }
      catch(const std::exception& e)
      {
        Serial.println(e.what());
      }
      
      return;
    }

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

    //fenster schlafzimmer: mqttGenericBridge/HM_5F5A68/state
    //fenster Bad: mqttGenericBridge/HM_58AD5B/state
    if (topicS.indexOf("HM_5F5A68") > 0){
      if(msgStr.endsWith("open")){
        //Fenster ist offen
        FensterSchlafzimmer="Schlafz. AUF";
      }
      else{
        //Fenster ist zu
        FensterSchlafzimmer="Schlafz. ZU";
      }
      return;
    }

    //fenster Bad: mqttGenericBridge/HM_58AD5B/state
    if (topicS.indexOf("HM_58AD5B") > 0){
      if(msgStr.endsWith("open")){
        //Fenster ist offen
        FensterBad="Bad AUF";
      }
      else{
        //Fenster ist zu
        FensterBad="Bad ZU";
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

    if(currentScreen==0){
      //drawScreen0();
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
    mqttClient.subscribe("shellies/shelly1-ABF975/relay/0"); ////licht_terasse1
    mqttClient.subscribe("shellies/shellyrgbw2_E4CB31/color/0"); //shellyrgbw2_terasse2
    //fenster schlafzimmer
    mqttClient.subscribe("mqttGenericBridge/HM_5F5A68/state");
    //fenster bad mqttGenericBridge/HM_58AD5B
    mqttClient.subscribe("mqttGenericBridge/HM_58AD5B/state");
    //shellyrgbw2_E4CB31/color/0 [on|off]
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

void fhemSwitchOnOff(String sDevice, bool OnOff){
  //String msg="set shelly1_Terasse on";
  String sOnOff=OnOff?" on":" off";
  String msg="set " + sDevice + sOnOff;
  Serial.print(msg);
  mqttSendFHEMcmnd(msg);
}
    
int get_LDR(){
  pinMode(LDR_PIN, INPUT);
  analogSetAttenuation(ADC_0db);
  int value=analogRead(LDR_PIN);
  Serial.printf("\nLDR is %i\n");
  return value;
}

void setup(void) {
  Serial.begin (115200);

  tft.begin();
  
  utils::set_BL(50);

  utils::led(0,100,0,false);

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
    drawScreen0();
}

unsigned long lastMillis=millis();
//UNUSED bool terasse1_last_state=false;
//UNUSED bool terasse1_curr_state=false;
const int DEBOUNCE_DELAY = 500;
unsigned long lastDebounceTime = 0;

bool toggleSwitch(toggle_switch* tsw){
  if (tsw == nullptr)
    return false;
  //only toggle if time elapsed...
  bool currState=tsw->getState();
  Serial.printf("\ntoggleSwitch: state=%i\n", currState);
  if((millis()-lastDebounceTime)> DEBOUNCE_DELAY){
    Serial.printf("\ntoggleSwitch: time OK: %i\n", lastDebounceTime);
    lastDebounceTime=millis();
    bool newState = tsw->toggle();
    if(newState){
      fhemSwitchOnOff(tsw->getFHEMdevice(), true);
    }else
    {
      fhemSwitchOnOff(tsw->getFHEMdevice(), false);
    } 
  }
  else{
    Serial.printf("\ntoggleSwitch: time not OK!: %i\n", lastDebounceTime);
  }
  return currState;

}

//an array of drawScreen functions
void (*draw_screen[])(void) = {
  drawScreen0,
  drawScreen1,
  drawScreen2
};
const int MAX_SCREEN=3;

void loop() {
    mqttClient.loop();
  delay(5);
  //get_LDR(); always 6
  if(xpt.touched()){
    // get position for XPT digitizer and TFT
    xptPosition (&xptX, &xptY, &xptZ, &tftX, &tftY);
    Serial.printf("currenScreen=%i\n", currentScreen);
    if(currentScreen==1){
        if (tSwitch.contains(tftX, tftY)){
          Serial.println("Toggle Switch hit");
          toggleSwitch(&tSwitch);          
        }
        if (tSwitch2.contains(tftX, tftY)){
          Serial.println("Toggle Switch2 hit");
          toggleSwitch(&tSwitch2);          
        }
    }
    if(tftY>210)
    {
      if((millis()-lastDebounceTime)> DEBOUNCE_DELAY){
        lastDebounceTime=millis();

        int i=currentScreen; // is 1 or 2
        if (tftX>TFT_WIDTH/2) //go right
        {
          i++;
          if(i > MAX_SCREEN-1){  // number of elements
            i--;
          }
          /*
          if (currentScreen==1){          
            drawScreen1();
          }
          */
        }else{  // go left
          i--;
          if (i < 0)
            i=0;
          /*
          if (currentScreen==2){
            drawScreen0();
          }
          */
        }
        Serial.printf("\nscreen switch to %i\n",i);
        (*draw_screen[i])();  // screens 1 to x, i is 0 to x-1
      }
  }
    Serial.printf("touch at %i / %i with pressure %i\n", tftX, tftY,xptZ); 
  }
}
