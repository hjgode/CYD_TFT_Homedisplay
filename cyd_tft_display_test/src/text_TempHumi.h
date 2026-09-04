//text_TempHumi.h
#ifndef TEXT_TEMP_HUMI_H
#define TEXT_TEMP_HUMI_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class text_TempHumi{

    public:
        text_TempHumi(TFT_eSPI& display, int x, int y, int w, int h, String t, float v, int humi, 
        int fgColor, int bgColor, 
            const GFXfont* font1, const GFXfont* font2);
        void drawText();

    private:
        TFT_eSPI& _display;
        int _x0, _y0;
        int _w, _h;
        int _fgColor, _bgColor;

        String _t;
        float _temp;
        int _humi;
        const GFXfont *_font1;
        const GFXfont *_font2;
};

#endif