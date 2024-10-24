#ifndef Animation_h
#define Animation_h

#include <FastLED.h>
#include "MouseketEars.h"

class Animation {
    public:
        Animation() {};
        virtual void Setup() = 0;
        virtual void Loop(LedData *data) = 0;

};

#endif
