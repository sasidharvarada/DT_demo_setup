#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define interruptPin D3  // Led in NodeMCU at pin GPIO16 (D3).

const char* ssid = "esw-m19@iiith";
const char* password = "e5W-eMai@3!20hOct";

volatile int count = 0;                // Variable modified in an ISR
volatile bool countIncreased = false;  // Flag for indicating count increase

WiFiClient client;
HTTPClient http;

#define CSE_IP "dev-onem2m.iiit.ac.in"
#define OM2M_ORGIN "Tue_20_12_22:Tue_20_12_22"
#define CSE_PORT 443  // ESP8266 usually doesn't support HTTPS without additional setup
#define HTTPS false
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "AE-DT"

// #define OM2M_DATA_CONT  "WM-Node-1/Data"
// String url = "http://dev-onem2m.iiit.ac.in:443/~/in-cse/in-name/AE-DT/WM-Node-1/Data/la";

#define OM2M_DATA_CONT "WM-Node-2/Data"
String url = "http://dev-onem2m.iiit.ac.in:443/~/in-cse/in-name/AE-DT/WM-Node-2/Data/la";

#define POST_INTERVAL 60000  // Interval for posting data (1 minute)

unsigned long lastPostTime = 0;  // Variable to store the time of the last post

void ICACHE_RAM_ATTR handleInterrupt() {
  count++;                // Increment count when the interrupt occurs
  countIncreased = true;  // Set the flag to indicate count increase
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
}

void sendGET(String url) {
  HTTPClient http;
  http.begin(client, url);  // Use client and url as parameters
  http.addHeader("X-M2M-Origin", "dev_guest:dev_guest");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
    // Serial.println("---------------------------------Payload----------------------------------------------");
    // Serial.println(payload);

    // Find the "con" field within the payload
    int conStart = payload.indexOf("[");          // Find the start of the count value
    int conEnd = payload.indexOf("]", conStart);  // Find the end of the count value
    if (conStart >= 0 && conEnd >= 0) {
      String conValue = payload.substring(conStart + 1, conEnd);  // Extract numeric value within the square brackets
      conValue.trim();                                            // Remove leading and trailing spaces
      Serial.print("Extracted value (con): ");
      Serial.println(conValue);

      // Set the extracted value as the count
      count = conValue.toInt();  // Convert string to integer and set it to count
      Serial.print("count: ");
      Serial.println(count);
    }
  } else {
    Serial.print("HTTP request failed with error code: ");
    Serial.println(httpCode);
    // If GET request fails, restart ESP8266
    Serial.println("Restarting ESP8266...");
    ESP.restart();
  }

  http.end();
}


void post_onem2m() {
  String data = "[" + String(count) + "]";

  String server = (HTTPS ? "https://" : "http://") + String(CSE_IP) + ":" + String(CSE_PORT) + String(OM2M_MN);
  http.begin(client, server + OM2M_AE + "/" + OM2M_DATA_CONT);
  http.addHeader("X-M2M-Origin", OM2M_ORGIN);
  http.addHeader("Content-Type", "application/json;ty=4");

  String req_data = String() + "{\"m2m:cin\": {"
                    + "\"con\": \"" + data + "\","
                    + "\"cnf\": \"text\""
                    + "}}";

  Serial.println("Server URL: " + server);
  Serial.println("Request Data: " + req_data);

  int responseCode = http.POST(req_data);
  Serial.println("HTTP Response Code: " + String(responseCode));
  http.end();
}

void setup() {
  Serial.begin(115200);  // Set Serial communication speed

  pinMode(interruptPin, INPUT_PULLUP);                                             // Configure interrupt pin with pull-up resistor
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);  // Set interrupt

  connectToWiFi();  // Connect to WiFi

  // Send GET request to obtain the initial count value
  sendGET(url);
}

void loop() {
  unsigned long currentTime = millis();

  if (!countIncreased && currentTime - lastPostTime >= POST_INTERVAL) {  // Time to post data
    lastPostTime = currentTime;                                          // Update last post time
    post_onem2m();                                                       // Post data to the server
  }

  if (countIncreased) {  // Only print when count has increased
    Serial.print("Count: ");
    Serial.println(count);
    countIncreased = false;  // Reset the flag
  }

  // Add any additional logic here
}
