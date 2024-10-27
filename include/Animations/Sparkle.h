#include "Animation.h"

#ifndef Sparkle_h
#define Sparkle_h

class Sparkle : public Animation
{
    public:
        
        Sparkle(const bool rainbow, const CHSV color = CHSV(0,0,255)) :
            Animation(),
            rainbow(rainbow),
            color(color)
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
                CHSV color;
                this->spawnTimer = now;

                color = ( this->rainbow ? CHSV(random(0,255), 255, 255) : this->color );
                color.v = scale8_video(color.v, 192);
		        (*data->leftLeds)[random(0, data->leftLeds->size())] = color;
                
                color = ( this->rainbow ? CHSV(random(0,255), 255, 255) : this->color );
                color.v = scale8_video(color.v, 192);
		        (*data->rightLeds)[random(0, data->rightLeds->size())] = color;
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
        const CHSV color;

};

#endif