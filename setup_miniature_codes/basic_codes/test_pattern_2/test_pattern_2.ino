#include <FastLED.h>

#define LED_PIN     27      // Pin connected to the Data line of WS2812B
#define NUM_LEDS    300     // Total number of LEDs in the strip
#define BRIGHTNESS  50      // Reduced maximum brightness of LEDs
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Function declarations
void waterFlowing(int startLED, int endLED, int scrollPosition);
void valveOn(int startLED, int endLED);
void impureWater(int startLED, int endLED, int scrollPosition);
void turnOffLEDs(); // Function to turn off all LEDs

unsigned long previousMillisValve = 0; // Timer variable for valves
unsigned long previousMillisScroll = 0; // Timer variable for scrolling
const unsigned long valveDelay = 2000; // 2-second delay for the valve
const unsigned long scrollDelay = 100; // Delay for the scrolling effect
const unsigned long impureWaterDelay = 2000; // 2-second delay for impure water

int scrollPosition = 0; // Scrolling position for water flowing sections
int state = 0;  // State machine for controlling sequence

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  unsigned long currentMillis = millis();

  // Scrolling should always happen, so it's updated independently
  if (currentMillis - previousMillisScroll >= scrollDelay) {
    scrollPosition++;
    if (scrollPosition >= 20) {
      scrollPosition = 0;  // Reset after one full loop
    }
    previousMillisScroll = currentMillis; // Update last scroll time
  }

  // Main state machine for the valve control and water flow
  switch (state) {
    case 0:
      // First water flow (294 to 300)
      waterFlowing(294, 300, scrollPosition);
      if (currentMillis - previousMillisValve >= valveDelay) {
        previousMillisValve = currentMillis;
        state = 1;
      }
      break;

    case 1:
      // Turn on valve (291 to 292) and wait 2 seconds
      valveOn(291, 292);
      waterFlowing(294, 300, scrollPosition);
      if (currentMillis - previousMillisValve >= valveDelay) {
        previousMillisValve = currentMillis;
        state = 2;
      }
      break;

    case 2:
      // Second water flow (290 to 271)
      waterFlowing(271, 290, scrollPosition);
      if (currentMillis - previousMillisValve >= valveDelay) {
        previousMillisValve = currentMillis;
        state = 3;
      }
      break;

    case 3:
      // Turn on both valves (270 and 248) and wait 2 seconds
      valveOn(270, 270);
      valveOn(248, 248);
      waterFlowing(294, 300, scrollPosition);
      waterFlowing(271, 290, scrollPosition); // Keep water flowing
      if (currentMillis - previousMillisValve >= valveDelay) {
        previousMillisValve = currentMillis;
        state = 4;
      }
      break;

    case 4:
      // Keep water flowing for a while before showing impure water
      valveOn(270, 270);
      valveOn(248, 248);
      waterFlowing(294, 300, scrollPosition);
      waterFlowing(271, 290, scrollPosition); // Keep water flowing
      waterFlowing(262, 269, scrollPosition);
      if (currentMillis - previousMillisValve >= valveDelay) {
        previousMillisValve = currentMillis;
        state = 5;  // Move to impure water state
      }
      break;

    case 5:
      // Show impure water after a 2-second delay
      impureWater(249, 256, scrollPosition);
      impureWater(240, 247, scrollPosition);
      
      if (currentMillis - previousMillisValve >= impureWaterDelay) {
        previousMillisValve = currentMillis; // Update the timer for the next sequence
        turnOffLEDs();  // Turn off all LEDs before restarting the sequence
        delay(500); // Add a delay of 500 ms for visual feedback before restarting
        state = 0;  // Loop back to the beginning
      }
      break;
  }

  // Show the LED updates
  FastLED.show();
}

// Function to handle water flowing section
void waterFlowing(int startLED, int endLED, int scrollPosition) {
  int range = endLED - startLED + 1;
  int offPosition1 = scrollPosition % range;
  int offPosition2 = (scrollPosition + 1) % range;

  CRGB colorToUse = CRGB(35, 137, 118); // water color

  for (int i = endLED; i >= startLED; i--) {
    leds[i] = colorToUse; // Set the selected color
  }
  leds[endLED - offPosition1] = CRGB::Black;
  leds[endLED - offPosition2] = CRGB::Black;
}

// Function to turn on valve LEDs (static red LEDs)
void valveOn(int startLED, int endLED) {
  for (int i = startLED; i <= endLED; i++) {
    leds[i] = CRGB(200, 0, 0); // Custom dim red color
  }
}

// Function to handle impure water (scrolling effect)
void impureWater(int startLED, int endLED, int scrollPosition) {
  int range = endLED - startLED + 1;
  int offPosition1 = scrollPosition % range;
  int offPosition2 = (scrollPosition + 1) % range;

  CRGB colorToUse = CRGB(139, 69, 19); // Brown color for impure water

  for (int i = endLED; i >= startLED; i--) {
    leds[i] = colorToUse; // Set the selected color
  }
  leds[endLED - offPosition1] = CRGB::Black;
  leds[endLED - offPosition2] = CRGB::Black;
}

// Function to turn off all LEDs
void turnOffLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black; // Turn off each LED
  }
}
