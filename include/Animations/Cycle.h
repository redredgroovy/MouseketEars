#pragma once

#include "MouseketEars.h"

#include "Animations/Acid.h"
#include "Animations/Fuego.h"
#include "Animations/Hypno.h"
#include "Animations/Sparkle.h"
#include "Animations/TwinkleFOX.h"

// Cycle interval in milliseconds
// This is the time between animation transitions
#define ANIMATION_INTERVAL_MS 20 * 1000

// Duration of the transition effect in milliseconds
// This is the time it takes to transition from one animation to the next
#define ANIMATION_FADE_DURATION_MS 1000

// Array of animations to cycle through in order
Animation* animationCycle[] = {
	new Acid(OceanColors_p, 128),
	new Fuego(PoisonGreen_h),
	new Sparkle(CHSV(0,0,255)),
	new Acid(LavaColors_p, 128),
	new Fuego(IceBlue_h),
	new TwinkleFOX(FairyLight_p, 128),
	new TwinkleFOX(Ice_p),
	new Acid(PoisonGreen_p),
	new Sparkle(),
	new Hypno(192),
	new TwinkleFOX(Snow_p),
	new Fuego(FireOrange_h),
};

class Cycle : public Animation
{
	public:
		Cycle() : Animation()
		{
			// Initialize additional buffers for transitioning between two animations
			mFadeBuffer.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * HW_LEDS_PER_EAR);
			mFadeBuffer.leftLeds = new CRGBSet(mFadeBuffer.rawLeftLeds, HW_LEDS_PER_EAR);
			mFadeBuffer.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * HW_LEDS_PER_EAR);
			mFadeBuffer.rightLeds = new CRGBSet(mFadeBuffer.rawRightLeds, HW_LEDS_PER_EAR);
			mFadeBuffer.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * HW_NUM_RINGS);
			mFadeBuffer.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * HW_NUM_RINGS);

			// Define CRGBSets for each individual LED ring to simplify certain animations
			uint8_t led_index = 0;
			for( uint8_t ring = 0; ring < HW_NUM_RINGS; ring++ ) {
				mFadeBuffer.leftRings[ring] = new CRGBSet(*(mFadeBuffer.leftLeds), led_index, led_index+HW_RING_SIZE[ring]-1);
				mFadeBuffer.rightRings[ring] = new CRGBSet(*(mFadeBuffer.rightLeds), led_index, led_index+HW_RING_SIZE[ring]-1);
				led_index += HW_RING_SIZE[ring];
			}
		}

		void Setup()
		{
			uint32_t now = millis();

			mStartup = true;

			mFadeInterval = now;
			mFadeStart = now;
			mInTransition = true;
			mSweepDir = SWEEP_LEFT_TO_RIGHT;

			mCurrent = 0;
			mNext = 0;
			animationCycle[mNext]->Setup();
		}

		void Loop(LedData *data)
		{
			uint32_t now = millis();

			// Initiate a new transition if the interval has elapsed
			if ( now - mFadeInterval > ANIMATION_INTERVAL_MS ) {
				mFadeInterval = now;
				mFadeStart = now;
				mInTransition = true;

				// Alternate the sweep direction
				if ( mSweepDir == SWEEP_LEFT_TO_RIGHT ) {
					mSweepDir = SWEEP_RIGHT_TO_LEFT;
				} else {
					mSweepDir = SWEEP_LEFT_TO_RIGHT;
				}

				// Prime the incoming animation
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

				// Map the fade progress to a value between 0 and HW_VIRTUAL_COLS*2 based
				// on elapsed time and the duration of the transition
				float elapsed = now - mFadeStart;
				float progress = min( (elapsed/ANIMATION_FADE_DURATION_MS) * 255.0, 255.0 );
				if ( progress >= 255.0 ){
					mStartup = false;
					mInTransition = false;
					mCurrent = mNext;
				}
				int16_t sweepX = scale8(HW_VIRTUAL_COLS*2, (uint8_t)progress); // 0-HW_VIRTUAL_COLS*2

				if ( mSweepDir == SweepDirection::SWEEP_LEFT_TO_RIGHT ) {
					// Sweep the left ear...
					if ( sweepX < HW_VIRTUAL_COLS ) {
						for ( uint8_t i = 0; i < data->leftLeds->size(); i++ ) {
							if ( HW_V_COORDS_X[i] < sweepX ) {
								(*data->leftLeds)[i] = CRGB((*mFadeBuffer.leftLeds)[i]);
							}
						}
					// ...then the right ear
					} else {
						(*data->leftLeds) = (*mFadeBuffer.leftLeds);
						for ( uint8_t i = 0; i < data->rightLeds->size(); i++ ) {
							if ( HW_V_COORDS_X[i] < sweepX - HW_VIRTUAL_COLS ) {
								(*data->rightLeds)[i] = CRGB((*mFadeBuffer.rightLeds)[i]);
							}
						}
					}
					// Overlay the transition line and sparkles
					for ( int y = 0; y < HW_VIRTUAL_ROWS; y++ ) {
						setWidePixelXY(data, sweepX, y, CHSV(Gold_h.h,255,192));
						if ( random(5) == 1 ) {
							setWidePixelXY(data, sweepX-1, y, CHSV(0,0,192));
						}
					}
				} else { // mSweepDir == SweepDirection::SWEEP_RIGHT_TO_LEFT
					sweepX = (HW_VIRTUAL_COLS*2) - 1 - sweepX; // Invert the sweep direction
					// Sweep the right ear...
					if ( sweepX >= HW_VIRTUAL_COLS ) {
						for ( uint8_t i = 0; i < data->rightLeds->size(); i++ ) {
							if ( HW_V_COORDS_X[i] > sweepX - HW_VIRTUAL_COLS ) {
								(*data->rightLeds)[i] = CRGB((*mFadeBuffer.rightLeds)[i]);
							}
						}
					// ...then the left ear
					} else {
						(*data->rightLeds) = (*mFadeBuffer.rightLeds);
						for ( uint8_t i = 0; i < data->leftLeds->size(); i++ ) {
							if ( HW_V_COORDS_X[i] > sweepX ) {
								(*data->leftLeds)[i] = CRGB((*mFadeBuffer.leftLeds)[i]);
							}
						}
					}
					// Overlay the transition line and sparkles
					for ( int y = 0; y < HW_VIRTUAL_ROWS; y++ ) {
						setWidePixelXY(data, sweepX, y, CHSV(Gold_h.h,255,192));
						if ( random(5) == 1 ) {
							setWidePixelXY(data, sweepX+1, y, CHSV(0,0,192));
						}
					}
				}
			}
		}
	
	protected:
		uint8_t mCurrent = 0;
		uint8_t mNext = 0;

		bool mStartup = true;
		bool mInTransition = false;

		enum SweepDirection {
			SWEEP_LEFT_TO_RIGHT = 0,
			SWEEP_RIGHT_TO_LEFT = 1,
			SWEEP_UP = 2,
			SWEEP_DOWN = 3,
		};

		SweepDirection mSweepDir = SWEEP_RIGHT_TO_LEFT;

		LedData mFadeBuffer;
		uint32_t mFadeInterval = 0;
		uint32_t mFadeStart = 0;

};