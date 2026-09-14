#include "meter_ring.h"
#include <ArduinoLog.h>

    meterRing::meterRing(TFT_eSPI * tft, const GFXfont * font1 ){
        _tft=tft;
        _font1=font1;
        initDone=false;
    }
    void meterRing::updateValue(int val){
        if (! initDone)
            return;
        drawRing(val, _vmin, _vmax, _x,_y,_r,_einheit,_colorset);
    }
    // #########################################################################
    //  Draw the meter on the screen, returns x coord of righthand side
    // #########################################################################
    int meterRing::drawRing(int value, int vmin, int vmax, int x, int y, int r, std::string einheit, byte colorset)
    {
        TFT_eSPI *tft=meterRing::_tft;
        _tft->setFreeFont(_font1);
        _vmin=vmin; _vmax=vmax;
        _x=x;_y=y; _r=r;
        _einheit=einheit;
        _colorset=colorset;

        initDone=true;

        // Minimum value of r is about 52 before value text intrudes on ring
        // drawing the text first is an option
        
        int xpos=x; int ypos=y;
        ///x,y = center
        x += r; 
        y += r;   // Calculate coords of centre of ring

        int w = r / 4;    // Width of outer ring is 1/4 of radius
        
        int angle = 150;  // Half the sweep angle of meter (300 degrees)

        int text_colour = 0; // To hold the text colour

        int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v
    //text
        // Convert value to a string
        char buf[10];
        //byte len = 4; if (value > 999) len = 5;
        //dtostrf(value, len, 0, buf);
        snprintf(buf, 10, "%i", value);

        // Set the text colour to default
        tft->setTextColor(TFT_WHITE, TFT_BLACK);
        // Uncomment next line to set the text colour to the last segment value!
        // tft.setTextColor(text_colour, TFT_BLACK);
        
        String text=String(buf);
        uint16_t tWidth=tft->textWidth(text); //ie 54pixel
        uint16_t tHeight=tft->fontHeight();
        uint16_t xTxt, yTxt; // differenz wzischen innen und aussen
        int _w=(r * 2);
        int _h=(r * 2);
        //center string in ring
        xTxt =  (_w-tWidth)/2 + xpos ;
        yTxt = (_h-tHeight)/2 + ypos;
        Log.info("\nRingMeter; Text width %i\nFont Height %i\nxpos=%i, ypos=%i, Ring=%i\n", tWidth, tHeight, xpos, ypos, r);
        /*
        RingMeter; Text width 54
        Font Height 24
        xpos=170, ypos=10, Ring=40
        => width ring = 80
        -> center= (80 - 54) / 2 + 170 = 13 + 170
        */
        tft->drawString(text, xTxt, yTxt);
/*
        // Print value, if the meter is large then use big font 6, othewise use 4
        if (r > 84) 
            tft->drawCentreString(buf, x - 5, y - 20, 6); // Value in middle
        else 
            tft->drawCentreString(buf, x - 5, y - 20, 4); // Value in middle
*/
        // Print units, if the meter is large then use big font 4, othewise use 2
        String unitsS=String(einheit.c_str());
        tWidth=tft->textWidth(unitsS);
        tHeight=tft->fontHeight();
        xTxt = (_w-tWidth)/2 + xpos;
        yTxt = (_h-tHeight)/2 + ypos + _h/2;

        const char *units=einheit.c_str();
        tft->setTextColor(TFT_WHITE, TFT_BLACK);
/*        
        tft->setTextDatum(MC_DATUM);
        if (r > 84) 
            tft->drawString(units, x, y + 30, 4); // Units display
        else 
            tft->drawString(units, x, y + 5, 2); // Units display

        tft->setTextDatum(TL_DATUM);
*/
        tft->drawString(unitsS, xTxt, yTxt);
    
    //graphics
        byte seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
        byte inc = 5; // Draw segments every 5 degrees, increase to 10 for segmented ring

        // Draw colour blocks every inc degrees
        for (int i = -angle; i < angle; i += inc) {

            // Choose colour from scheme
            int colour = 0;
            switch (colorset) {
            case 0: colour = TFT_RED; break; // Fixed colour
            case 1: colour = TFT_GREEN; break; // Fixed colour
            case 2: colour = TFT_BLUE; break; // Fixed colour
            case 3: colour = meterRing::rainbow(map(i, -angle, angle, 0, 127)); break; // Full spectrum blue to red
            case 4: colour = rainbow(map(i, -angle, angle, 63, 127)); break; // Green to red (high temperature etc)
            case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
            default: colour = TFT_BLUE; break; // Fixed colour
            }

            // Calculate pair of coordinates for segment start
            float sx = cos((i - 90) * 0.0174532925);
            float sy = sin((i - 90) * 0.0174532925);
            uint16_t x0 = sx * (r - w) + x;
            uint16_t y0 = sy * (r - w) + y;
            uint16_t x1 = sx * r + x;
            uint16_t y1 = sy * r + y;

            // Calculate pair of coordinates for segment end
            float sx2 = cos((i + seg - 90) * 0.0174532925);
            float sy2 = sin((i + seg - 90) * 0.0174532925);
            int x2 = sx2 * (r - w) + x;
            int y2 = sy2 * (r - w) + y;
            int x3 = sx2 * r + x;
            int y3 = sy2 * r + y;

            if (i < v) { // Fill in coloured segments with 2 triangles
                tft->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
                tft->fillTriangle(x1, y1, x2, y2, x3, y3, colour);
                text_colour = colour; // Save the last colour drawn
            }
            else // Fill in blank segments
            {
                tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_LIGHTGREY);
                tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_LIGHTGREY);
            }
        }
        // Calculate and return right hand side x coordinate
        return (x + r);
    }

    // #########################################################################
    // Return a 16 bit rainbow colour
    // #########################################################################
    unsigned int meterRing::rainbow(byte value)
    {
    // Value is expected to be in range 0-127
    // The value is converted to a spectrum colour from 0 = blue through to 127 = red

    byte red = 0; // Red is the top 5 bits of a 16 bit colour value
    byte green = 0;// Green is the middle 6 bits
    byte blue = 0; // Blue is the bottom 5 bits

    byte quadrant = value / 32;

    if (quadrant == 0) {
        blue = 31;
        green = 2 * (value % 32);
        red = 0;
    }
    if (quadrant == 1) {
        blue = 31 - (value % 32);
        green = 63;
        red = 0;
    }
    if (quadrant == 2) {
        blue = 0;
        green = 63;
        red = value % 32;
    }
    if (quadrant == 3) {
        blue = 0;
        green = 63 - 2 * (value % 32);
        red = 31;
    }
    return (red << 11) + (green << 5) + blue;
    }

