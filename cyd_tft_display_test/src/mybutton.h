//mybutton.h
#include <Arduino.h>
#include <TFT_eSPI.h>

class myButton{
    private:
        TFT_eSPI *_gfx;
        bool _state;
        int16_t  _x, _y; // Coordinates of top-left corner of button
        uint16_t _w, _h;   // Width and height of button
        uint16_t _outlinecolor, _fillcolor, _forecolor;
        bool  currstate; // Button states
        
    public:
        void initButton(TFT_eSPI *gfx, int16_t x, int16_t y, 
                      uint16_t w, uint16_t h, 
                      uint16_t outline, 
                      uint16_t fill, 
                      uint16_t forecolor);
        void drawButton(String);
        bool     contains(int16_t x, int16_t y);
};