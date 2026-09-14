//meter_ring

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <stdlib.h>

// Meter colour schemes
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5

//     ringMeter(reading, 0, 100, xpos, ypos, radius, "%RH", BLUE2BLUE); // Draw analogue meter

class meterRing{
    public:
        meterRing(TFT_eSPI *tft, const GFXfont * font1);
        //{_tft=tft; _font1=font1;}
        int drawRing(int value, int vmin, int vmax, int x, int y, int r, std::string einheit, byte colorset);
        void updateValue(int val);
    private:
        bool initDone;
        int _vmin, _vmax, _x, _y, _r;
        std::string _einheit;
        byte _colorset;
        TFT_eSPI *_tft;
        unsigned int rainbow(byte value);
        const GFXfont * _font1;
};
