#include "Arduino.h"

#include <ezButton.h>
#include <FastLED.h>
#include <RunningMedian.h>

#include "MouseketEars.h"
#include "Animations/Acid.h"
#include "Animations/Charge.h"
#include "Animations/Cycle.h"
#include "Animations/Fuego.h"
#include "Animations/Hypno.h"
#include "Animations/Sparkle.h"
#include "Animations/TwinkleFOX.h"

#define LEFT_EAR_PIN D5
#define RIGHT_EAR_PIN D4
#define BUTTON_PIN D7
#define VOLTAGE_PIN A0

#define VOLTAGE_CUTOFF 3.5

#define CHIPSET WS2812B
#define PIXEL_ORDER GRB
#define MAX_POWER_IN_MW 1000
#define MAX_BRIGHTNESS 32

LedData gLeds;

Animation* gAnimations[] = {
	new Cycle(),

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
	new TwinkleFOX(BlueWhite_p),

	new Hypno(192),

	new Charge(),
};
uint8_t gCurrentAnimation = 0;

ezButton gButton(BUTTON_PIN);

RunningMedian gVoltageSamples = RunningMedian(16);

uint8_t currentChargePct(float voltage)
{
	// Cutoff at 3.5 volts - we're likely under 5% capacity anyway.
	if ( voltage < 3.5 ) { return 0; }
	else if ( voltage > 4.2 ) { return 100; }

	// Approximation of discharge curve from 4.2  -> 3.5 volts
	return (uint8_t)(123 - (123 / (pow(1 + pow((voltage/3.7), 80), 0.165))));
}

// Once we enter voltageCutoff, we stay here for safety until the controller is reset
void voltageCutoff()
{
	if ( gVoltageSamples.getMedian() >= VOLTAGE_CUTOFF ) {
		return;
	}
	DPRINTLN("Triggered voltage cutoff");

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
	gLeds.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * hw::ledsPerEar);
	gLeds.leftLeds = new CRGBSet(gLeds.rawLeftLeds, hw::ledsPerEar);
	gLeds.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * hw::ledsPerEar);
	gLeds.rightLeds = new CRGBSet(gLeds.rawRightLeds, hw::ledsPerEar);
	gLeds.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * hw::numRings);
	gLeds.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * hw::numRings);

	// Define CRGBSets for each individual LED ring to simplify certain animations
	uint8_t led_index = 0;
	for( uint8_t ring = 0; ring < hw::numRings; ring++ ) {
		gLeds.leftRings[ring] = new CRGBSet(*(gLeds.leftLeds), led_index, led_index+hw::ringSize[ring]-1);
		gLeds.rightRings[ring] = new CRGBSet(*(gLeds.rightLeds), led_index, led_index+hw::ringSize[ring]-1);
		//gFadeBuffer.leftRings[ring] = new CRGBSet(*(gFadeBuffer.leftLeds), led_index, led_index+hw::ringSize[ring]-1);
		//gFadeBuffer.rightRings[ring] = new CRGBSet(*(gFadeBuffer.rightLeds), led_index, led_index+hw::ringSize[ring]-1);
		led_index += hw::ringSize[ring];
	}

	FastLED.addLeds<CHIPSET, LEFT_EAR_PIN, PIXEL_ORDER>(gLeds.rawLeftLeds, hw::ledsPerEar);
	FastLED.addLeds<CHIPSET, RIGHT_EAR_PIN, PIXEL_ORDER>(gLeds.rawRightLeds, hw::ledsPerEar);

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
	}
	
	// Render a frame of the current animation
	gAnimations[gCurrentAnimation]->Loop(&gLeds);
	FastLED.show();

	EVERY_N_MILLISECONDS(1000) {
		DPRINTF("FPS: %d\n", FastLED.getFPS());
		DPRINTF("a:%0.3f m:%0.3f %d%%\n", gVoltageSamples.getAverage(), gVoltageSamples.getMedian(), currentChargePct(gVoltageSamples.getMedian()));
	}
}