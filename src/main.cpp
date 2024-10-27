#include "Arduino.h"

#include <ezButton.h>
#include <FastLED.h>
#include <RunningMedian.h>

#include "MouseketEars.h"

#include "Animation.h"
#include "Animations/Acid.h"
#include "Animations/Charge.h"
#include "Animations/Cycle.h"
#include "Animations/Fuego.h"
#include "Animations/Hypno.h"
#include "Animations/Sparkle.h"
#include "Animations/TwinkleFOX.h"

#undef DEBUG

#define LEFT_EAR_PIN D5
#define RIGHT_EAR_PIN D4
#define BUTTON_PIN D7
#define VOLTAGE_PIN A0

#define VOLTAGE_CUTOFF 3.5

#define CHIPSET WS2812B
#define PIXEL_ORDER GRB
#define MAX_POWER_IN_MW 1000
#define MAX_BRIGHTNESS 32

#ifdef DEBUG
  #undef VOLTAGE_CUTOFF
  #define VOLTAGE_CUTOFF 0
#endif

LedData gLeds;
LedData gFadeBuffer;

Animation* gAnimations[] = {
  new Cycle(&gFadeBuffer),

  new Acid(LavaColors_p, 128),
  new Acid(OceanColors_p, 128),
  new Acid(PoisonGreen_p),
  
  new Fuego(FireOrange_h),
  new Fuego(PoisonGreen_h),
  new Fuego(IceBlue_h),
    
  new Sparkle(false, Gold_h),
  new Sparkle(false, CHSV(0,0,255)),
  new Sparkle(true),
  
  new TwinkleFOX(FairyLight_p, 128),
  new TwinkleFOX(Ice_p),
  new TwinkleFOX(Snow_p),

  new Hypno(192),

  new Charge(),
};

uint8_t gCurrentAnimation = 0;
/*
uint8_t gNextAnimation = 0;
bool gInTransition = false;
int16_t gFadeRatio = 0;
uint16_t gSweepX = 0;
uint16_t gSweepY = 0;
*/

ezButton gButton(BUTTON_PIN);

RunningMedian gVoltageSamples = RunningMedian(16);

// Courtesy of https://github.com/G6EJD/LiPo_Battery_Capacity_Estimator
uint8_t currentChargePct(float voltage)
{
  if ( voltage < 3.5 ) { return 0; }
  else if ( voltage > 4.2 ) { return 100; }
  
  return (uint8_t)(2808.3808 * pow(voltage, 4) - 
                   43560.9157 * pow(voltage, 3) +
                   252848.5888 * pow(voltage, 2) -
                   650767.4615 * voltage + 626532.5703);
}

// Once we enter voltageCutoff, we stay here for safety until the controller is reset
void voltageCutoff()
{
  if ( gVoltageSamples.getMedian() >= VOLTAGE_CUTOFF ) {
    return;
  }

  #ifdef DEBUG
    Serial.println("Triggered voltage cutoff");
  #endif 

  FastLED.clear();
  FastLED.show();

  bool ledState = false;

  while ( true ) {
    delay(1000);
    ledState = !ledState;
    (*gLeds.leftLeds)[31] = CRGB((ledState?16:0),0,0);
    FastLED.show();
  }
}

void setup()
{
  Serial.begin(115200);
	while (!Serial && millis() < 2000);

  #ifdef DEBUG
    delay(2000); // failsafe to avoid a bricked controller
  #endif

  // Initialize the FastLED configuration
  FastLED.setMaxPowerInMilliWatts(MAX_POWER_IN_MW);
  FastLED.setBrightness(MAX_BRIGHTNESS);

  // Initialize the FastLED pixel data
  gLeds.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gLeds.leftLeds = new CRGBSet(gLeds.rawLeftLeds, NUM_LEDS_PER_EAR);
  gLeds.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gLeds.rightLeds = new CRGBSet(gLeds.rawRightLeds, NUM_LEDS_PER_EAR);
  gLeds.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);
  gLeds.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);

  // Initialize the buffer for transitioning between two animations
  gFadeBuffer.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gFadeBuffer.leftLeds = new CRGBSet(gFadeBuffer.rawLeftLeds, NUM_LEDS_PER_EAR);
  gFadeBuffer.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS_PER_EAR);
  gFadeBuffer.rightLeds = new CRGBSet(gFadeBuffer.rawRightLeds, NUM_LEDS_PER_EAR);
  gFadeBuffer.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);
  gFadeBuffer.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * NUM_RINGS);

  // Define CRGBSets for each individual LED ring to simplify certain animations
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

  // Prime the voltage monitor and immediately enter the safety cuffoff if we are low volage
  pinMode(VOLTAGE_PIN, INPUT);
  for( uint8_t i = 0; i < 16; i++) {
    float reading = 2 * analogReadMilliVolts(VOLTAGE_PIN) / 1000.0; // attenuation ratio 1/2, mV --> V
    gVoltageSamples.add(reading); 
  }
  voltageCutoff();

  gAnimations[gCurrentAnimation]->Setup();
}

