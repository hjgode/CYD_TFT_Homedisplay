#include "text_TempHumi.h"

text_TempHumi::text_TempHumi(TFT_eSPI& display, int x, int y, int w, int h, String t, float temp, 
    int humi, int fgColor, int bgColor, 
        const GFXfont* font1, const GFXfont* font2)
    :_display(display), _x0(x), _y0(y), _w(w), _h(h), 
        _t(t), _temp(temp), _humi(humi), _fgColor(fgColor), _bgColor(bgColor), _font1(font1), _font2(font2) {}


void text_TempHumi::drawText(){
    _display.setFreeFont(_font1);
    _display.drawString(_t, _x0, _y0);
    //calc val pos
    uint16_t tWidth=_display.textWidth(_t);
    uint16_t tHeight=_display.fontHeight();

    //draw value
    _display.setFreeFont(_font2);
    char valtxt[12];
    snprintf(valtxt, 12,  "%0.1f", _temp);
    int iLen = strlen(valtxt);
    // uint16_t: the width of a string in pixels.
    uint16_t sumX = _display.drawString(String(valtxt), _x0+_w, _y0);
    Serial.printf("\nFirst sumX for Temp str= %i\n", sumX);
    int charWidth=sumX/iLen;
    snprintf(valtxt, 12,  "%i", _humi);
    //tWidth=_display.textWidth(valtxt); //does not work with GFXFF
    //tWidth=80;
    // x= (charWidth +1) x iLen  //one char space in between
    sumX =_display.drawString(String(valtxt), _x0 + _w +  (iLen+1)*charWidth, _y0);
    Serial.printf("Second sumX for Temp str= %i\n", sumX);
//First sumX for Temp str= 64 "35.0"    =>  4 char = 64, 1 char=16
//Second sumX for Temp str= 32 "55" =>  2 char = 32
}