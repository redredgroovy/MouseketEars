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
        
        Cycle() : Animation()
        {
            // Initialize the buffer for transitioning between two animations
            mFadeBuffer.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
            mFadeBuffer.leftLeds = new CRGBSet(mFadeBuffer.rawLeftLeds, NUM_LEDS_PER_EAR);
            mFadeBuffer.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
            mFadeBuffer.rightLeds = new CRGBSet(mFadeBuffer.rawRightLeds, NUM_LEDS_PER_EAR);
            mFadeBuffer.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);
            mFadeBuffer.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);

            // Define CRGBSets for each individual LED ring to simplify certain animations
            uint8_t led_index = 0;
            for( uint8_t ring = 0; ring < NUM_RINGS; ring++ ) {
                mFadeBuffer.leftRings[ring] = new CRGBSet(*(mFadeBuffer.leftLeds), led_index, led_index+RING_SIZE[ring]-1);
                mFadeBuffer.rightRings[ring] = new CRGBSet(*(mFadeBuffer.rightLeds), led_index, led_index+RING_SIZE[ring]-1);
                led_index += RING_SIZE[ring];
            }
        }

        void Setup()
        {
            mStartup = true;
            mCurrent = 0;
            mFadeTimer = millis();
            
            mInTransition = true;
            mFadeRatio = 0;
            mSweepX = 0;
            mSweepY = 0;
            mNext = 0;
            animationCycle[mNext]->Setup();
        }

        void Loop(LedData *data)
        {
            unsigned long now = millis();

             if ( now - mFadeTimer > ANIMATION_INTERVAL_MS ) {
                mFadeTimer = now;

                mInTransition = true;
                mFadeRatio = 0;
                mSweepX = 0;
                mSweepY = 0;
                mNext = (mCurrent + 1) % ARRAY_SIZE(animationCycle);
                animationCycle[mNext]->Setup();
            }

            // Render a frame of the current animation
            if ( mStartup ) {
                data->leftLeds->fill_solid(CRGB::Black);
                data->rightLeds->fill_solid(CRGB::Black);
            } else {            
                animationCycle[mCurrent]->Loop(data);
            }

            if ( mInTransition ) {
                // Render a frame of the incoming animation
                animationCycle[mNext]->Loop(&mFadeBuffer);

                // Sweep from left to right within ANIMATION_FADE_DURATION
                EVERY_N_MILLISECONDS(ANIMATION_FADE_DURATION_MS / (VIRTUAL_EAR_COLS*2)) {
                    mSweepX++;
                    if ( mSweepX >= (VIRTUAL_EAR_COLS*2) ) {
                        mStartup = false;
                        mInTransition = false;
                        mCurrent = mNext;
                    }
                }

                // Sweep the left ear...
                if ( mSweepX < VIRTUAL_EAR_COLS ) {
                    for ( uint8_t i = 0; i < data->leftLeds->size(); i++ ) {
                        if ( gCoordsX16[i] < mSweepX ) {
                            (*data->leftLeds)[i] = CRGB((*mFadeBuffer.leftLeds)[i]);
                        }
                    }
                // ...then the right ear
                } else {
                    (*data->leftLeds) = (*mFadeBuffer.leftLeds);
                    for ( uint8_t i = 0; i < data->rightLeds->size(); i++ ) {
                        if ( gCoordsX16[i] < mSweepX - VIRTUAL_EAR_COLS ) {
                            (*data->rightLeds)[i] = CRGB((*mFadeBuffer.rightLeds)[i]);
                        }
                    }
                }

                // Overlay the transition line and sparkles
                for ( int y = 0; y < VIRTUAL_EAR_ROWS; y++ ) {
                    setWidePixelXY(data, mSweepX, y, CHSV(Gold_h.h,255,192));
                    if ( mSweepX > 0 && random(5) == 1 ) {
                        setWidePixelXY(data, mSweepX-1, y, CHSV(0,0,192)); 
                    }
                }
            }
        }
    
    protected:
        uint8_t mCurrent = 0;
        uint8_t mNext = 0;

        bool mStartup = true;
        bool mInTransition = false;

        int16_t mFadeRatio = 0;
        uint16_t mSweepX = 0;
        uint16_t mSweepY = 0;
      
        LedData mFadeBuffer;
        unsigned long mFadeTimer = 0;

};

#endif