#pragma once

#include <FastLED.h>

#include "Hardware_v1.h"
#include "Palettes.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

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
		};

		virtual void Setup() = 0;
		virtual void Loop(LedData *data) = 0;

		const uint8_t mFadeScale;
};

//
// Helper functions to translate X/Y coordinates to ring pixels
//
void setPixelXY(CRGBSet *leds, uint8_t x, uint8_t y, CRGB color)
{
	if (x >= hw::vCols || y >= hw::vRows)
		return;

	for (uint8_t i = 0; i < leds->size(); i++) {
		if (hw::vCoordsX[i] != x)
			continue;
		if (hw::vCoordsY[i] != y)
			continue;
		(*leds)[i] = color;
	}
}

void setWidePixelXY(LedData *data, uint8_t x, uint8_t y, CRGB color)
{
	if ( x<0 || y<0 || x>=(hw::vCols*2) || y>=hw::vRows ) { return; }
	if ( x < hw::vCols ) {
		setPixelXY(data->leftLeds, x, y, color);
	} else {
		setPixelXY(data->rightLeds, x-hw::vCols, y, color);
	}
}

inline uint16_t XY(uint8_t x, uint8_t y)
{
	return x+(y*hw::vCols);
}