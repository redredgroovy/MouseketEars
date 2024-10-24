#include "Animation.h"

typedef struct {
    uint8_t hue;
    uint8_t ring;
    uint8_t rpm;
} Pixel;

class Radar : public Animation
{
    public:
        
        Radar(const bool rainbow = false) :
            Animation(),
            rainbow(rainbow)
        {
        }

        void Setup()
        {
            this->fadeTimer = millis();
            for ( int i = 0; i < 4; i++ ) {
                this->lPixels[i].hue = random8();
                this->lPixels[i].ring = i;
                this->lPixels[i].rpm = 30 + random8(60);
            }
        }

        void Loop(LedData *data)
        {
            unsigned long now = millis();

            for ( int i = 0; i < 4; i++ ) {
                (*data->leftRings[i])[scale8(beat8(this->lPixels[i].rpm), data->leftRings[i]->size()-1)] = CHSV(this->lPixels[i].hue, 255, 255);
            }

            uint8_t rPos = beatsin8(60, 0, data->rightRings[0]->size()-1);
            (*data->rightRings[0])[rPos] = CRGB::Green;
            rPos = beatsin8(60, 0, data->rightRings[1]->size()-1);
            (*data->rightRings[1])[rPos] = CRGB::Green;
            rPos = beatsin8(60, 0, data->rightRings[2]->size()-1);
            (*data->rightRings[2])[rPos] = CRGB::Green;
            rPos = beatsin8(60, 0, data->rightRings[3]->size()-1);
            (*data->rightRings[3])[rPos] = CRGB::Green;

            if ( now - this->fadeTimer > 10 ) {
                this->fadeTimer = now;
                data->leftLeds->fadeToBlackBy(32);
                data->rightLeds->fadeToBlackBy(32);
            }
         }

    protected:
        unsigned long fadeTimer = 0;
        Pixel lPixels[4];

        const bool rainbow;


};