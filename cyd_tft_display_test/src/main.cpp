#include "driver/ledc.h"

#include <ArduinoOTA.h>

#include <XPT2046_Touchscreen.h>
#include "SPI.h"
#include "TFT_eSPI.h"
//#include <TFT_eWidget.h>
//#include "Extensions/Button.h"

#include <WiFi.h>
#include <PubSubClient.h>

// load custom 8bit Fonts created with fontconvert
//#include "FreeMonoBold20pt8b.h"
//#include "FreeMonoBold18pt8b.h"
#include "FreeMonoBold8pt8b.h"
#include "FreeMonoBold10pt8b.h"
#include "FreeMonoBold12pt8b.h"
#include "FreeMonoBold14pt8b.h"
#include "FreeMono14pt8b.h"
#include "FreeMonoBold16pt8b.h"

//some graphics
// see https://palsayantan.github.io/Image-to-RGB565/
// converted from svg using inkscape, save as 30x30 pixel png and uploaded, use 32bit RGBA
#include "window30green.h"
#include "window30red.h"
String FensterSchlafzimmer="Schlafz. ist ";
String FensterBad="Bad ist ";

std::string benzintexte[6];
std::string benzintopics[6]={"benzin0_1","benzin0_2","benzin0_3","benzin1_1","benzin1_2","benzin1_3"};

std::string stromtexte[3];
std::string stromtopics[3]={"Power","DayAktuell","DayLast"};

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen xpt(XPT2046_CS, XPT2046_IRQ);

uint16_t xptX, xptY, tftX, tftY; 
uint8_t xptZ;

#include "zigbee_thermo.h"
#include "ProgressBar.h"

//#include "ringmeter.h"
#include "toggle_switch.h"
toggle_switch tSwitch;
toggle_switch tSwitch2;
#include "mybutton.h"
myButton button1;
myButton button2;
myButton button3;
myButton button4;
#include "text_value.h"

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

//TODO: do we need thes 7bit Fonts or just use our custom 8bit fonts only?
//see also platformio.ini -D 
//the predefined loaded 7bit fonts do not use much flash memory
// see /home/hgode/git/cyd_platformio/cyd_tft_display_test/.pio/libdeps/esp32-2432S028Rv3/TFT_eSPI/Fonts/GFXFF/gfxfont.h
// disabled 7bit font loading by undefined LOAD_ADAFRUIT_GFX_7BIT
#ifndef LOAD_GFXFF
#define LOAD_GFXFF
#endif

#ifdef LOAD_GFXFF // Only include the fonts if LOAD_GFXFF is defined in User_Setup.h

    // Use these when printing or drawing text in GLCD and high rendering speed fonts
    #define GFXFF 1
