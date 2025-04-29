#pragma once

#include "Animation.h"
#include <RunningMedian.h>

extern RunningMedian gVoltageSamples;
extern uint8_t currentChargePct(float voltage);

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
            data->leftLeds->fill_solid(CRGB::Black);
            data->rightLeds->fill_solid(CRGB::Black);
            data->leftRings[0]->fill_gradient(CHSV(0,255,64), CHSV(100,255,64));
            //EVERY_N_SECONDS(2) {
                uint8_t scale = (uint8_t)(currentChargePct(gVoltageSamples.getMedian()) / 100.0 * 35.0); //data->leftRings[0]->size());
                //(*data->leftLeds)[31] = CRGB(0,64,0);
                (*data->leftLeds)(scale,34) = CRGB::Black;
            //}
        }

};