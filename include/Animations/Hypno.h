#pragma once

#include "Animation.h"

class Hypno : public Animation
{
    public:
        
        Hypno() : Animation()
        {
        }

        Hypno(uint8_t fadeScale) : Animation(fadeScale)
        {
        }

        void Setup()
        {
        }

        void Loop(LedData *data)
        {
            uint8_t thisHue = beat8(mBpm, 255);
            
            for (int ring=0; ring < 4; ring++ ) {
                fill_rainbow_circular(data->leftRings[ring]->leds, data->leftRings[ring]->size(), thisHue);
                fill_rainbow_circular(data->rightRings[ring]->leds, data->rightRings[ring]->size(), thisHue, true);
            }

			if (mFadeScale > 0) {
				data->leftLeds->fadeLightBy(mFadeScale);
				data->rightLeds->fadeLightBy(mFadeScale);
			}
        }

    protected:
        const uint8_t mBpm = 20;

};