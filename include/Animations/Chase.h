#include "Animation.h"

typedef struct {
    uint8_t hue;
    uint8_t ring;
    uint8_t rpm;
} Pixel;

class Chase : public Animation
{
    public:
        
        Chase(const bool rainbow = false) :
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

                this->rPixels[i].hue = random8();
                this->rPixels[i].ring = i;
                this->rPixels[i].rpm = 30 + random8(60);
            }
        }

        void Loop(LedData *data)
        {
            unsigned long now = millis();

            for ( int i = 0; i < 4; i++ ) {
                (*data->leftRings[i])[scale8(beat8(this->lPixels[i].rpm), data->leftRings[i]->size()-1)] = CHSV(this->lPixels[i].hue, 255, 255);
                (*data->rightRings[i])[scale8(beat8(this->rPixels[i].rpm), data->rightRings[i]->size()-1)] = CHSV(this->rPixels[i].hue, 255, 255);
            }

            if ( now - this->fadeTimer > 10 ) {
                this->fadeTimer = now;
                data->leftLeds->fadeToBlackBy(32);
                data->rightLeds->fadeToBlackBy(32);
            }
         }

    protected:
        unsigned long fadeTimer = 0;
        Pixel lPixels[4];
        Pixel rPixels[4];


        const bool rainbow;


};