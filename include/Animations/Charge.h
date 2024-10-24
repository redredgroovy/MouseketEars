#include "Animation.h"

class Charge : public Animation
{
    public:
        
        Charge() : Animation()
        {
        }

        void Setup()
        {
            this->spawnTimer = millis();
            this->fadeTimer = millis();
        }

        void Loop(LedData *data)
        {
            unsigned long now = millis();

            if ( now - this->spawnTimer > 5000 ) {
                this->spawnTimer = now;
		        (*data->leftLeds)[0] = CRGB::Green;
            }

            if ( now - this->fadeTimer > 20 ) {
                this->fadeTimer = now;
                data->leftLeds->fadeToBlackBy(1);
                data->rightLeds->fadeToBlackBy(1);
            }
         }

    protected:
        const uint8_t bpm = 10;
        unsigned long spawnTimer = 0;
        unsigned long fadeTimer = 0;

};