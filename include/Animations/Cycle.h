#include "Animation.h"

#ifndef Cycle_h
#define Cycle_h

#include "Animations/Acid.h"
#include "Animations/Fuego.h"
#include "Animations/Hypno.h"
#include "Animations/Sparkle.h"
#include "Animations/TwinkleFOX.h"

#define ANIMATION_INTERVAL_MS 15 * 1000
#define ANIMATION_FADE_DURATION_MS 1000

Animation* animationCycle[] = {
    new Acid(OceanColors_p, 128),
    new Fuego(PoisonGreen_h),
    new Sparkle(false, CHSV(0,0,255)),
    new Acid(LavaColors_p, 128),
    new Fuego(IceBlue_h),
    new TwinkleFOX(FairyLight_p, 128),
    new TwinkleFOX(Ice_p),
    new Acid(PoisonGreen_p),
    new Sparkle(true),
    new Hypno(192),
    new TwinkleFOX(Snow_p),
    new Fuego(FireOrange_h),
};

class Cycle : public Animation
{
    public:
        
        Cycle(LedData *fadeBuffer) :
            Animation(),
            fadeBuffer(fadeBuffer)
        {
        }

        void Setup()
        {
            this->startup = true;
            this->current = 0;
            this->fadeTimer = millis();
            
            this->inTransition = true;
            this->fadeRatio = 0;
            this->sweepX = 0;
            this->sweepY = 0;
            this->next = 0;
            animationCycle[this->next]->Setup();
        }

        void Loop(LedData *data)
        {
            unsigned long now = millis();

             if ( now - this->fadeTimer > ANIMATION_INTERVAL_MS ) {
                this->fadeTimer = now;

                this->inTransition = true;
                this->fadeRatio = 0;
                this->sweepX = 0;
                this->sweepY = 0;
                this->next = (this->current + 1) % ARRAY_SIZE(animationCycle);
                animationCycle[this->next]->Setup();
            }

            // Render a frame of the current animation
            if ( this->startup ) {
                data->leftLeds->fill_solid(CRGB::Black);
                data->rightLeds->fill_solid(CRGB::Black);
            } else {            
                animationCycle[this->current]->Loop(data);
            }

            if ( inTransition ) {
                // Render a frame of the incoming animation
                animationCycle[this->next]->Loop(this->fadeBuffer);

                // Sweep from left to right within ANIMATION_FADE_DURATION
                EVERY_N_MILLISECONDS(ANIMATION_FADE_DURATION_MS / (VIRTUAL_EAR_COLS*2)) {
                    this->sweepX++;
                    if ( this->sweepX >= (VIRTUAL_EAR_COLS*2) ) {
                        this->startup = false;
                        this->inTransition = false;
                        this->current = this->next;
                    }
                }

                // Sweep the left ear...
                if ( this->sweepX < VIRTUAL_EAR_COLS ) {
                    for ( uint8_t i = 0; i < data->leftLeds->size(); i++ ) {
                        if ( coordsX16[i] < this->sweepX ) {
                            (*data->leftLeds)[i] = CRGB((*this->fadeBuffer->leftLeds)[i]);
                        }
                    }
                // ...then the right ear
                } else {
                    (*data->leftLeds) = (*this->fadeBuffer->leftLeds);
                    for ( uint8_t i = 0; i < data->rightLeds->size(); i++ ) {
                        if ( coordsX16[i] < this->sweepX - VIRTUAL_EAR_COLS ) {
                            (*data->rightLeds)[i] = CRGB((*this->fadeBuffer->rightLeds)[i]);
                        }
                    }
                }

                // Overlay the transition line and sparkles
                for ( int y = 0; y < VIRTUAL_EAR_ROWS; y++ ) {
                    setWidePixelXY(data, this->sweepX, y, CHSV(Gold_h.h,255,192));
                    if ( this->sweepX > 0 && random(5) == 1 ) {
                        setWidePixelXY(data, this->sweepX-1, y, CHSV(0,0,192)); 
                    }
                }
            }
        }
    
    protected:
        uint8_t current = 0;
        uint8_t next = 0;
        bool inTransition = false;
        int16_t fadeRatio = 0;
        uint16_t sweepX = 0;
        uint16_t sweepY = 0;
        bool startup = true;

        LedData *fadeBuffer;
    
        unsigned long fadeTimer = 0;

};

#endif