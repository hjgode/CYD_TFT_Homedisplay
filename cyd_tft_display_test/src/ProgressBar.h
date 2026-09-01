// Class that represents a drawable progress bar.
// Written by Ben Dodd (mitgobla)

#ifndef MBED_PROGRESSBAR_H
#define MBED_PROGRESSBAR_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class ProgressBar {
public:

    ProgressBar(TFT_eSPI& display, int x0, int y0, int w, int h, 
        int foregroundColor = TFT_GREEN, int backgroundColor = TFT_BLACK, int borderColor = TFT_WHITE);
    void setForegroundColor(int foregroundColor);
    void setBackgroundColor(int backgroundColor);
    void setBorderColor(int borderColor);

    void draw();
    void setProgressPercentage(float percentage);

private:
    TFT_eSPI& _display;
    int _x0, _y0;
    int _w, _h;
    int _fgColor, _bgColor, _bdColor;

    int _value;
};

#endif