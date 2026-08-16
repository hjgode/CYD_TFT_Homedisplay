#include "toggle_switch.h"

toggle_switch::toggle_switch(void){
  currstate = false;
  
}

bool toggle_switch::getState(){
    return currstate;
}
void toggle_switch::setState(bool state){
    currstate=state;
}
bool toggle_switch::toggle(){
    currstate=!currstate;
    drawButton(currstate);
    return currstate;
}

void toggle_switch::initSwitch(TFT_eSPI *gfx, int16_t x, int16_t y, 
                      uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t forecolor)
{

  _x           = x;
  _y           = y;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _fillcolor    = fill;
  _forecolor    = forecolor;
  _gfx          = gfx;
}

void toggle_switch::drawButton(){
    /*
    //x, y, w, h, radius, color
    tft.fillRoundRect(216, 12, 50, 20, 10, TFT_WHITE);
    tft.drawRoundRect(216, 12, 50, 20, 10, TFT_WHITE);
    tft.fillCircle(226, 22, 8, TFT_SKYBLUE);
    */
   int r=_h/2;
  //x, y, w, h, radius, color
  _gfx->fillRoundRect(_x, _y, _w, _h, r, _fillcolor);
  _gfx->drawRoundRect( _x, _y, _w, _h, r, _outlinecolor);
  _gfx->fillCircle(_x+10, _y+10, 8, _forecolor);

}

void toggle_switch::drawButton(bool OnOff){
    /*
    //x, y, w, h, radius, color
    tft.fillRoundRect(216, 12, 50, 20, 10, TFT_WHITE);
    tft.drawRoundRect(216, 12, 50, 20, 10, TFT_WHITE);
    tft.fillCircle(226, 22, 8, TFT_SKYBLUE);
    */
   int r=_h/2;
  //x, y, w, h, radius, color
  _gfx->fillRoundRect(_x, _y, _w, _h, r, _fillcolor);
  _gfx->drawRoundRect( _x, _y, _w, _h, r, _outlinecolor);
  int offset=0;
  if (OnOff)
    _gfx->fillCircle(_w+_x-r, _y+r, r-2, _forecolor);
   else
    _gfx->fillCircle(_x+r, _y+r, r-2, _forecolor);

}

bool toggle_switch::isClicked(){
    uint16_t x, y;
//    _gfx->getTouch(x,y);
    return ((x >= _x) && (x < (_x + _w)) &&
          (y >= _y) && (y < (_y + _h)));
}

bool toggle_switch::contains(int16_t x, int16_t y) {
    Serial.printf("Contains: x=%i, _x=%i, x2=%i, y=%i, y2=%i\n", x, _x, _x+_w, y, _y+_h);
  uint16_t xTest=(x >= _x) && (x < (_x + _w));
  uint16_t yTest=(y >= _y) && (y < (_y + _h));
  Serial.printf("xTest=%i, yTest=%i\n", xTest, yTest);
  bool bcontains=false;
  if (xTest>0 && yTest>0)
    bcontains=true;
  else
    bcontains=false;
//  bool bcontains = ((x >= _x) && (x < (_x + _w)) && (y >= _y) && (y < (_y + _h)));
  return bcontains;
}