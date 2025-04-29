#pragma once

#include <FastLED.h>

//
// Custom colors and palettes
//
const CHSV FireOrange_h = CHSV(28,255,255);
const CHSV PoisonGreen_h = CHSV(90,255,255);
const CHSV IceGreen_h = CHSV(120,255,255);
const CHSV IceBlue_h = CHSV(145,255,255);
const CHSV Gold_h = CHSV(40,255,255);

const CRGBPalette16 IceColors_p FL_PROGMEM = CRGBPalette16(
	CRGB::Black,
	CRGB::Blue,
	CRGB::Aqua,
	CRGB::White
);

const CRGBPalette16 PoisonGreen_p FL_PROGMEM = CRGBPalette16(
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