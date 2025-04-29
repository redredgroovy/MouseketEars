#pragma once

#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define NUM_LEDS_PER_EAR 83
#define RING_SIZE        (uint8_t[]) {35,24,16,8}
#define NUM_RINGS        4

#define VIRTUAL_EAR_ROWS 16
#define VIRTUAL_EAR_COLS 16

typedef struct {
  CRGB *rawLeftLeds;
  CRGBSet *leftLeds;

  CRGB *rawRightLeds;
  CRGBSet *rightLeds;

  CRGBSet **leftRings;
  CRGBSet **rightRings;
} LedData;

//
// Static mapping of pixels to exxtrapolated X/Y coordinates
//
uint8_t gCoordsX256[] = {
128, 105, 83, 62, 44, 28, 15, 6, 1, 0, 3, 10, 21, 36, 53, 73, 94, 117, 139, 162, 183, 203, 220, 234, 245, 252, 255, 254, 248, 239, 227, 211, 192, 171, 149,
128, 104, 81, 62, 48, 38, 35, 38, 48, 62, 81, 104, 128, 151, 174, 193, 207, 217, 220, 217, 207, 193, 174, 151,
128, 105, 87, 74, 70, 74, 87, 105, 128, 150, 168, 181, 185, 181, 168, 150,
128, 107, 99, 107, 128, 148, 156, 148,
};
uint8_t gCoordsY256[] = {
255, 253, 247, 237, 223, 207, 188, 167, 144, 121, 99, 77, 57, 39, 24, 12, 4, 0, 1, 5, 13, 25, 40, 58, 78, 100, 123, 145, 168, 189, 208, 224, 237, 247, 253,
220, 217, 207, 193, 174, 151, 128, 104, 81, 62, 48, 38, 35, 38, 48, 62, 81, 104, 128, 151, 174, 193, 207, 217,
185, 181, 168, 150, 128, 105, 87, 74, 70, 74, 87, 105, 128, 150, 168, 181,
156, 148, 128, 107, 99, 107, 128, 148,
};

uint8_t gCoordsX16[] = {
8, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 1, 1, 2, 3, 4, 6, 7, 8, 10, 11, 12, 13, 14, 14, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9,
8, 6, 5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 13, 13, 12, 11, 10, 9,
8, 6, 5, 4, 4, 4, 5, 6, 8, 9, 10, 11, 11, 11, 10, 9,
8, 6, 6, 6, 8, 9, 9, 9,
};
uint8_t gCoordsY16[] = {
15, 15, 15, 14, 13, 12, 11, 10, 8, 7, 6, 5, 3, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 15,
13, 13, 12, 11, 10, 9, 8, 6, 5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13,
11, 11, 10, 9, 8, 6, 5, 4, 4, 4, 5, 6, 8, 9, 10, 11,
9, 9, 8, 6, 6, 6, 8, 9,
};

uint8_t gCoordsX8[] = {
4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 6, 6, 5, 5, 4,
4, 3, 2, 2, 1, 1, 1, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6, 5, 5, 4,
4, 3, 2, 2, 2, 2, 2, 3, 4, 4, 5, 5, 5, 5, 5, 4,
4, 3, 3, 3, 4, 4, 4, 4,
};
uint8_t gCoordsY8[] = {
7, 7, 7, 7, 6, 6, 5, 5, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 7,
6, 6, 6, 5, 5, 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6,
5, 5, 5, 4, 4, 3, 2, 2, 2, 2, 2, 3, 4, 4, 5, 5,
4, 4, 4, 3, 3, 3, 4, 4,
};


//
// Helper functions to translate X/Y coordinates to ring pixels
//
void setPixelXY(CRGBSet *leds, uint8_t x, uint8_t y, CRGB color)
{
  if (x >= 16 || y >= 16)
    return;

  for (uint8_t i = 0; i < leds->size(); i++) {
    if (gCoordsX16[i] != x)
      continue;
    if (gCoordsY16[i] != y)
      continue;
    (*leds)[i] = color;
  }
}

void setWidePixelXY(LedData *data, uint8_t x, uint8_t y, CRGB color)
{
  if ( x<0 || y<0 || x>=(VIRTUAL_EAR_COLS*2) || y>=VIRTUAL_EAR_ROWS ) { return; }
  if ( x < VIRTUAL_EAR_COLS ) {
    setPixelXY(data->leftLeds, x, y, color);
  } else {
    setPixelXY(data->rightLeds, x-VIRTUAL_EAR_COLS, y, color);
  }
}

//
// Custom colors and palettes
//
CHSV FireOrange_h = CHSV(28,255,255);
CHSV PoisonGreen_h = CHSV(90,255,255);
CHSV IceGreen_h = CHSV(120,255,255);
CHSV IceBlue_h = CHSV(145,255,255);
CHSV Gold_h = CHSV(40,255,255);

CRGBPalette16 IceColors_p = CRGBPalette16(
  CRGB::Black,
  CRGB::Blue,
  CRGB::Aqua,
  CRGB::White
);

CRGBPalette16 PoisonGreen_p  = CRGBPalette16(
  CHSV(PoisonGreen_h.h, 255, 255), 
	CHSV(PoisonGreen_h.h, 255, 0), 
	CHSV(PoisonGreen_h.h, 255, 0),
	CHSV(PoisonGreen_h.h, 255, 0)
);

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM = {
  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
  HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight,
  QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight,
  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight
};

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM = {
  0x304048, 0x304048, 0x304048, 0x304048,
  0x304048, 0x304048, 0x304048, 0x304048,
  0x304048, 0x304048, 0x304048, 0x304048,
  0x304048, 0x304048, 0x304048, 0xE0F0FF
};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM = {
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM = {
  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray
};