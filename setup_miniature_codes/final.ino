#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include <string.h>
#include <ArduinoJson.h>
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
void lightblueScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition);
void brownScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition);
void showBlueOnStrip(int stripIndex, int pinStart, int pinEnd);

#define PORT 8100
#define BAUD 115200
    String a, b, c;
// WiFi credentials
const char *ssid = "myssid";
const char *password = "password";

// Space required by the packages received
StaticJsonDocument<512> doc;
// Hosted on port 8100
AsyncWebServer server(PORT);

void blueScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition) {
    if (pinEnd >= NUM_LEDS_PER_STRIP) {
        pinEnd = NUM_LEDS_PER_STRIP - 1;
    }

    int rangeLength = pinEnd - pinStart + 1;
    int offPosition1 = (scrollPosition) % rangeLength;
    int offPosition2 = (scrollPosition + 1) % rangeLength;

    // Set all LEDs in the range to blue
    for (int i = pinStart; i <= pinEnd; i++) {
        leds[stripIndex][i] = CRGB::Blue;
    }

    // Turn off two specific LEDs to create the scrolling effect
    if (offPosition1 < rangeLength) {
        leds[stripIndex][pinStart + offPosition1] = CRGB::Black;
    }
    if (offPosition2 < rangeLength) {
        leds[stripIndex][pinStart + offPosition2] = CRGB::Black;
    }

    FastLED.show();
}

void lightblueScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition) {
    if (pinEnd >= NUM_LEDS_PER_STRIP) {
        pinEnd = NUM_LEDS_PER_STRIP - 1;
    }

    int rangeLength = pinEnd - pinStart + 1;
    int offPosition1 = (scrollPosition) % rangeLength;
    int offPosition2 = (scrollPosition + 1) % rangeLength;

    // Set all LEDs in the range to brown
    for (int i = pinStart; i <= pinEnd; i++) {
        leds[stripIndex][i] = CRGB(135, 206, 235); // light blue color
    }

    // Turn off two specific LEDs to create the scrolling effect
    if (offPosition1 < rangeLength) {
        leds[stripIndex][pinStart + offPosition1] = CRGB::Black;
    }
    if (offPosition2 < rangeLength) {
        leds[stripIndex][pinStart + offPosition2] = CRGB::Black;
    }

    FastLED.show();
}

