#pragma once

#include <FastLED.h>

#include "Hardware_v1.h"
#include "Palettes.h"

#define DEBUG 1

#ifdef DEBUG
	#define DPRINT(...) Serial.print(__VA_ARGS__)
	#define DPRINTF(...) Serial.printf(__VA_ARGS__)
	#define DPRINTLN(...) Serial.println(__VA_ARGS__)
#else
	#define DPRINT(...)
	#define DPRINTF(...)
	#define DPRINTLN(...)
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

typedef struct {
	CRGB *rawLeftLeds;
	CRGBSet *leftLeds;

	CRGB *rawRightLeds;
	CRGBSet *rightLeds;

	CRGBSet **leftRings;
	CRGBSet **rightRings;
} LedData;
	
class Animation {
	public:
		Animation(uint8_t fadeScale=0) :
			mFadeScale(fadeScale)
		{
		}

		virtual void Setup()
		{
		}

		virtual void Loop(LedData *data) = 0;

		const uint8_t mFadeScale;
};

//
// Helper functions to translate X/Y coordinates to ring pixels
//
void setPixelXY(CRGBSet *leds, uint8_t x, uint8_t y, CRGB color)
{
	if ( x<0 || y<0 || x >= HW_VIRTUAL_COLS || y >= HW_VIRTUAL_ROWS)
		return;

	for (uint8_t i = 0; i < leds->size(); i++) {
		if (HW_V_COORDS_X[i] != x)
			continue;
		if (HW_V_COORDS_Y[i] != y)
			continue;
		(*leds)[i] = color;
	}
}

void setWidePixelXY(LedData *data, uint8_t x, uint8_t y, CRGB color)
{
	if ( x < HW_VIRTUAL_COLS ) {
		setPixelXY(data->leftLeds, x, y, color);
	} else {
		setPixelXY(data->rightLeds, x-HW_VIRTUAL_COLS, y, color);
	}
}

inline uint16_t XY(uint8_t x, uint8_t y)
{
	return x+(y*HW_VIRTUAL_COLS);
}