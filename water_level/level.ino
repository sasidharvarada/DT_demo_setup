#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "SCRC_LAB_IOT";
const char* password = "Scrciiith@123";
// #define echoPin 3
// #define trigPin 1
#define ONE_WIRE_BUS 26

#define echoPin 19
#define trigPin 18

int C = 0;
long duration;
int distance;

WiFiClient client;
HTTPClient http;

#define CSE_IP "dev-onem2m.iiit.ac.in"
#define OM2M_ORGIN "Tue_20_12_22:Tue_20_12_22"
#define CSE_PORT 443  // ESP8266 usually doesn't support HTTPS without additional setup
#define HTTPS false
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "AE-DT"
#define OM2M_DATA_CONT  "WL/Data"

String resource = "https://api.thingspeak.com/update?";
String apiKeyValue = "P3Q2F4MBVTPW0QGI";


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void readSensors(float& tempC, int& distance) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2;

  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
}

void post_onem2m(float distance, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    String data = "[" + String(distance) + "," + String(temperature) + "]";

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
  } else {
    Serial.println("WiFi Disconnected");
    ESP.restart();
  }
}

void postThingSpeak(float distance, float temperature) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = resource + "api_key=" + apiKeyValue + "&field1=" + String(distance) + "&field2=" + String(temperature);
    http.begin(url.c_str());
    int httpResponseCode = http.GET();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode == 200) {
      Serial.println("Data successfully pushed to ThingSpeak");
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
    ESP.restart();
  }
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
  Serial.println("Distance measurement using JSN-SR04T");
  delay(500);

  connectWiFi();
  sensors.begin();
}

void loop() {
  float tempC;
  int distance;
  readSensors(tempC, distance);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (tempC != DEVICE_DISCONNECTED_C) {
    Serial.println("Temperature: ");
    Serial.println(tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  delay(800);
  C++;

  if (C == 15) {
    post_onem2m(distance, tempC);
    postThingSpeak(distance, tempC);
    C = 0;
  }
}
