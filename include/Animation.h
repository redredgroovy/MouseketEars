#ifndef Animation_h
#define Animation_h

#include <FastLED.h>
#include "MouseketEars.h"

uint16_t XY(uint8_t x, uint8_t y)
{
	if ( x<0 || x>=VIRTUAL_EAR_COLS || y<0 || y>=VIRTUAL_EAR_ROWS ) {
		return VIRTUAL_EAR_ROWS*VIRTUAL_EAR_COLS; // scratch pixel at end of array
    }
	return x+(y*VIRTUAL_EAR_COLS);
}

class Animation {
    public:
        Animation(uint8_t fadeScale=0) :
            fadeScale(fadeScale)
        { 
        };

        virtual void Setup() = 0;
        virtual void Loop(LedData *data) = 0;

        const uint8_t fadeScale;
};

#endif
