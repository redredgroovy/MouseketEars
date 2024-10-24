#include "Animation.h"

class Sparkle : public Animation
{
    public:
        
        Sparkle(const bool rainbow = false) :
            Animation(),
            rainbow(rainbow)
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

            if ( now - this->spawnTimer > 20 ) {
                this->spawnTimer = now;
		        (*data->leftLeds)[random(0, data->leftLeds->size())] = ( this->rainbow ? CHSV(random(0,255), 255, 255) : CHSV(0,0,255) );
		        (*data->rightLeds)[random(0, data->rightLeds->size())] = ( this->rainbow ? CHSV(random(0,255), 255, 255) : CHSV(0,0,255) );
            }

            if ( now - this->fadeTimer > 10 ) {
                this->fadeTimer = now;
                data->leftLeds->fadeToBlackBy(32);
                data->rightLeds->fadeToBlackBy(32);
            }
         }

    protected:
        unsigned long spawnTimer = 0;
        unsigned long fadeTimer = 0;

        const bool rainbow;

};