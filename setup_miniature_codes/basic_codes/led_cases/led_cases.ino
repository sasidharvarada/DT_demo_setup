#include <FastLED.h>

// Define the number of LED strips and LEDs per strip
#define NUM_STRIPS   6
#define NUM_LEDS_PER_STRIP 30
#define BRIGHTNESS   100
#define LED_TYPE     WS2812B
#define COLOR_ORDER  GRB

// Define the pins for each LED strip
#define LED_PIN_1    14
#define LED_PIN_2    27
#define LED_PIN_3    26
#define LED_PIN_4    25
#define LED_PIN_5    33
#define LED_PIN_6    32

const int motor = 4;
const int solenoid_1 = 16;
const int solenoid_2_3 = 17;
const int solenoid_4 = 5;
const int solenoid_5 = 18;
const int solenoid_6 = 19;

// Array to hold the LED data for each strip
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

// Function prototypes
void blueScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition);
void brownScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition);
void showBlueOnStrip(int stripIndex, int pinStart, int pinEnd);

void setup() {
  // Initialize each strip with the corresponding pin
  FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LED_TYPE, LED_PIN_3, COLOR_ORDER>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LED_TYPE, LED_PIN_4, COLOR_ORDER>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LED_TYPE, LED_PIN_5, COLOR_ORDER>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<LED_TYPE, LED_PIN_6, COLOR_ORDER>(leds[5], NUM_LEDS_PER_STRIP);

  pinMode(motor, OUTPUT);
  pinMode(solenoid_1, OUTPUT);
  pinMode(solenoid_2_3, OUTPUT);
  pinMode(solenoid_4, OUTPUT);
  pinMode(solenoid_5, OUTPUT);
  pinMode(solenoid_6, OUTPUT);

  FastLED.setBrightness(BRIGHTNESS);

  turnOffLedRange(0, 0, 10);
  turnOffLedRange(1, 0, 29);
  turnOffLedRange(2, 0, 21);
  turnOffLedRange(3, 0, 21);
  turnOffLedRange(4, 0, 26);
  turnOffLedRange(5, 0, 8);

  digitalWrite(motor, LOW);
  digitalWrite(solenoid_1, LOW);
  digitalWrite(solenoid_2_3, LOW);
  digitalWrite(solenoid_4, LOW);
  digitalWrite(solenoid_5, LOW);
  digitalWrite(solenoid_6, LOW);

}

void loop() {  
    static int scrollPosition = 0;
/////////////////////////////CASE : [0,0,0]/////////////////////////////////////////////////////////

  // turnOffLedRange(1, 0, 29);
  // turnOffLedRange(2, 0, 21);
  // turnOffLedRange(3, 0, 21);
  // turnOffLedRange(4, 0, 26);
  // turnOffLedRange(5, 0, 8);

  // blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip

  // digitalWrite(motor, LOW);
  // digitalWrite(solenoid_1, LOW);
  // digitalWrite(solenoid_2_3, LOW);
  // digitalWrite(solenoid_4, LOW);
  // digitalWrite(solenoid_5, LOW);
  // digitalWrite(solenoid_6, LOW);



/////////////////////////////CASE : [1,0,0]/////////////////////////////////////////////////////////


  // turnOffLedRange(1, 23, 29);
  // turnOffLedRange(2, 0, 21);
  // turnOffLedRange(3, 15, 21);
  // turnOffLedRange(4, 0, 26);

  // blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip
  // blueScrolling(1, 0, 22, scrollPosition); // Solid blue on strip 2 from pin 0 to 15  
  // blueScrolling(3, 0, 14, scrollPosition); // Solid blue on strip 2 from pin 0 to 15
  // blueScrolling(5, 0, 8, scrollPosition); // Solid blue on strip 2 from pin 0 to 15

  // digitalWrite(motor, HIGH);
  // digitalWrite(solenoid_1, HIGH);
  // digitalWrite(solenoid_2_3, LOW);
  // digitalWrite(solenoid_4, HIGH);
  // digitalWrite(solenoid_5, LOW);
  // digitalWrite(solenoid_6, LOW);

/////////////////////////////CASE : [1,1,0]/////////////////////////////////////////////////////////

  // turnOffLedRange(3, 0, 4);
  // turnOffLedRange(4, 0, 26);

  // blueScrolling(0, 0, 11, scrollPosition);  
  // blueScrolling(1, 0, 29, scrollPosition); 
  // blueScrolling(2,0,7,scrollPosition);
  // brownScrolling(2,8,22,scrollPosition);
  // brownScrolling(3,5,14,scrollPosition);
  // brownScrolling(5,0,8,scrollPosition); 

  // digitalWrite(motor, HIGH);
  // digitalWrite(solenoid_1, LOW);
  // digitalWrite(solenoid_2_3, HIGH);
  // digitalWrite(solenoid_4, HIGH);
  // digitalWrite(solenoid_5, LOW);
  // digitalWrite(solenoid_6, LOW);

/////////////////////////////CASE : [1,0,1]/////////////////////////////////////////////////////////

  // turnOffLedRange(1, 23, 29);
  // turnOffLedRange(2, 0, 21);
  // turnOffLedRange(5, 0, 4);

  // blueScrolling(0, 0, 11, scrollPosition);  
  // blueScrolling(1, 0, 22, scrollPosition); 
  // blueScrolling(3,0,21,scrollPosition);
  // blueScrolling(4,0,26,scrollPosition);
  // blueScrolling(5,5,8,scrollPosition); 

  // digitalWrite(motor, HIGH);
  // digitalWrite(solenoid_1, HIGH);
  // digitalWrite(solenoid_2_3, LOW);
  // digitalWrite(solenoid_4, LOW);
  // digitalWrite(solenoid_5, HIGH);
  // digitalWrite(solenoid_6, HIGH);

/////////////////////////////CASE : [ 1,1,1]/////////////////////////////////////////////////////////

  blueScrolling(0, 0, 11, scrollPosition);  
  blueScrolling(1, 0, 29, scrollPosition); 
  blueScrolling(2,0,7,scrollPosition);
  brownScrolling(2,8,21,scrollPosition);
  brownScrolling(3,5,21,scrollPosition);
  brownScrolling(4,0,13,scrollPosition);
  blueScrolling(4,10,26,scrollPosition); 
  blueScrolling(5,5,8,scrollPosition);

  turnOffLedRange(3, 0, 4);
  turnOffLedRange(5, 0, 4);

  digitalWrite(motor, HIGH);  
  digitalWrite(solenoid_1, LOW);
  digitalWrite(solenoid_2_3, HIGH);
  digitalWrite(solenoid_4, LOW);
  digitalWrite(solenoid_5, HIGH);
  digitalWrite(solenoid_6, HIGH);

  scrollPosition++;  // Increment the scrolling position for animation
  delay(75);  // Control the scroll speed
}