void brownScrolling(int stripIndex, int pinStart, int pinEnd, int scrollPosition) {
    if (pinEnd >= NUM_LEDS_PER_STRIP) {
        pinEnd = NUM_LEDS_PER_STRIP - 1;
    }

    int rangeLength = pinEnd - pinStart + 1;
    int offPosition1 = (scrollPosition) % rangeLength;
    int offPosition2 = (scrollPosition + 1) % rangeLength;

    // Set all LEDs in the range to brown
    for (int i = pinStart; i <= pinEnd; i++) {
        leds[stripIndex][i] = CRGB(165, 42, 42); // Brown color
    }

    // Turn off two specific LEDs to create the scrolling effect
    if (offPosition1 < rangeLength) {
        leds[stripIndex][pinStart + offPosition1] = CRGB::Black;
    }
    if (offPosition2 < rangeLength) {
        leds[stripIndex][pinStart + offPosition2] = CRGB::Black;
    }

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

// Function to turn off a range of LEDs on a specific strip
void turnOffLedRange(int stripIndex, int startLed, int endLed) {
    // Ensure startLed and endLed are within the bounds
    if (stripIndex >= 0 && stripIndex < NUM_STRIPS && startLed >= 0 && endLed < NUM_LEDS_PER_STRIP && startLed <= endLed) {
        // Set the specified range of LEDs to black (off)
        for (int i = startLed; i <= endLed; i++) {
            leds[stripIndex][i] = CRGB::Black;  // Turn off LED
        }
        FastLED.show();  // Update the LED strip
    } else {
        Serial.println("Invalid stripIndex, startLed, or endLed");
    }
}


void action(String a, String b, String c)
{
    if (a == "1" && b == "0" && c == "0") {
      static int scrollPosition = 0;

      turnOffLedRange(1, 23, 29);
      turnOffLedRange(2, 0, 21);
      turnOffLedRange(3, 15, 21);
      turnOffLedRange(4, 0, 26);

      blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip
      blueScrolling(1, 0, 22, scrollPosition); // Solid blue on strip 2 from pin 0 to 15  
      blueScrolling(3, 0, 14, scrollPosition); // Solid blue on strip 2 from pin 0 to 15
      blueScrolling(5, 0, 8, scrollPosition); // Solid blue on strip 2 from pin 0 to 15

      digitalWrite(motor, HIGH);
      digitalWrite(solenoid_1, HIGH);
      digitalWrite(solenoid_2_3, LOW);
      digitalWrite(solenoid_4, HIGH);
      digitalWrite(solenoid_5, LOW);
      digitalWrite(solenoid_6, LOW); 

      scrollPosition++;  
      delay(75);  
    }
    else if (a == "1" && b == "0" && c == "1") {
      static int scrollPosition = 0;

      turnOffLedRange(1, 23, 29);
      turnOffLedRange(2, 0, 21);
      turnOffLedRange(5, 0, 4);

      blueScrolling(0, 0, 11, scrollPosition);  
      blueScrolling(1, 0, 22, scrollPosition); 
      blueScrolling(3,0,21,scrollPosition); 
      blueScrolling(4,0,13,scrollPosition);
      lightblueScrolling(4,10,26,scrollPosition); 
      lightblueScrolling(5,5,8,scrollPosition);  

      digitalWrite(motor, HIGH);
      digitalWrite(solenoid_1, HIGH);
      digitalWrite(solenoid_2_3, LOW);
      digitalWrite(solenoid_4, LOW);
      digitalWrite(solenoid_5, HIGH);
      digitalWrite(solenoid_6, HIGH);

      scrollPosition++;  
      delay(75);  
    }
    else if (a == "1" && b == "1" && c == "0") {
      static int scrollPosition = 0;

      turnOffLedRange(3, 0, 4);
      turnOffLedRange(3, 15, 21);
      turnOffLedRange(4, 0, 26);

      blueScrolling(0, 0, 11, scrollPosition);  
      blueScrolling(1, 0, 29, scrollPosition); 
      blueScrolling(2,0,7,scrollPosition);
      brownScrolling(2,8,22,scrollPosition);
      brownScrolling(3,5,14,scrollPosition);
      brownScrolling(5,0,8,scrollPosition); 

      digitalWrite(motor, HIGH);
      digitalWrite(solenoid_1, LOW);
      digitalWrite(solenoid_2_3, HIGH);
      digitalWrite(solenoid_4, HIGH);
      digitalWrite(solenoid_5, LOW);
      digitalWrite(solenoid_6, LOW);

      scrollPosition++;  
      delay(75);  // Control the scroll speed 
    }
    else if (a == "1" && b == "1" && c == "1") {
      /////////////////////////////CASE : [ 1,1,1]/////////////////////////////////////////////////////////
      static int scrollPosition = 0;

      blueScrolling(0, 0, 11, scrollPosition);  
      blueScrolling(1, 0, 29, scrollPosition); 
      blueScrolling(2,0,7,scrollPosition);
      brownScrolling(2,8,21,scrollPosition);
      brownScrolling(3,5,21,scrollPosition);
      brownScrolling(4,0,13,scrollPosition);
      lightblueScrolling(4,10,26,scrollPosition); 
      lightblueScrolling(5,5,8,scrollPosition);  

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
    else if (a == "0" && b == "0" && c == "0") {
          /////////////////////////////CASE : [0,0,0]/////////////////////////////////////////////////////////
          static int scrollPosition = 0;

          turnOffLedRange(1, 0, 29);
          turnOffLedRange(2, 0, 21);
          turnOffLedRange(3, 0, 21);
          turnOffLedRange(4, 0, 26);
          turnOffLedRange(5, 0, 8);

          blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip

          digitalWrite(motor, LOW);
          digitalWrite(solenoid_1, HIGH);
          digitalWrite(solenoid_2_3, LOW);
          digitalWrite(solenoid_4, HIGH);
          digitalWrite(solenoid_5, LOW);
          digitalWrite(solenoid_6, LOW);

          scrollPosition++; 
          delay(75);  
    }
    else if (a == "0" && b == "1" && c == "0") {
          /////////////////////////////CASE : [0,0,0]/////////////////////////////////////////////////////////
          static int scrollPosition = 0;

          turnOffLedRange(1, 0, 29);
          turnOffLedRange(2, 0, 21);
          turnOffLedRange(3, 0, 21);
          turnOffLedRange(4, 0, 26);
          turnOffLedRange(5, 0, 8);

          blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip

          digitalWrite(motor, LOW);
          digitalWrite(solenoid_1, LOW);
          digitalWrite(solenoid_2_3, HIGH);
          digitalWrite(solenoid_4, HIGH);
          digitalWrite(solenoid_5, LOW);
          digitalWrite(solenoid_6, LOW);

          scrollPosition++; 
          delay(75);  
    }
    else if (a == "0" && b == "0" && c == "1") {
          /////////////////////////////CASE : [0,0,0]/////////////////////////////////////////////////////////
          static int scrollPosition = 0;

          turnOffLedRange(1, 0, 29);
          turnOffLedRange(2, 0, 21);
          turnOffLedRange(3, 0, 21);
          turnOffLedRange(4, 0, 26);
          turnOffLedRange(5, 0, 8);

          blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip

          digitalWrite(motor, LOW);
          digitalWrite(solenoid_1, HIGH);
          digitalWrite(solenoid_2_3, LOW);
          digitalWrite(solenoid_4, LOW);
          digitalWrite(solenoid_5, HIGH);
          digitalWrite(solenoid_6, HIGH);

          scrollPosition++; 
          delay(75);  
    }
    else if (a == "0" && b == "1" && c == "1") {
          /////////////////////////////CASE : [0,0,0]/////////////////////////////////////////////////////////
          static int scrollPosition = 0;

          turnOffLedRange(1, 0, 29);
          turnOffLedRange(2, 0, 21);
          turnOffLedRange(3, 0, 21);
          turnOffLedRange(4, 0, 26);
          turnOffLedRange(5, 0, 8);

          blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip

          digitalWrite(motor, LOW);
          digitalWrite(solenoid_1, LOW);
          digitalWrite(solenoid_2_3, HIGH);
          digitalWrite(solenoid_4, LOW);
          digitalWrite(solenoid_5, HIGH);
          digitalWrite(solenoid_6, HIGH);

          scrollPosition++; 
          delay(75);  
    }
    else {
          /////////////////////////////CASE : [0,0,0]/////////////////////////////////////////////////////////
          static int scrollPosition = 0;

          turnOffLedRange(1, 0, 29);
          turnOffLedRange(2, 0, 21);
          turnOffLedRange(3, 0, 21);
          turnOffLedRange(4, 0, 26);
          turnOffLedRange(5, 0, 8);

          blueScrolling(0, 0, 11, scrollPosition);  // Blue scrolling on strip 0 across the entire strip

          digitalWrite(motor, LOW);
          digitalWrite(solenoid_1, LOW);
          digitalWrite(solenoid_2_3, LOW);
          digitalWrite(solenoid_4, LOW);
          digitalWrite(solenoid_5, LOW);
          digitalWrite(solenoid_6, LOW);

          scrollPosition++; 
          delay(75);  
    }
}

void wifi_init() {
    unsigned long startAttemptTime = millis();  // Record the start time

    WiFi.begin(ssid, password);

    // Try connecting for 30 seconds
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);

        // Check if 30 seconds have passed
        if (millis() - startAttemptTime >= 30000) {
            Serial.println("\nFailed to connect to the WiFi network. Restarting...");
            ESP.restart();  // Restart the ESP32 after 30 seconds
        }
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}


bool isDataReceived = false;

void data_recieve(AsyncWebServerRequest *request, unsigned char *data, size_t len, size_t index, size_t total)
{
    String stri;
    Serial.print("\n");
    for (int i = 0; i < len; i++)
    {
        stri += (char)data[i];
    }
    Serial.print(stri);
    Serial.print("\n");

    DeserializationError error = deserializeJson(doc, stri);

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    JsonObject m2m_cin = doc["m2m:sgn"]["m2m:nev"]["m2m:rep"]["m2m:cin"];
    const char *m2m_cin_con = m2m_cin["con"];
    String conv_data = m2m_cin_con;
    Serial.print("DATA : ");
    Serial.println(conv_data);
    Serial.println("======== oneM2M DATA PARSED! ==========");

    // Split the received string (e.g., "1,0,0") into separate parts for a, b, and c

    int firstComma = conv_data.indexOf(',');
    int secondComma = conv_data.indexOf(',', firstComma + 1);

    a = conv_data.substring(0, firstComma);
    b = conv_data.substring(firstComma + 1, secondComma);
    c = conv_data.substring(secondComma + 1);

    // Print values of a, b, c
    Serial.print("a=");
    Serial.print(a);
    Serial.print(", b=");
    Serial.print(b);
    Serial.print(", c=");
    Serial.println(c);

    action(a, b, c);

    // Send back a response
    request->send(200, "application/json", stri);
}

void setup()
{
    Serial.begin(115200);
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

  showBlueOnStrip(0, 0, 11); 
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

    wifi_init();
    server.on(
        "/",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        NULL,
        data_recieve);
    server.begin();
    Serial.println("started server");

}

unsigned long lastUpdate = 0;
const unsigned long scrollInterval = 75;  // Interval for updating the scrolling effect

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate >= scrollInterval) {
        lastUpdate = currentMillis;

        // Update the scrolling positions
        if (a.length() > 0 && b.length() > 0 && c.length() > 0) {
            action(a, b, c);
        }
    }
}
