#pragma once

#include <FastLED.h>

#define HW_LEDS_PER_EAR 83
#define HW_NUM_RINGS 4
#define HW_RING_SIZE (uint8_t[]) {35,24,16,8}

#define HW_VIRTUAL_ROWS 16
#define HW_VIRTUAL_COLS 16

#define HW_CHIPSET WS2812B
#define HW_PIXEL_ORDER GRB

#define HW_LEFT_EAR_PIN D5
#define HW_RIGHT_EAR_PIN D4
#define HW_BUTTON_PIN D7
#define HW_VOLTAGE_PIN A0

#define HW_MAX_POWER_IN_MW 1000
#define HW_MAX_BRIGHTNESS 32
#define HW_VOLTAGE_CUTOFF 3.5

//
// Static mapping of ring pixels to virtual X/Y coordinates
//
const uint8_t HW_V_COORDS_X[] = {
	8, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 1, 1, 2, 3, 4, 6, 7, 8, 10, 11, 12, 13, 14, 14, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9,
	8, 6, 5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 13, 13, 12, 11, 10, 9,
	8, 6, 5, 4, 4, 4, 5, 6, 8, 9, 10, 11, 11, 11, 10, 9,
	8, 6, 6, 6, 8, 9, 9, 9,
};
const uint8_t HW_V_COORDS_Y[] = {
	15, 15, 15, 14, 13, 12, 11, 10, 8, 7, 6, 5, 3, 2, 1, 1, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 15,
	13, 13, 12, 11, 10, 9, 8, 6, 5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13,
	11, 11, 10, 9, 8, 6, 5, 4, 4, 4, 5, 6, 8, 9, 10, 11,
	9, 9, 8, 6, 6, 6, 8, 9,
};