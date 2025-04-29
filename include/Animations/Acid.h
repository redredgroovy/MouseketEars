#pragma once

#include "MouseketEars.h"

class Acid : public Animation
{
	public:
		
		Acid(CRGBPalette16 palette = PoisonGreen_p) :
			Animation(),
			mCurrentPalette(palette)
		{
		}
		
		Acid(CRGBPalette16 palette, const uint8_t fadeScale) :
			Animation(fadeScale),
			mCurrentPalette(palette)
		{
		}

		void Setup()
		{
			noisesmoothing = 200;
			colorshift = 0;

			// just any free input pin
			random16_add_entropy(analogRead(A1));

			// fill coordinates with random values
			// set zoom levels
			for(int i = 0; i < NUM_LAYERS; i++) {
				x[i] = random16();
				y[i] = random16();
				z[i] = random16();
				scale_x[i] = 6000;
				scale_y[i] = 6000;
			}
		}

		void Loop(LedData *data)
		{
			x[0] = beatsin16(3, 200, 64000);
			y[0] += 100;
			z[0] = 7000;
			scale_x[0] = 6000;
			scale_y[0] = 8000;
			FillNoise(0);

			x[1] = beatsin16(2, 200, 64000);
			y[1] += 130;
			z[1] = 7000;
			scale_x[1] = 6000;
			scale_y[1] = 8000;
			FillNoise(1);

			x[2] = beatsin16(4, 200, 6400);
			y[2] += 1000;
			z[2] = 3000;
			scale_x[2] = 7000;
			scale_y[2] = 8000;
			FillNoise(2);

			MergeMethod1(data, 2);

			data->leftLeds->fadeLightBy(mFadeScale);
			data->rightLeds->fadeLightBy(mFadeScale);
		}

		void FillNoise(byte layer)
		{
			for(uint8_t i = 0; i < (hw::vCols*2); i++) {
				uint32_t ioffset = scale_x[layer] * (i-(hw::vCols*2)/2);

				for(uint8_t j = 0; j < hw::vRows; j++) {
					uint32_t joffset = scale_y[layer] * (j-hw::vRows/2);

					byte data = inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer]) >> 8;

					// Marks data smoothing
					data = qsub8(data, 16);
					data = qadd8(data, scale8(data,39));

					uint8_t olddata = noise[layer][i][j];
					uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
					data = newdata;

					noise[layer][i][j] = data;
				}
			}
		}

		void MergeMethod1(LedData *data, byte colorrepeat)
		{
			for(uint8_t i = 0; i < (hw::vCols*2); i++) {
				for(uint8_t j = 0; j < hw::vRows; j++) {

					// map the noise values down to a byte range
					// layer 0 and 2 interfere for the color
					uint8_t color = ( ( noise[0][i][j] )
						+ ( noise[1][i][j] )
						+ ( noise[2][i][j] ) )
						/ 3;

					// layer 2 gives the brightness  
					uint8_t bri = (noise[2][i][j]);

					// assign a color depending on the actual palette
					CRGB pixel = ColorFromPalette(mCurrentPalette, colorrepeat * (color + colorshift), bri);

					// Map a virtual matrix across both ears
					if ( i < hw::vCols ) {
						setPixelXY(data->leftLeds,i,j,pixel);
					} else {
						setPixelXY(data->rightLeds,i-hw::vCols,j,pixel);
					}
				}
			}
		}

	protected:

		static const uint8_t NUM_LAYERS = 3;

		uint32_t x[NUM_LAYERS];
		uint32_t y[NUM_LAYERS];
		uint32_t z[NUM_LAYERS];
		uint32_t scale_x[NUM_LAYERS];
		uint32_t scale_y[NUM_LAYERS];

		uint8_t noise[NUM_LAYERS][(hw::vCols*2)][hw::vRows];

		CRGBPalette16 mCurrentPalette;
		byte colorshift;
		uint8_t noisesmoothing;

};