#include "Animation.h"

#ifndef Fuego_h
#define Fuego_h

class Fuego : public Animation
{
    public:
        
        Fuego(const CHSV color = FireOrange_h) :
            Animation(),
            color(color)
        {
        }

		Fuego(const CHSV color, const uint8_t fadeScale) :
            Animation(fadeScale),
            color(color)
        {
        }

        void Setup()
        {
        }

        void Loop(LedData *data)
        {
            // some changing values
		  	uint16_t ctrl1 = inoise16(11 * millis(), 0, 0);
		  	uint16_t ctrl2 = inoise16(13 * millis(), 100000, 100000);
		  	uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);

			// parameters for the heatmap
			uint16_t speed = 20;
			x[0] = 3 * ctrl * speed;
			y[0] = 20 * millis() * speed;
			z[0] = 5 * millis() * speed ;
			scale_x[0] = ctrl1 / 2;
			scale_y[0] = ctrl2 / 2;

			// calculate the noise data
			uint8_t layer = 0;
			for (uint8_t i = 0; i < VIRTUAL_EAR_COLS; i++) {
				uint32_t ioffset = scale_x[layer] * (i - (VIRTUAL_EAR_COLS/2)-1);
				for (uint8_t j = 0; j < VIRTUAL_EAR_ROWS; j++) {
					uint32_t joffset = scale_y[layer] * (j - (VIRTUAL_EAR_ROWS/2)-1);
					uint16_t data = ((inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer])) + 1);
					noise[layer][i][j] = data >> 8;
				}
			}

			// parameters for the brightness mask
			speed = 20;
			x[1] = 3 * ctrl * speed;
			y[1] = 20 * millis() * speed;
			z[1] = 5 * millis() * speed ;
			scale_x[1] = ctrl1 / 2;
			scale_y[1] = ctrl2 / 2;

			//calculate the noise data
			layer = 1;
			for (uint8_t i = 0; i < VIRTUAL_EAR_COLS; i++) {
				uint32_t ioffset = scale_x[layer] * (i - (VIRTUAL_EAR_COLS/2)-1);
				for (uint8_t j = 0; j < VIRTUAL_EAR_ROWS; j++) {
					uint32_t joffset = scale_y[layer] * (j - (VIRTUAL_EAR_ROWS/2)-1);
					uint16_t data = ((inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer])) + 1);
					noise[layer][i][j] = data >> 8;
				}
			}

			// draw lowest line - seed the fire
			for (uint8_t x = 0; x < VIRTUAL_EAR_COLS; x++) {
				heat[XY(x, VIRTUAL_EAR_ROWS-1)] =  noise[0][VIRTUAL_EAR_ROWS-1][7];
			}

			//copy everything one line up
			for (uint8_t y = 0; y < VIRTUAL_EAR_ROWS - 1; y++) {
				for (uint8_t x = 0; x < VIRTUAL_EAR_COLS; x++) {
					heat[XY(x, y)] = heat[XY(x, y + 1)];
				}
			}

			//dim
			for (uint8_t y = 0; y < VIRTUAL_EAR_ROWS - 1; y++) {
				for (uint8_t x = 0; x < VIRTUAL_EAR_COLS; x++) {
					uint8_t dim = noise[0][x][y];
			 		// high value = high flames
					//dim = dim / 1.7;
					dim = dim / 3;
					dim = 255 - dim;
					heat[XY(x, y)] = scale8(heat[XY(x, y)] , dim);
				}
			}

			for (uint8_t y = 0; y < VIRTUAL_EAR_ROWS; y++) {
				for (uint8_t x = 0; x < VIRTUAL_EAR_COLS; x++) {
					// map the colors based on heatmap
					//(*data->leftRing)[XY(x,y)] = this->rainbowMode ? CHSV(beat8(1), 255, 255) : this->color;
                    buf[XY(x,y)] = this->color;
                    buf[XY(x,y)].nscale8(heat[XY(x,y)]);

					// dim the result based on 2nd noise layer
					buf[XY(x, y)].nscale8(noise[1][x][y]);
                    setPixelXY(data->leftLeds, x, y, buf[XY(x,y)]);
                    setPixelXY(data->rightLeds, x, y, buf[XY(x,y)]);
				}
		 	}

			if (this->fadeScale > 0) {
				data->leftLeds->fadeLightBy(this->fadeScale);
				data->rightLeds->fadeLightBy(this->fadeScale);
			}
		}


    protected:
        const CHSV color;

        // parameters and buffer for the noise array
		static const uint8_t numLayers = 2;
		uint32_t x[numLayers];
		uint32_t y[numLayers];
		uint32_t z[numLayers];
		uint32_t scale_x[numLayers];
		uint32_t scale_y[numLayers];

		uint8_t noise[numLayers][VIRTUAL_EAR_COLS][VIRTUAL_EAR_ROWS];
		uint8_t noise2[numLayers][VIRTUAL_EAR_COLS][VIRTUAL_EAR_ROWS];

		CRGB buf[VIRTUAL_EAR_COLS*VIRTUAL_EAR_ROWS];
        uint8_t heat[VIRTUAL_EAR_COLS*VIRTUAL_EAR_ROWS];

};

#endif