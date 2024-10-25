#include "Arduino.h"

#include <ezButton.h>
#include <FastLED.h>
#include <RunningMedian.h>

#include "MouseketEars.h"

#include "Animation.h"
#include "Animations/Charge.h"
#include "Animations/Chase.h"
#include "Animations/Hypno.h"
#include "Animations/Sparkle.h"

#define LEFT_EAR_PIN D4
#define RIGHT_EAR_PIN D5
#define BUTTON_PIN D7
#define VOLTAGE_PIN A0

#define VOLTAGE_CUTOFF 3.4

#define CHIPSET WS2812B
#define PIXEL_ORDER GRB
#define MAX_POWER_IN_MW 1000
#define MAX_BRIGHTNESS 16

#define NUM_LEDS_PER_EAR 83
#define RING_SIZE        (uint8_t[]) {35,24,16,8}
#define NUM_RINGS        4

#define ANIMATION_INTERVAL 1000
#define ANIMATION_FADE_DURATION 1

Animation* gAnimations[] = {
  new Hypno(),
  new Charge(),
  new Hypno(),
  new Sparkle(false),
  new Chase(),
  new Sparkle(true),
};

uint8_t gCurrentAnimation = 0;
uint8_t gNextAnimation = 0;
bool gInTransition = false;
int16_t gFadeRatio = 0;

LedData gLeds;
LedData gFadeBuffer;

ezButton gButton(BUTTON_PIN);

RunningMedian gVoltageSamples = RunningMedian(16);

uint8_t currentCharge(float voltage)
{
  if ( voltage >= 4.20 ) { return 100; }
  else if ( voltage >= 4.15 ) { return 95; }
  else if ( voltage >= 4.11 ) { return 90; }
  else if ( voltage >= 4.08 ) { return 85; }
  else if ( voltage >= 4.02 ) { return 80; }
  else if ( voltage >= 3.98 ) { return 75; }
  else if ( voltage >= 3.95 ) { return 70; }
  else if ( voltage >= 3.91 ) { return 65; }
  else if ( voltage >= 3.87 ) { return 60; }
  else if ( voltage >= 3.85 ) { return 55; }
  else if ( voltage >= 3.84 ) { return 50; }
  else if ( voltage >= 3.82 ) { return 45; }
  else if ( voltage >= 3.80 ) { return 40; }
  else if ( voltage >= 3.79 ) { return 35; }
  else if ( voltage >= 3.77 ) { return 30; }
  else if ( voltage >= 3.75 ) { return 25; }
  else if ( voltage >= 3.73 ) { return 20; }
  else if ( voltage >= 3.71 ) { return 15; }
  else if ( voltage >= 3.69 ) { return 10; }
  else if ( voltage >= 3.61 ) { return 5; }
  return 0;
}

