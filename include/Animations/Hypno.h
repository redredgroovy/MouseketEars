#include "Animation.h"

class Hypno : public Animation
{
    public:
        
        Hypno() : Animation()
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
        }

    protected:
        const uint8_t bpm = 20;

};