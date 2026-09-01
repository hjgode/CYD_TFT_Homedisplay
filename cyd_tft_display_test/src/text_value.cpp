#include "text_value.h"

text_value::text_value(TFT_eSPI& display, int x, int y, int w, int h, String t, float v, int fgColor, int bgColor, 
        const GFXfont* font1, const GFXfont* font2)
    :_display(display), _x0(x), _y0(y), _w(w), _h(h), 
        _t(t), _value(v), _fgColor(fgColor), _bgColor(bgColor), _font1(font1), _font2(font2) {}


void text_value::drawText(){
    _display.setFreeFont(_font1);
    _display.drawString(_t, _x0, _y0);
    //calc val pos
    uint16_t tWidth=_display.textWidth(_t);
    uint16_t tHeight=_display.fontHeight();

    //draw value
    _display.setFreeFont(_font2);
    char valtxt[12];
    snprintf(valtxt, 12,  "%0.1f", _value);
    _display.drawString(String(valtxt), _x0+_w, _y0);
}