// Light Blue scrolling effect for a specific LED strip
void blueScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition) {
  // Ensure pinEnd is within the bounds
  if (pinEnd >= NUM_LEDS_PER_STRIP) {
    pinEnd = NUM_LEDS_PER_STRIP - 1;
  }
  
  // Calculate the off positions based on the scroll position
  int rangeLength = pinEnd - pinStart + 1;
  int offPosition1 = scrollPosition % rangeLength;
  int offPosition2 = (scrollPosition + 1) % rangeLength;

  // Set all LEDs in the range to blue
  for (int i = pinStart; i <= pinEnd; i++) {
    leds[stripIndex][i] = CRGB::Blue;
  }

  // Turn off two specific LEDs to create the scrolling effect
  leds[stripIndex][pinStart + offPosition1] = CRGB::Black;
  leds[stripIndex][pinStart + offPosition2] = CRGB::Black;

  FastLED.show();
}

// Brown scrolling effect for a specific LED strip using the same logic
void brownScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition) {
  // Ensure pinEnd is within the bounds
  if (pinEnd >= NUM_LEDS_PER_STRIP) {
    pinEnd = NUM_LEDS_PER_STRIP - 1;
  }

  // Calculate the off positions based on the scroll position
  int rangeLength = pinEnd - pinStart + 1;
  int offPosition1 = scrollPosition % rangeLength;
  int offPosition2 = (scrollPosition + 1) % rangeLength;

  // Set all LEDs in the range to brown
  for (int i = pinStart; i <= pinEnd; i++) {
    leds[stripIndex][i] = CRGB(165, 42, 42); // Brown color
  }

  // Turn off two specific LEDs to create the scrolling effect
  leds[stripIndex][pinStart + offPosition1] = CRGB::Black;
  leds[stripIndex][pinStart + offPosition2] = CRGB::Black;

  FastLED.show();
}

// Function to show solid blue on a specific strip within a range
void showBlueOnStrip(int stripIndex, int pinStart, int pinEnd) {
  // Ensure pinEnd is within the bounds
  if (pinEnd >= NUM_LEDS_PER_STRIP) {
    pinEnd = NUM_LEDS_PER_STRIP - 1;
  }

  // Set LEDs in the range between pinStart and pinEnd to blue
  for (int i = pinStart; i <= pinEnd; i++) {
    leds[stripIndex][i] = CRGB(0, 0, 255); // Set blue color
  }

  FastLED.show();
  delay(5);  // Keep blue LEDs on for a while
}

// Turn off a specific range of LEDs for a given strip
void turnOffLedRange(int stripIndex, int startLed, int endLed) {
  for (int i = startLed; i <= endLed; i++) {
    leds[stripIndex][i] = CRGB::Black;
  }
  FastLED.show();
}
