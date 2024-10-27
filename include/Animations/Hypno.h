#include "Animation.h"

#ifndef Hypno_h
#define Hypno_h

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
            uint8_t thisHue = beat8(bpm, 255);
            
            for( int ring=0; ring < 4; ring++ ) {
                fill_rainbow_circular(data->leftRings[ring]->leds, data->leftRings[ring]->size(), thisHue);
                fill_rainbow_circular(data->rightRings[ring]->leds, data->rightRings[ring]->size(), thisHue, true);
            }

			if (this->fadeScale > 0) {
				data->leftLeds->fadeLightBy(this->fadeScale);
				data->rightLeds->fadeLightBy(this->fadeScale);
			}
        }

    protected:
        const uint8_t bpm = 20;

};

#endif