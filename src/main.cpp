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

LedData gLeds;
ezButton gButton(HW_BUTTON_PIN);
RunningMedian gVoltageSamples = RunningMedian(16);

// Array of all available animations
// This is used to cycle through the animations in order
Animation* gAnimations[] = {
	new Cycle(),

	new Acid(LavaColors_p, 128),
	new Acid(OceanColors_p, 128),
	new Acid(PoisonGreen_p),

	new Fuego(FireOrange_h),
	new Fuego(PoisonGreen_h),
	new Fuego(IceBlue_h),

	new Sparkle(Gold_h),
	new Sparkle(CHSV(0,0,255)),
	new Sparkle(),
	
	new TwinkleFOX(FairyLight_p, 128),
	new TwinkleFOX(Ice_p),
	new TwinkleFOX(Snow_p),
	new TwinkleFOX(BlueWhite_p),

	new Hypno(192),

	new Charge(),
};
uint8_t gCurrentAnimation = 0;

/*
 * Calculate the current charge percentage based on battery voltage using
 * a rough approximation of the discharge curve for a LiPo cell.
 */
uint8_t currentChargePct(float voltage)
{
	// Cutoff at 3.5 volts - we're likely under 5% capacity anyway.
	if ( voltage < 3.5 ) { return 0; }
	else if ( voltage > 4.2 ) { return 100; }

	// Approximation of discharge curve from 4.2 -> 3.5 volts
	return (uint8_t)(123 - (123 / (pow(1 + pow((voltage/3.7), 80), 0.165))));
}

/*
 * Check the battery voltage and enter a safety cutoff if it is too low.
 * It will flash a red LED to indicate low voltage and turn off all other LEDs.
 * The controller will stay in this state until it is reset.
 */
void checkVoltageCutoff(void)
{
	if ( gVoltageSamples.getMedian() >= HW_VOLTAGE_CUTOFF ) {
		return;
	}

	// Once we enter voltageCutoff, we stay here for safety until the controller is reset
	DPRINTLN("Triggered voltage cutoff");

	// Turn off all LEDs to conserve power
	FastLED.clear();
	FastLED.show();

	// Flash a dim red LED to indicate low voltage
	bool ledState = false;
	while ( true ) {
		delay(1000);
		ledState = !ledState;
		(*gLeds.leftLeds)[31] = CRGB((ledState?16:0),0,0);
		FastLED.show();
	}
}

/*
 * Setup the controller and initialize the FastLED library.
 * This function is called once at startup.
 */
void setup(void)
{
	Serial.begin(115200);
	while (!Serial && millis() < 2000);

	#ifdef DEBUG
	delay(2000); // failsafe to avoid a bricked controller
	#endif

	// Initialize the FastLED configuration
	FastLED.setMaxPowerInMilliWatts(HW_MAX_POWER_IN_MW);
	FastLED.setBrightness(HW_MAX_BRIGHTNESS);

	// Initialize the FastLED pixel data structures
	gLeds.rawLeftLeds = (CRGB*)malloc(sizeof(CRGB) * HW_LEDS_PER_EAR);
	gLeds.leftLeds = new CRGBSet(gLeds.rawLeftLeds, HW_LEDS_PER_EAR);
	gLeds.rawRightLeds = (CRGB*)malloc(sizeof(CRGB) * HW_LEDS_PER_EAR);
	gLeds.rightLeds = new CRGBSet(gLeds.rawRightLeds, HW_LEDS_PER_EAR);
	gLeds.leftRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * HW_NUM_RINGS);
	gLeds.rightRings = (CRGBSet**)malloc(sizeof(CRGBSet*) * HW_NUM_RINGS);

	// Define CRGBSets for each individual LED ring to simplify certain animations
	uint8_t led_index = 0;
	for( uint8_t ring = 0; ring < HW_NUM_RINGS; ring++ ) {
		gLeds.leftRings[ring] = new CRGBSet(*(gLeds.leftLeds), led_index, led_index+HW_RING_SIZE[ring]-1);
		gLeds.rightRings[ring] = new CRGBSet(*(gLeds.rightLeds), led_index, led_index+HW_RING_SIZE[ring]-1);
		led_index += HW_RING_SIZE[ring];
	}

	// Initialize the FastLED buffers
	FastLED.addLeds<HW_CHIPSET, HW_LEFT_EAR_PIN, HW_PIXEL_ORDER>(gLeds.rawLeftLeds, HW_LEDS_PER_EAR);
	FastLED.addLeds<HW_CHIPSET, HW_RIGHT_EAR_PIN, HW_PIXEL_ORDER>(gLeds.rawRightLeds, HW_LEDS_PER_EAR);
	FastLED.clear();
	FastLED.show();

	// Initialize the interactive button
	gButton.setDebounceTime(50);

	// Prime the voltage monitor and immediately enter the safety cuffoff if we are low volage
	pinMode(HW_VOLTAGE_PIN, INPUT);
	for( uint8_t i = 0; i < gVoltageSamples.getSize(); i++) {
		float reading = 2 * analogReadMilliVolts(HW_VOLTAGE_PIN) / 1000.0; // analog attenuation ratio 1/2, mV --> V
		gVoltageSamples.add(reading); 
	}
	checkVoltageCutoff();

	// Prepare the current animation to run
	gAnimations[gCurrentAnimation]->Setup();
}

/*
 * Main loop of the program. This function is called repeatedly after setup() has completed.
 * It handles sampling the battery voltage, checking for button presses, and rendering the current animation.
 */
void loop(void)
{
	// Sample the battery voltage every second and enter the safety cutoff if we are low voltage
	EVERY_N_SECONDS(1) {
		float reading = 2 * analogReadMilliVolts(HW_VOLTAGE_PIN) / 1000.0; // analog attenuation ratio 1/2, mV --> V
		gVoltageSamples.add(reading);
		checkVoltageCutoff();
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

	EVERY_N_SECONDS(5) {
		uint32_t seconds = millis() / 1000;
		uint32_t minutes = seconds / 60;
		uint32_t hours = minutes / 60;
		seconds = seconds % 60;
		minutes = minutes % 60;

		//DPRINTF("FPS: %d\n", FastLED.getFPS());
		DPRINTF("[%2d:%02d:%02d] %0.3fV (%d%%)\n", hours, minutes, seconds, gVoltageSamples.getMedian(), currentChargePct(gVoltageSamples.getMedian()));
	}
}