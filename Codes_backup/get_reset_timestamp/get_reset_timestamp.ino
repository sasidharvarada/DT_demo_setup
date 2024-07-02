#include <WiFi.h>
#include "OneWire.h"
#include "DallasTemperature.h"
#include <HTTPClient.h>

#define SERIAL Serial

const char* ssid = "SCRC_LAB_IOT";
const char* password = "Scrciiith@123";

OneWire oneWire(27);
DallasTemperature tempSensor(&oneWire);
#define tdssensorPin 35

int tdssensorValue = 0;
float tdsValue = 0;
float Voltage = 0;
float Temp = 0;
float tdsValue_without_temp = 0;

WiFiClient client;
HTTPClient http;

#define CSE_IP    "10.3.1.117"
#define CSE_PORT  8200
#define HTTPS  false
#define OM2M_ORGIN  "admin:admin"
#define OM2M_MN  "/~/in-cse/in-name/"
#define OM2M_AE   "AE-DT"
#define OM2M_DATA_CONT "Node-1/Data"
// #define OM2M_DATA_CONT "Node-2/Data"
// #define OM2M_DATA_CONT "Node-3/Data"

String reset_value = "0";
String conValue = "0";

long prev_time = 0;
long new_time = 0;

void setup() {
  Serial.begin(115200);
  tempSensor.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED || WiFi.status() == WL_CONNECT_FAILED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to " + String(ssid) + " network");
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
  }
}

String sendGET(String url) {
  HTTPClient http;
  http.setTimeout(10000); // Set a timeout of 10 seconds (adjust as needed)
  http.begin(url);
  // http.addHeader("X-M2M-Origin", "dev_guest:dev_guest");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("--------------------------------- check onem2m after 10 sec ----------------------------------------------");
    // Serial.println(payload);
    // Find the "con" field within the payload
    int conStart = payload.indexOf("\"con\" : ");
    if (conStart >= 0) {
      int conEnd = payload.indexOf("\n", conStart);
      if (conEnd >= 0) {
        String conValue = payload.substring(conStart + 8, conEnd);  // Extract "con" value
        conValue.trim();                                     // Remove leading and trailing spaces
        Serial.print("OneM2M value : ");
        Serial.println(conValue);

        // Remove square brackets
        conValue = conValue.substring(1, conValue.length() - 1);

        int firstCommaIndex = conValue.indexOf(',');

        String timestamp = conValue.substring(1, firstCommaIndex);
        reset_value = conValue.substring(firstCommaIndex + 1, conValue.length() - 1);

        new_time = atol(timestamp.c_str());
        Serial.print("new: ");
        Serial.println(new_time);
        Serial.print("prev: ");
        Serial.println(prev_time);

        Serial.println("reset values is " + String(reset_value) + ".");
      }
    }
  } else {
    Serial.print("HTTP request failed with error code: ");
    Serial.println(httpCode);
  }

  http.end();
  return reset_value;
}

void tds_temp() {
  tdssensorValue = analogRead(tdssensorPin);
  Voltage = tdssensorValue * 3.3 / 1024.0;

  tempSensor.requestTemperaturesByIndex(0);
  Temp = tempSensor.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(Temp);
  Serial.println(" °C");

  float compensationCoefficient = 1.0 + 0.02 * (Temp - 25.0);
  float compensationVoltage = Voltage / compensationCoefficient;
  tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
    - 255.86 * compensationVoltage * compensationVoltage
    + 857.39 * compensationVoltage) * 0.5;

  Serial.print("TDS Value (with Temp Compensation) = ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  float compensationCoefficient_without_temp = 1.0 + 0.02 * (25.0 - 25.0);
  float compensationVoltage_without_temp = Voltage / compensationCoefficient_without_temp;
  tdsValue_without_temp = (133.42 * compensationVoltage_without_temp * compensationVoltage_without_temp
    - 255.86 * compensationVoltage_without_temp * compensationVoltage_without_temp
    + 857.39 * compensationVoltage_without_temp) * 0.5;

  Serial.print("TDS Value (without Temp Compensation) = ");
  Serial.print(tdsValue_without_temp);
  Serial.println(" ppm");
}

void post_onem2m() {
  String data = "[" + String(Temp) + " , " + String(tdsValue_without_temp) + " , " + String(tdsValue) + " , " + String(Voltage) + "]";

  String server = "http://" + String(CSE_IP) + ":" + String(CSE_PORT) + String(OM2M_MN);

  if (WiFi.status() == WL_CONNECTED) {
    http.begin(client, server + OM2M_AE + "/" + OM2M_DATA_CONT);
    http.addHeader("X-M2M-Origin", OM2M_ORGIN);
    http.addHeader("Content-Type", "application/json;ty=4");

    String req_data = String() + "{\"m2m:cin\": {"
      + "\"con\": \"" + data + "\","
      + "\"rn\": \"" + "cin_" + String(millis()) + "\","
      + "\"cnf\": \"text\""
      + "}}";

    Serial.println("Server URL: " + server);
    Serial.println("Request Data: " + req_data);

    int code = http.POST(req_data);
    Serial.println(code);
    Serial.println("HTTP Response Code: " + String(code));
    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // String url = "http://dev-onem2m.iiit.ac.in:443/~/in-cse/in-name/AE-WM/WM-WD/WM-WD-AD98-00/Data/la";
    String url = "http://10.3.1.117:8200/~/in-cse/in-name/AE-DT/Node-1/Actuation/la";
    // String url = "http://10.3.1.117:8200/~/in-cse/in-name/AE-DT/Node-2/Actuation";
   

 // String url = "http://10.3.1.117:8200/~/in-cse/in-name/AE-DT/Node-3/Actuation";

    conValue = sendGET(url);
    Serial.println("reset values are " + String(reset_value) + ".");
  if (new_time == prev_time) {
    Serial.println("Matched");
  } else {
    Serial.println("Not Matched");
    if (reset_value == "1") {
      Serial.print("Esp will restart");
      ESP.restart();
      reset_value = "0";
      prev_time = new_time;
    }
  }
    tds_temp();
    post_onem2m();
    // delay(60000);
    delay(1000);
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Reconnected to WiFi!");
  }
}