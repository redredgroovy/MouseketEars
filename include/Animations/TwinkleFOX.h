#include "Animation.h"

#ifndef TwinkleFOX_h
#define TwinkleFOX_h

#define AUTO_SELECT_BACKGROUND_COLOR 0
#define COOL_LIKE_INCANDESCENT 1

class TwinkleFOX : public Animation
{
    public:
        
        TwinkleFOX(CRGBPalette16 palette = PartyColors_p) :
            Animation(),
            currentPalette(palette)
        {
        }

		TwinkleFOX(CRGBPalette16 palette, const uint8_t fadeScale) :
			Animation(fadeScale),
			currentPalette(palette)
        {
        }

        void Setup()
        {
        }

        void Loop(LedData *data)
        {
            // "PRNG16" is the pseudorandom number generator
            // It MUST be reset to the same starting value each time
            // this function is called, so that the sequence of 'random'
            // numbers that it generates is (paradoxically) stable.
            uint16_t PRNG16 = 11337;

            uint32_t clock32 = millis();

            // Set up the background color, "bg".
            // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
            // the current palette are identical, then a deeply faded version of
            // that color is used for the background color
            CRGB bg;
            if( (AUTO_SELECT_BACKGROUND_COLOR == 1) && (currentPalette[0] == currentPalette[1] )) {
                bg = currentPalette[0];
                uint8_t bglight = bg.getAverageLight();
                if( bglight > 64) {
                    bg.nscale8_video( 16); // very bright, so scale to 1/16th
                } else if( bglight > 16) {
                    bg.nscale8_video( 64); // not that bright, so scale to 1/4th
                } else {
                    bg.nscale8_video( 86); // dim, scale to 1/3rd.
                }
            } else {
                bg = backgroundColor; // just use the explicitly defined background color
            }

            uint8_t backgroundBrightness = bg.getAverageLight();

            for( CRGBSet *leds : { data->leftLeds, data->rightLeds } ) {
                for(uint8_t i = 0; i < leds->size(); i++) {
                    CRGB& pixel = (*leds)[i];

                    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
                    uint16_t myclockoffset16= PRNG16; // use that number as clock offset
                    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
                    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
                    uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
                    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
                    uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

                    // We now have the adjusted 'clock' for this pixel, now we call
                    // the function that computes what color the pixel should be based
                    // on the "brightness = f( time )" idea.
                    CRGB c = computeOneTwinkle( myclock30, myunique8);

                    uint8_t cbright = c.getAverageLight();
                    int16_t deltabright = cbright - backgroundBrightness;
                    if( deltabright >= 32 || (!bg)) {
                        // If the new pixel is significantly brighter than the background color,
                        // use the new color.
                    pixel = c;
                    } else if( deltabright > 0 ) {
                        // If the new pixel is just slightly brighter than the background color,
                        // mix a blend of the new color and the background color
                        pixel = blend( bg, c, deltabright * 8);
                    } else {
                        // if the new pixel is not at all brighter than the background color,
                        // just use the background color.
                        pixel = bg;
                    }
                }
            }
            if ( fadeScale > 0 ) {
                data->leftLeds->fadeLightBy(this->fadeScale);
		    	data->rightLeds->fadeLightBy(this->fadeScale);
            }
        }

        uint8_t attackDecayWave8( uint8_t i)
        {
            if( i < 86) {
                return i * 3;
            } else {
                i -= 86;
                return 255 - (i + (i/2));
            }
        }

        void coolLikeIncandescent( CRGB& c, uint8_t phase)
        {
            if( phase < 128) return;

            uint8_t cooling = (phase - 128) >> 4;
            c.g = qsub8( c.g, cooling);
            c.b = qsub8( c.b, cooling * 2);
        }

        CRGB computeOneTwinkle( uint32_t ms, uint8_t salt)
        {
            uint16_t ticks = ms >> (8-twinkleSpeed);
            uint8_t fastcycle8 = ticks;
            uint16_t slowcycle16 = (ticks >> 8) + salt;
            slowcycle16 += sin8( slowcycle16);
            slowcycle16 =  (slowcycle16 * 2053) + 1384;
            uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

            uint8_t bright = 0;
            if( ((slowcycle8 & 0x0E)/2) < twinkleDensity) {
                bright = attackDecayWave8( fastcycle8);
            }

            uint8_t hue = slowcycle8 - salt;
            CRGB c;
            if( bright > 0) {
                c = ColorFromPalette( currentPalette, hue, bright, NOBLEND);
                if( COOL_LIKE_INCANDESCENT == 1 ) {
                coolLikeIncandescent( c, fastcycle8);
                }
            } else {
                c = CRGB::Black;
            }
            return c;
        }

    protected:
        uint8_t twinkleSpeed = 4;
        uint8_t twinkleDensity = 5;
        CRGB backgroundColor = CRGB::Black;
        CRGBPalette16 currentPalette;

};

#endif