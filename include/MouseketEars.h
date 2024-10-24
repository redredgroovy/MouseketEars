#ifndef MouseketEars_h
#define MouseketEars_h

#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

typedef struct {
  CRGB *rawLeftLeds;
  CRGBSet *leftLeds;

  CRGB *rawRightLeds;
  CRGBSet *rightLeds;

  CRGBSet **leftRings;
  CRGBSet **rightRings;
} LedData;

#endif