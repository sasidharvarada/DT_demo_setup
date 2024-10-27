#include <FastLED.h>

#define LED_PIN     27      // Pin connected to the Data line of WS2812B
#define NUM_LEDS    300     // Total number of LEDs in the strip
#define BRIGHTNESS  50      // Reduced maximum brightness of LEDs
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Function declarations
void waterFlowing(int startLED, int endLED, int scrollPosition);
void noColorSection(int startLED, int endLED);
void valveOn(int startLED, int endLED);
void impureWater(int startLED, int endLED, int scrollPosition);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  runPattern();
}

void runPattern() {
  static int scrollPosition = 0;  // Scrolling position for water flowing sections

  // Water flowing (300 to 293)
  waterFlowing(294, 300, scrollPosition);

  // Valve on (292 to 291)
  valveOn(291, 292);

  // Water flowing (290 to 271)
  waterFlowing(271, 290, scrollPosition);

  // Valve on (270)
  valveOn(270, 270);
  // Valve on (248)
  valveOn(248, 248);

  // Water flowing (269 to 262)
  waterFlowing(262, 269, scrollPosition);

  // No color section (261 to 257)
  noColorSection(257, 261);

  // Impure water (240 to 247)
  impureWater(249, 256, scrollPosition);

  // Impure water (240 to 247)
  impureWater(240, 247, scrollPosition);

  // Show the LED updates
  FastLED.show();

  // Increment the scroll position for the next frame
  scrollPosition++;
  if (scrollPosition >= 20) {
    scrollPosition = 0;  // Reset after one full loop
  }

  delay(100); // Adjust the speed of the scrolling effect
}

// Function to handle water flowing section
void waterFlowing(int startLED, int endLED, int scrollPosition) {
  int range = endLED - startLED + 1;
  int offPosition1 = scrollPosition % range;
  int offPosition2 = (scrollPosition + 1) % range;

  CRGB colorToUse = CRGB(35,137,118); // water color

  for (int i = endLED; i >= startLED; i--) {
    leds[i] = colorToUse; // Set the selected color
  }
  leds[endLED - offPosition1] = CRGB::Black;
  leds[endLED - offPosition2] = CRGB::Black;
}

// Function to handle sections with no color (off)
void noColorSection(int startLED, int endLED) {
  for (int i = startLED; i <= endLED; i++) {
    leds[i] = CRGB::Black;
  }
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

  CRGB colorToUse = CRGB(139, 69, 19); // Brown color

  for (int i = endLED; i >= startLED; i--) {
    leds[i] = colorToUse; // Set the selected color
  }
  leds[endLED - offPosition1] = CRGB::Black;
  leds[endLED - offPosition2] = CRGB::Black;
}