void loop()
{
  // Sample the battery voltage 10 times per second and enter the safety cutoff if we are low voltage
  EVERY_N_MILLISECONDS(100) {
    float reading = 2 * analogReadMilliVolts(VOLTAGE_PIN) / 1000.0; // attenuation ratio 1/2, mV --> V
    gVoltageSamples.add(reading);
    voltageCutoff();
  }

  // Use the button to trigger animation rotation
  gButton.loop();
  if ( gButton.isPressed() ) {
    gCurrentAnimation = (gCurrentAnimation + 1) % ARRAY_SIZE(gAnimations);
    gAnimations[gCurrentAnimation]->Setup();
    //gCurrentAnimation = gNextAnimation;
    //gInTransition = 0;
  }
  
  /*
  EVERY_N_SECONDS(ANIMATION_INTERVAL) {
    gInTransition = true;
    gFadeRatio = 0;
    gSweepX = 0;
    gSweepY = 0;
    gNextAnimation = (gCurrentAnimation + 1) % ARRAY_SIZE(gAnimations);
    gAnimations[gNextAnimation]->Setup();
  }
  */

/*
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
*/

  // Render a frame of the current animation
  gAnimations[gCurrentAnimation]->Loop(&gLeds);

/*
  if ( gInTransition ) {
    gAnimations[gNextAnimation]->Loop(&gFadeBuffer);

    // Sweep from left to right within ANIMATION_FADE_DURATION
    EVERY_N_MILLISECONDS(ANIMATION_FADE_DURATION / (VIRTUAL_EAR_COLS*2)) {
      gSweepX++;
      if ( gSweepX >= (VIRTUAL_EAR_COLS*2) ) {
        gInTransition = false;
        gCurrentAnimation = gNextAnimation;
      }
    }

    // Sweep the left ear...
    if ( gSweepX < VIRTUAL_EAR_COLS ) {
      for ( uint8_t i = 0; i < gLeds.leftLeds->size(); i++ ) {
        if ( coordsX16[i] < gSweepX ) {
          (*gLeds.leftLeds)[i] = CRGB((*gFadeBuffer.leftLeds)[i]);
        }
      }
    // ...then the right ear
    } else {
      (*gLeds.leftLeds) = (*gFadeBuffer.leftLeds);
      for ( uint8_t i = 0; i < gLeds.rightLeds->size(); i++ ) {
        if ( coordsX16[i] < gSweepX - VIRTUAL_EAR_COLS ) {
          (*gLeds.rightLeds)[i] = CRGB((*gFadeBuffer.rightLeds)[i]);
        }
      }
    }

    // Overlay the transition line and sparkles
    for ( int y = 0; y < VIRTUAL_EAR_ROWS; y++ ) {
      setWidePixelXY(&gLeds, gSweepX, y, CHSV(Gold_h.h,255,192));
      if ( gSweepX>0 && random(5) == 1 ) {
          setWidePixelXY(&gLeds, gSweepX-1, y, CHSV(0,0,192)); 
      }
    }
  }
*/
  FastLED.show();

  #ifdef DEBUG                                             
    EVERY_N_MILLISECONDS(1000) {
      Serial.printf("FPS: %d\n", FastLED.getFPS());
      Serial.printf("a:%0.3f m:%0.3f %d%%\n", gVoltageSamples.getAverage(), gVoltageSamples.getMedian(), currentChargePct(gVoltageSamples.getMedian()));
    }
  #endif

}