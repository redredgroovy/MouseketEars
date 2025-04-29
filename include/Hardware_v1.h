#pragma once

#include <FastLED.h>

namespace hw {
	const uint16_t ledsPerEar = 83;
	const uint8_t  numRings   = 4;
	const uint16_t ringSize[] = {35,24,16,8};

	const uint8_t  vRows      = 16;
	const uint8_t  vCols      = 16;

	//
	// Static mapping of ring pixels to extrapolated X/Y coordinates
	//
	const uint8_t vCoordsX[] = {
	8, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 1, 1, 2, 3, 4, 6, 7, 8, 10, 11, 12, 13, 14, 14, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9,
	8, 6, 5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 13, 13, 12, 11, 10, 9,
	8, 6, 5, 4, 4, 4, 5, 6, 8, 9, 10, 11, 11, 11, 10, 9,
	8, 6, 6, 6, 8, 9, 9, 9,
	};
	const uint8_t vCoordsY[] = {
	15, 15, 15, 14, 13, 12, 11, 10, 8, 7, 6, 5, 3, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 15,
	13, 13, 12, 11, 10, 9, 8, 6, 5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13,
	11, 11, 10, 9, 8, 6, 5, 4, 4, 4, 5, 6, 8, 9, 10, 11,
	9, 9, 8, 6, 6, 6, 8, 9,
	};

};