void setup()
{
  Serial.begin(115200);
	while (!Serial && millis() < 2000);

  // Prime the voltage monitor
  pinMode(VOLTAGE_PIN, INPUT);
  for( uint8_t i = 0; i < 16; i++) {
    float reading = 2 * analogReadMilliVolts(VOLTAGE_PIN) / 1000.0; // attenuation ratio 1/2, mV --> V
    gVoltageSamples.add(reading); 
  }

  // Immediately enter the safety cutoff if we are low voltage
  if ( gVoltageSamples.getMedian() < VOLTAGE_CUTOFF ) {}

  // Initialize the FastLED configuration
  FastLED.setMaxPowerInMilliWatts(MAX_POWER_IN_MW);
  FastLED.setBrightness(MAX_BRIGHTNESS);

  gLeds.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gLeds.leftLeds = new CRGBSet(gLeds.rawLeftLeds, NUM_LEDS_PER_EAR);
  gLeds.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gLeds.rightLeds = new CRGBSet(gLeds.rawRightLeds, NUM_LEDS_PER_EAR);
  gLeds.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);
  gLeds.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);

  gFadeBuffer.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gFadeBuffer.leftLeds = new CRGBSet(gFadeBuffer.rawLeftLeds, NUM_LEDS_PER_EAR);
  gFadeBuffer.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gFadeBuffer.rightLeds = new CRGBSet(gFadeBuffer.rawRightLeds, NUM_LEDS_PER_EAR);
  gFadeBuffer.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);
  gFadeBuffer.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);

  uint8_t led_index = 0;
  for( uint8_t ring = 0; ring < NUM_RINGS; ring++ ) {
    gLeds.leftRings[ring] = new CRGBSet(*(gLeds.leftLeds), led_index, led_index+RING_SIZE[ring]-1);
    gLeds.rightRings[ring] = new CRGBSet(*(gLeds.rightLeds), led_index, led_index+RING_SIZE[ring]-1);
    gFadeBuffer.leftRings[ring] = new CRGBSet(*(gFadeBuffer.leftLeds), led_index, led_index+RING_SIZE[ring]-1);
    gFadeBuffer.rightRings[ring] = new CRGBSet(*(gFadeBuffer.rightLeds), led_index, led_index+RING_SIZE[ring]-1);
    led_index += RING_SIZE[ring];
  }

  FastLED.addLeds<CHIPSET, LEFT_EAR_PIN, PIXEL_ORDER>(gLeds.rawLeftLeds, NUM_LEDS_PER_EAR);
  FastLED.addLeds<CHIPSET, RIGHT_EAR_PIN, PIXEL_ORDER>(gLeds.rawRightLeds, NUM_LEDS_PER_EAR);

  FastLED.clear();
  FastLED.show();

  gButton.setDebounceTime(50);

  gAnimations[gCurrentAnimation]->Setup();
}

void loop()
{
  // Sample the battery voltage twice per second
  EVERY_N_MILLISECONDS(500) {
    float reading = 2 * analogReadMilliVolts(VOLTAGE_PIN) / 1000.0; // attenuation ratio 1/2, mV --> V
    gVoltageSamples.add(reading);
  }

  // Immediately enter the safety cutoff if we are low voltage
  if ( gVoltageSamples.getMedian() < VOLTAGE_CUTOFF ) {}

  gButton.loop();
  if ( gButton.isPressed() ) {
    gNextAnimation = (gCurrentAnimation + 1) % ARRAY_SIZE(gAnimations);
    gAnimations[gNextAnimation]->Setup();
    gCurrentAnimation = gNextAnimation;
    gInTransition = 0;
    Serial.println("click");
  }

  EVERY_N_SECONDS(5) {
    Serial.printf("a:%0.3f m:%0.3f %d%%\n", gVoltageSamples.getAverage(), gVoltageSamples.getMedian(), currentCharge(gVoltageSamples.getMedian()));
  }
  
  /*
  EVERY_N_SECONDS(ANIMATION_INTERVAL) {
    gInTransition = true;
    gFadeRatio = 0;
    gNextAnimation = (gCurrentAnimation + 1) % ARRAY_SIZE(gAnimations);
    gAnimations[gNextAnimation]->Setup();
  }
  */

  EVERY_N_MILLISECONDS(ANIMATION_FADE_DURATION * 1000 / 64) {
    if ( gInTransition ) {
      if ( gFadeRatio < 128 ) {
        gFadeRatio += 4;
      } else {
        gFadeRatio += 1;
      }
      if ( gFadeRatio >= 255) {
        gInTransition = false;
        gCurrentAnimation = gNextAnimation;
      }
    } 
  }

  gAnimations[gCurrentAnimation]->Loop(&gLeds);

  if ( gInTransition ) {
    gAnimations[gNextAnimation]->Loop(&gFadeBuffer);   
    gLeds.leftLeds->nblend(*(gFadeBuffer.leftLeds), gFadeRatio);
    gLeds.rightLeds->nblend(*(gFadeBuffer.rightLeds), gFadeRatio);
  }

  FastLED.show();

  /*                                              
  EVERY_N_MILLISECONDS(1000) {
    Serial && Serial.printf("FPS: %d\n", FastLED.getFPS());
  }
  */

}