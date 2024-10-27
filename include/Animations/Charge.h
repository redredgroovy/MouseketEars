#include "Animation.h"

#ifndef Charge_h
#define Charge_h

class Charge : public Animation
{
    public:
        
        Charge() : Animation()
        {
        }
         
        void Setup()
        {
        }

        void Loop(LedData *data)
        {
            data->leftLeds->fadeToBlackBy(1);
            data->rightLeds->fadeToBlackBy(1);
            EVERY_N_SECONDS(2) {
                (*data->leftLeds)[31] = CRGB(0,64,0);
            }
        }

};

#endif