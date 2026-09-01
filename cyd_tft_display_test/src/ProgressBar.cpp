// Class that represents a drawable progress bar.
// Written by Ben Dodd (mitgobla)

//ProgressBar bar(display, 5, 5, display.height() - 5, 16);
//bar.setProgressPercentage(getRandomFloat());

#include "ProgressBar.h"

ProgressBar::ProgressBar(TFT_eSPI& display, int x0, int y0, int w, int h, 
    int foregroundColor, int backgroundColor, int borderColor) :
    _display(display), _x0(x0), _y0(y0), _w(w), _h(h), 
        _fgColor(foregroundColor), _bgColor(backgroundColor), _bdColor(borderColor) {}

void ProgressBar::setForegroundColor(int foregroundColor) {
    _fgColor = foregroundColor;
}

void ProgressBar::setBackgroundColor(int backgroundColor) {
    _bgColor = backgroundColor;
}

void ProgressBar::setBorderColor(int borderColor) {
    _bdColor = borderColor;
}

void ProgressBar::draw() {
    // x, y, w, h
//    _display.drawRect(_x0, _y0, _w, _h, _bdColor);
//    _display.fillRect(_x0 + 1, _y0 + 1, _w - 2 - 1, _h - 2, _bgColor);
    _display.drawRoundRect(_x0, _y0, _w, _h, 6, _bdColor);
    _display.fillRoundRect(_x0 + 1, _y0 + 1, _w - 2 - 1, _h - 2, 6, _bgColor);
}

void ProgressBar::setProgressPercentage(float percentage) {
    //draw smaller rect inside 
    // width = w/100 * percentage
//    _display.fillRect(_x0+1, _y0+1, (int_fast32_t)((_w-2)/100*percentage), _h-2, _fgColor);
    _display.fillRoundRect(_x0+1, _y0+1, (int_fast32_t)((_w-2)/100*percentage), _h-2, 4, _fgColor);
}