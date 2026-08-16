#include <TFT_eSPI.h>

class toggle_switch{
  public:
    toggle_switch(void);
    // "Classic" initButton() uses centre & size
    void     initSwitch(TFT_eSPI *gfx, int16_t x, int16_t y, 
                      uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t forecolor);
    bool     contains(int16_t x, int16_t y);
    void     drawButton();
    void     drawButton(bool OnOff);
    bool toggle();
    void setState(bool);
    bool getState();
    bool isClicked();
    
 private:
  TFT_eSPI *_gfx;
  bool _state;
  int16_t  _x, _y; // Coordinates of top-left corner of button
  uint16_t _w, _h;   // Width and height of button
  uint16_t _outlinecolor, _fillcolor, _forecolor;
  bool  currstate; // Button states
};

