#pragma once

#include "MouseketEars.h"

#define FIRE_WIDTH HW_VIRTUAL_COLS*2
#define FIRE_HEIGHT HW_VIRTUAL_ROWS
#define FIRE_CENTER_X (FIRE_WIDTH/2) - 1
#define FIRE_CENTER_Y (FIRE_HEIGHT/2) - 1

class Fuego : public Animation
{
	public:
		Fuego(const CHSV color = FireOrange_h) :
			Animation(),
			mColor(color)
		{
		}

		Fuego(const CHSV color, const uint8_t fadeScale) :
			Animation(fadeScale),
			mColor(color)
		{
		}

		void Loop(LedData *data)
		{
			uint32_t now = millis();

			// some changing values
		  	uint16_t ctrl1 = inoise16(11 * now, 0, 0);
		  	uint16_t ctrl2 = inoise16(13 * now, 100000, 100000);
		  	uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);

			// parameters for the heatmap
			uint16_t speed = 20;
			x[0] = 3 * ctrl * speed;
			y[0] = 20 * now * speed;
			z[0] = 5 * now * speed ;
			scale_x[0] = ctrl1 / 2;
			scale_y[0] = ctrl2 / 2;

			// calculate the noise data
			uint8_t layer = 0;
			for (uint8_t i = 0; i < FIRE_WIDTH; i++) {
				uint32_t ioffset = scale_x[layer] * (i - FIRE_CENTER_X);
				for (uint8_t j = 0; j < FIRE_HEIGHT; j++) {
					uint32_t joffset = scale_y[layer] * (j - FIRE_CENTER_Y);
					uint16_t data = ((inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer])) + 1);
					noise[layer][i][j] = data >> 8;
				}
			}

			// parameters for the brightness mask
			speed = 30;
			x[1] = 3 * ctrl * speed;
			y[1] = 20 * now * speed;
			z[1] = 5 * now * speed ;
			scale_x[1] = ctrl1 / 2;
			scale_y[1] = ctrl2 / 2;

			//calculate the noise data
			layer = 1;
			for (uint8_t i = 0; i < FIRE_WIDTH; i++) {
				uint32_t ioffset = scale_x[layer] * (i - FIRE_CENTER_X);
				for (uint8_t j = 0; j < FIRE_HEIGHT; j++) {
					uint32_t joffset = scale_y[layer] * (j - FIRE_CENTER_Y);
					uint16_t data = ((inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer])) + 1);
					noise[layer][i][j] = data >> 8;
				}
			}

			// draw lowest line - seed the fire
			for (uint8_t x = 0; x < FIRE_WIDTH; x++) {
				heat[XY(x, FIRE_HEIGHT-1)] = noise[0][FIRE_WIDTH-x][FIRE_CENTER_X];
			}

			//copy everything one line up
			for (uint8_t y = 0; y < FIRE_HEIGHT - 1; y++) {
				for (uint8_t x = 0; x < FIRE_WIDTH; x++) {
					heat[XY(x, y)] = heat[XY(x, y + 1)];
				}
			}

			//dim
			for (uint8_t y = 0; y < FIRE_HEIGHT - 1; y++) {
				for (uint8_t x = 0; x < FIRE_WIDTH; x++) {
					uint8_t dim = noise[0][x][y];
			 		// high value = high flames
					//dim = dim / 1.7;
					dim = dim / 7.0;
					dim = 255 - dim;
					heat[XY(x, y)] = scale8(heat[XY(x, y)] , dim);
				}
			}

			for (uint8_t y = 0; y < FIRE_HEIGHT; y++) {
				for (uint8_t x = 0; x < FIRE_WIDTH; x++) {
					// map the colors based on heatmap
					buf[XY(x,y)] = mColor;
					buf[XY(x,y)].nscale8(heat[XY(x,y)]);

					// dim the result based on 2nd noise layer
					buf[XY(x, y)].nscale8(noise[1][x][y]);
					//setPixelXY(data->leftLeds, x, y, buf[XY(x,y)]);
					//setPixelXY(data->rightLeds, x, y, buf[XY(x,y)]);
					setWidePixelXY(data, x, y, buf[XY(x,y)]);
				}
		 	}

			if (mFadeScale > 0) {
				data->leftLeds->fadeLightBy(mFadeScale);
				data->rightLeds->fadeLightBy(mFadeScale);
			}
		}

	protected:
		const CHSV mColor;

		// parameters and buffer for the noise array
		static const uint8_t numLayers = 2;
		uint32_t x[numLayers];
		uint32_t y[numLayers];
		uint32_t z[numLayers];
		uint32_t scale_x[numLayers];
		uint32_t scale_y[numLayers];

		uint8_t noise[numLayers][FIRE_WIDTH][FIRE_HEIGHT];
		uint8_t heat[FIRE_WIDTH*FIRE_HEIGHT];

		CRGB buf[FIRE_WIDTH*FIRE_HEIGHT];
};