/*
    #define GLCD  0
    #define FONT2 2
    #define FONT4 4
    #define FONT6 6
    #define FONT7 7
    #define FONT8 8
    #define FF7 &FreeMonoBold18pt7b
    #define FF1 &FreeMonoBold14pt8b
    #define sFF7 "Mono bold 18"
*/    
    #define FF08b &FreeMonoBold8pt8b
    #define FF10b &FreeMonoBold10pt8b
    #define FF12b &FreeMonoBold12pt8b
    #define FF14b &FreeMonoBold14pt8b
    #define FF14n &FreeMono14pt8b
    #define FF16b &FreeMonoBold16pt8b

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
  tft.setFreeFont(FF16b); // &FreeMonoBold16pt8b);
}
void setFontMedium(){
  tft.setFreeFont(&FreeMonoBold10pt8b);
}
void setFont12(){
  tft.setFreeFont(&FreeMonoBold12pt8b);
}
void setFontBold14(){
  tft.setFreeFont(&FreeMonoBold14pt8b);
}
void setFont14(){
  tft.setFreeFont(&FreeMono14pt8b);
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

void drawFooter(){
  setFontSmall();
  int i=7;
  tft.drawRect(10, 10+i*30, 240-10, 30,TFT_BLACK); //clear background
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
  drawFooter();
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
  if(FensterSchlafzimmer.endsWith("ZU"))
    tft.pushImage(270,iLine*iLineSpace+iOffsetY,30,30,window30green); //draw 16Bit Image from Progmem
  else
    tft.pushImage(270,iLine*iLineSpace+iOffsetY,30,30,window30red); //draw 16Bit Image from Progmem

iLine++;
  tft.drawString(FensterBad, 10, iLine*iLineSpace+iOffsetY);
  if (FensterBad.endsWith("ZU"))
    tft.pushImage(270,iLine*iLineSpace+iOffsetY,30,30,window30green); //draw 16Bit Image from Progmem
  else
    tft.pushImage(270,iLine*iLineSpace+iOffsetY,30,30,window30red); //draw 16Bit Image from Progmem

  //tft.pushImage(x,y,w,h,data) //draw 16Bit Image from Progmem
  drawFooter();
  currentScreen=1;
}

void drawScreen2(){
  if (currentScreen==2)
    return;
  tft.fillScreen(TFT_BLACK);
  setFontBold14();
  tft.setTextColor(TFT_WHITE);
  int iLine=0, iLineSpace=20, iOffsetY=12;

  for (int z=0; z<6; z++){
    //for z=0 or 3 use yellow bold, else use white normal
    if(z==0 || z==3){
      setFontBold14();
      tft.setTextColor(TFT_YELLOW);
    }
    else{
      setFont14();
      tft.setTextColor(TFT_WHITE);
    }
    tft.drawString(benzintexte[z].c_str(), 10, iLine*iLineSpace+iOffsetY);
    iLine++;
    //add one more line between block
    if(iLine==1 || iLine==4 || iLine==6){
      iLine++;
    }
  }
  tft.setTextColor(TFT_WHITE);
  setFontNormal();
  drawFooter();
  currentScreen=2;
}

void updateStromLine(String newStr, int line){
    int iLine=0, iLineSpace=30, iOffsetY=12;
    iLine=line;
    setFontNormal();
    tft.setTextColor(TFT_YELLOW);
    tft.setTextPadding(150);
    tft.drawString(newStr, 170, iLine*iLineSpace+iOffsetY);
    tft.setTextColor(TFT_WHITE);
    drawFooter();
}

//STROM
void drawScreen3(){
  if (currentScreen==3)
    return;
  tft.fillScreen(TFT_BLACK);
  setFontNormal();
  tft.setTextColor(TFT_WHITE);
  int iLine=0, iLineSpace=30, iOffsetY=12;

  tft.drawString("Aktuell: ", 10, iLine*iLineSpace+iOffsetY);
  iLine++;
  tft.drawString("Heute: ", 10, iLine*iLineSpace+iOffsetY);
  iLine++;
  tft.drawString("Gestern: ", 10, iLine*iLineSpace+iOffsetY);

//   tft.drawRightString("Hello, World!", 240, 20, 2); // Draw string aligned to the right at x=240, y=20 with font type 2
  iLine=0;
  tft.setTextColor(TFT_YELLOW);
  for (int z=0; z<3; z++){
//      tft.drawRightString((char*)stromtexte[z].c_str(),150,iLine*iLineSpace+iOffsetY);
      tft.drawString(stromtexte[z].c_str(), 170, iLine*iLineSpace+iOffsetY);
    iLine++;
  }
  tft.setTextColor(TFT_WHITE);
  drawFooter();
  currentScreen=3;
}

void drawScreen4(){
  //GFXfont *f;
  if (currentScreen==4)
    return;
  tft.fillScreen(TFT_BLACK);
  setFont12();
  tft.setTextColor(TFT_WHITE);
  uint16_t buttonH=28;
  button1.initButton(&tft, 12, 12, 120, 24, 0xC5F7, 0x1B9B, TFT_WHITE);
  button1.drawButton("Hell 1");

  button2.initButton(&tft, 12, 12+buttonH*1+2, 120, 24, 0xC5F7, 0x1B9B, TFT_WHITE);
  button2.drawButton("Hell 2");

  button3.initButton(&tft, 12, 12+buttonH*2+2, 120, 24, 0xC5F7, 0x1B9B, TFT_WHITE);
  button3.drawButton("Hell 3");
  button4.initButton(&tft, 12, 12+buttonH*3+2, 120, 24, 0xC5F7, 0x1B9B, TFT_WHITE);
  button4.drawButton("Hell 4");

  // tft, x1, y1 , x2, y2
  ProgressBar bar(tft, 12, 12+buttonH*4+2, 120, 24);
  bar.draw();
  bar.setProgressPercentage(50.0);

  text_value tValue(tft, 12, 180, 80, 30, "TEST", 35.0, TFT_BLACK, TFT_WHITE, FF14n, FF14b);
  tValue.drawText();

  drawFooter();
  currentScreen=4;
  setFontNormal();
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print ("Message arrived on Topic:");
    Serial.println (topic);

    // Convert payload to string    
    String payloadStr = String((char*)payload);
    String msgStr;

    //Benzin
    bool benzinupdate=false;
    std::string cTopic=topic;
    char chPayload[length+1]; for (int a=0; a<length+1; a++){chPayload[a]='\0';}
    strncpy(chPayload, (char*)payload, length);
    std::string cPayload(chPayload);
    
    for (int y=0;y<6;y++){
      int pos = cTopic.find(benzintopics[y]);
      if(pos > -1){
        benzinupdate=true;
        benzintexte[y]=cPayload;
      }
    }
    if(currentScreen==2 && benzinupdate){
        //force draw
        currentScreen=1;
      drawScreen2();
      return;
    }

    //Strom
    bool stromupdate=false;
    if(cTopic.find("Strom")){
      for (int y=0;y<3;y++){
        int pos = cTopic.find(stromtopics[y]);
        if(pos > -1){
          stromupdate=true;
          std::string padded = utils::padLeft(cPayload, 7, ' ');
          stromtexte[y]=padded;// cPayload;
//          if(currentScreen==3){
//            updateStromLine(padded.c_str(), y);
//          }
        }
      }
      if(currentScreen==3 && stromupdate){
        //force draw
        currentScreen=2;
        drawScreen3();
        return;
      }
    }

    for(int i=0;i<length;i++){
        msgStr += (char)payload[i];
    }
    Serial1.print(msgStr);
    Serial1.println();

    //assign text lines?
    String topicS="";
    topicS+=topic;

    //zigbee
    if (topicS.indexOf("zigbee")>0){
      //process zigbee comment, temperature, humidity
      //ie mqttGenericBridge/zigbee/zigbee_0xa4c1389ca963dfc0/humidity temperature, comment
      zigbeeType zType = zigbee_thermo::getType(topicS.c_str(), msgStr.c_str());
      
      string n=zigbee_thermo::getName(cTopic);
      Serial.printf("\nZigbee mesg '%s' received, type=%i, name='%s'\n", msgStr.c_str(), zType, n.c_str());
      zigbee_thermo zb= zb.getItem(n); //zigbee_thermo(n);
      
      switch (zType){
        case temperature:
          zb.setTemp(atof(msgStr.c_str()));
          break;
        case humidity:
          zb.setHumi(atoi(msgStr.c_str()));
          break;
        case comment:
          zb.setText(msgStr.c_str());
          break;
      }
      zb.putItem(n, zb);
      Serial.printf("\nZigbee=%s\n", zb.dumpItem().c_str());
    }

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
//    topicS.toInt();
  if(topicS.indexOf("text")>0){
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
  }
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
//    mqttClient.subscribe("mqttGenericBridge/BenzinPreise/#");
    // #zigbee devices
    // mqttGenericBridge/zigbee/zigbee_0xa4c1386fbb4a56db temperature, humidity, comment
    mqttClient.subscribe("mqttGenericBridge/zigbee/#");
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

uint16_t _brightness=50;

void setup(void) {
  // Start Preferences
  _brightness=utils::loadPrefs();
  utils::set_BL(_brightness);

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
  
  // Change the text padding to 320 pixels
  tft.setTextPadding(320);


  printMsg("ESP32 MQTT Mon");
  //  tft.setFreeFont(NULL);

  connectWiFi();

  ArduinoOTA.begin();

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
  drawScreen2,
  drawScreen3,
  drawScreen4
};
const int MAX_SCREEN=5;

int cnt=0;

void loop() {
  if(WiFi.status()!=WL_CONNECTED){
    mqttClient.disconnect();
    connectWiFi();
  }
  ArduinoOTA.handle();
  mqttClient.loop();
  delay(5);
  cnt+=5;
  if(cnt > 500){
    cnt=0;
//    std::string s=utils::printHeap();
//    Serial.print(s.c_str());
  }
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
    if(currentScreen==4){
      if(button1.contains(tftX,tftY)){
        Serial.println("Button1 hit");
        utils::set_BL(25);
      }
      if(button2.contains(tftX,tftY)){
        Serial.println("Button2 hit");
        utils::set_BL(50);
      }
      if(button3.contains(tftX,tftY)){
        Serial.println("Button2 hit");
        utils::set_BL(75);
      }
      if(button4.contains(tftX,tftY)){
        Serial.println("Button2 hit");
        utils::set_BL(100);
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
        }else{  // go left
          i--;
          if (i < 0)
            i=0;
        }
        Serial.printf("\nscreen switch to %i\n",i);
        (*draw_screen[i])();  // screens 1 to x, i is 0 to x-1
      }
  }
  Serial.printf("touch at %i / %i with pressure %i\n", tftX, tftY,xptZ); 
  }
}
