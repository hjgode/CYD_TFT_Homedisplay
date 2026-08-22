//mybutton.cpp
#include "mybutton.h"

// example
// initButton(tft, 28, 36,80, 28)
void myButton::initButton(TFT_eSPI *tft, int16_t x, int16_t y, 
                      uint16_t w, uint16_t h, 
                      uint16_t outline = 0xC5F7, 
                      uint16_t fill = 0x1B9B, 
                      uint16_t forecolor = 0xFFFF){
        _gfx=tft;
        _x=x;
        _y=y;
        _w=w;
        _h=h;
        _outlinecolor=outline;
        _fillcolor=fill;
        _forecolor=forecolor;

}

bool myButton::contains(int16_t x, int16_t y){
  uint16_t xTest=(x >= _x) && (x < (_x + _w));
  uint16_t yTest=(y >= _y) && (y < (_y + _h));
  bool bcontains=false;
  if (xTest>0 && yTest>0)
    bcontains=true;
  else
    bcontains=false;
  return bcontains;

}

void myButton::drawButton(String text){
  //_gfx->setFreeFont(NULL);
  //_gfx->setTextFont(1);
  _gfx->fillRoundRect(_x, _y, _w, _h, 6, _fillcolor);
  _gfx->drawRoundRect(_x, _y, _w, _h, 6, _outlinecolor);
  _gfx->setTextColor(_forecolor);
  //_gfx->setTextSize(2);
  //_gfx->setTextDatum(MC_DATUM); //middle center
  //_gfx->drawString(text, 68, 50); //68 = _w/2+_x ; 50 = _y+_h/2
  
  uint16_t tWidth=_gfx->textWidth(text);
  uint16_t tHeight=_gfx->fontHeight();
  uint16_t xTxt, yTxt; // differenz wzischen innen und aussen
  xTxt = (_w-tWidth)/2 + _x;
  yTxt = (_h-tHeight)/2 + _y;
  Serial.printf("\nText width %i\nFont Height %i\n", tWidth, tHeight);
  _gfx->drawString(text, xTxt, yTxt);
  
}