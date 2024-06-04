#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <ESP32Time.h>

volatile int count = 0;                // Variable modified in an ISR
volatile int interruptCount = 0;       // Variable to count interrupts
volatile bool countIncreased = false;  // Flag for indicating count increase

WiFiClient client;
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
ESP32Time rtc(0);

#define MIN_VALID_TIME 1672531200
#define MAX_VALID_TIME 2082844799

static uint64_t epochTime = 0;
#define CSE_IP "onem2m.iiit.ac.in"
#define OM2M_ORGIN "wdmon@20:gpod@llk4" //prod server id pass
#define CSE_PORT 443  // ESP8266 usually doesn't support HTTPS without additional setup
#define HTTPS false
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "AE-WM/WM-WF"

// const char* ssid = "SCRC_LAB_IOT";
// const char* password = "Scrciiith@123";

// #define interruptPin 23 // GPIO23
// #define OM2M_DATA_CONT "WM-WF-KH98-40/Data"
// String url = "http://onem2m.iiit.ac.in:443/~/in-cse/in-name/AE-WM/WM-WF/WM-WF-KH98-40/Data/la";

#define interruptPin 14 // GPIO14
#define OM2M_DATA_CONT "WM-WF-KH95-40/Data"
String url = "http://onem2m.iiit.ac.in:443/~/in-cse/in-name/AE-WM/WM-WF/WM-WF-KH95-40/Data/la";

const char* ssid = "TP-Link_20D1";
const char* password = "79069571";

#define POST_INTERVAL 60000  // Interval for posting data (1 minute)

unsigned long lastPostTime = 0;  // Variable to store the time of the last post
unsigned long prev_time = 0;     // Variable to store the last time update

// Calibration factor (counts per kiloliter)
const int CALIBRATION_FACTOR = 100;

// Function to calculate flow rate from count
float calculateFlowRate(int count1, int count2) {
    return ((count2 - count1) / (float)CALIBRATION_FACTOR) * 60.0;  // Convert flow rate to kiloliters per minute
}

void IRAM_ATTR handleInterrupt() {
  interruptCount++;                // Increment interrupt count when the interrupt occurs
  if (interruptCount >= 1000) {
    count++;                // Increment count every 1000 interrupts
    interruptCount = 0;     // Reset interrupt count
    countIncreased = true;  // Set the flag to indicate count increase
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                          // Use shorter delay to avoid excessive waiting
    if (millis() - startTime > 30000) {  // 30-second timeout
      Serial.println("WiFi connection failed.");
      return;
    }
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  sync_time();
}

void sync_time() {
  static bool first_time = true;
  int num_tries = 5;  // Number of attempts to get valid NTP time

  // Check if the ESP32 is connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {
    if (first_time == true) {
      // Initialize NTPClient only once
      timeClient.begin();
      timeClient.setTimeOffset(0);
      timeClient.update();
      first_time = false;
    }
    // Configure the time zone (IST: Indian Standard Time, UTC+5:30)
    configTime(5 * 3600, 30 * 60, "pool.ntp.org");
    while (num_tries > 0) {

      // Get the obtained NTP time
      uint64_t ntp_time = timeClient.getEpochTime();
      Serial.println(ntp_time);
      // Validate the obtained NTP time
      if (ntp_time != 0 && ntp_time >= MIN_VALID_TIME && ntp_time <= MAX_VALID_TIME) {
        // Set the RTC time using the NTP time
        rtc.setTime(ntp_time);
        epochTime = rtc.getEpoch();
        // Print the universal time and epoch time
        Serial.print("Universal time: " + rtc.getDateTime(true) + "\t");
        Serial.println(epochTime, DEC);  // For UTC, take timeoffset as 0
        // Update the last_update time
        prev_time = millis();
        return;  // Exit the function after successful synchronization
      } else {
        // Invalid NTP time, reduce the number of remaining attempts
        Serial.println("Invalid NTP time, retrying...");
        num_tries--;
        delay(1000);
        if (num_tries == 0) {
          Serial.println("All attempts failed, restarting time function...");
          // sync_time();  // Try syncing time again
          ESP.restart();
        }
      }
    }
  }
}

void sendGET(String url) {
  HTTPClient http;
  http.begin(client, url);  // Use client and url as parameters
  http.addHeader("X-M2M-Origin", "iiith_guest:iiith_guest");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
    int firstBracketIndex = payload.indexOf('[');
    int lastBracketIndex = payload.lastIndexOf(']');
    String data = payload.substring(firstBracketIndex + 1, lastBracketIndex);
    int firstCommaIndex = data.indexOf(',');
    int secondCommaIndex = data.indexOf(',', firstCommaIndex + 1);
    String thirdValue = data.substring(secondCommaIndex + 1);
    thirdValue.trim();
    count = thirdValue.toInt(); // Assign the third value to the count variable

    Serial.print("Count: ");
    Serial.println(count);

  } else {
    Serial.print("HTTP request failed with error code: ");
    Serial.println(httpCode);
    Serial.println("Restarting ESP32...");
    ESP.restart();
  }

  http.end();
}

void post_onem2m(uint64_t epochTime, float flowRate) {
  String server = (HTTPS ? "https://" : "http://") + String(CSE_IP) + ":" + String(CSE_PORT) + String(OM2M_MN);
  http.begin(client, server + OM2M_AE + "/" + OM2M_DATA_CONT);
  http.addHeader("X-M2M-Origin", OM2M_ORGIN);
  http.addHeader("Content-Type", "application/json;ty=4");

  String data = "["+ String(epochTime) +
                "," + String(flowRate) +
                "," + String(count) +"]";

  String req_data = String() + "{\"m2m:cin\": {"
                    + "\"con\": \"" + data + "\","
                    + "\"cnf\": \"text\""
                    + "}}";

  Serial.println("Server URL: " + server);
  Serial.println("Request Data: " + req_data);

  int responseCode = http.POST(req_data);
  Serial.println("HTTP Response Code: " + String(responseCode));

  if (responseCode == 400) {
    String response = http.getString();
    Serial.println("Server Response: " + response);
  }

  http.end();
}

void setup() {
  Serial.begin(115200);  // Set Serial communication speed

  pinMode(interruptPin, INPUT_PULLUP);                                             // Configure interrupt pin with pull-up resistor
  attachInterrupt(interruptPin, handleInterrupt, FALLING);  // Set interrupt

  connectToWiFi();  // Connect to WiFi

  // Send GET request to obtain the initial count value
  sendGET(url);

  // // Sync time
  // sync_time();
}

void loop() {
  unsigned long currentTime = millis();

  if (!countIncreased && currentTime - lastPostTime >= POST_INTERVAL) {  // Time to post data
    lastPostTime = currentTime;                                          // Update last post time
    
    // Get current count from the server
    sendGET(url);
    int count1 = count;  // Store the current count
    
    // Wait for 60 seconds
    delay(60000);
    
    // Get count again after 60 seconds
    sendGET(url);
    int count2 = count;  // Store the count after 60 seconds
    
    // Calculate flow rate
    float flowRate = calculateFlowRate(count1, count2);

    // Get current time from NTP server
    sync_time();  // Get epoch time in seconds

    // Post data to OneM2M
    post_onem2m(epochTime, flowRate);
  }

  if (countIncreased) {  // Only print when count has increased
    Serial.print("Count: ");
    Serial.println(count);
    countIncreased = false;  // Reset the flag
  }
}
