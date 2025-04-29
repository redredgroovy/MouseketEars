#pragma once

#include "Animation.h"

class Sparkle : public Animation
{
    public:
        
        Sparkle(const bool rainbow, const CHSV color = CHSV(0,0,255)) :
            Animation(),
            mRainbow(rainbow),
            mColor(color)
        {
        }

        void Setup()
        {
            mSpawnTimer = millis();
            mFadeTimer = millis();
        }

        void Loop(LedData *data)
        {
            unsigned long now = millis();

            if ( now - mSpawnTimer > 20 ) {
                CHSV color;
                mSpawnTimer = now;

                color = ( mRainbow ? CHSV(random(0,255), 255, 255) : mColor );
                color.v = scale8_video(color.v, 192);
		        (*data->leftLeds)[random(0, data->leftLeds->size())] = color;
                
                color = ( mRainbow ? CHSV(random(0,255), 255, 255) : mColor );
                color.v = scale8_video(color.v, 192);
		        (*data->rightLeds)[random(0, data->rightLeds->size())] = color;
            }

            if ( now - mFadeTimer > 10 ) {
                mFadeTimer = now;
                data->leftLeds->fadeToBlackBy(32);
                data->rightLeds->fadeToBlackBy(32);
            }
        }

    protected:
        unsigned long mSpawnTimer = 0;
        unsigned long mFadeTimer = 0;

        const bool mRainbow;
        const CHSV mColor;

};