//text_value.h
#ifndef TEXT_VALUE_H
#define TEXT_VALUE_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class text_value{

    public:
        text_value(TFT_eSPI& display, int x, int y, int w, int h, String t, float v, int fgColor, int bgColor, 
            const GFXfont* font1, const GFXfont* font2);
        void drawText();

    private:
        TFT_eSPI& _display;
        int _x0, _y0;
        int _w, _h;
        int _fgColor, _bgColor;

        String _t;
        float _value;
        const GFXfont *_font1;
        const GFXfont *_font2;
};

#endif