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

#define CSE_IP "dev-onem2m.iiit.ac.in"
#define CSE_PORT 443
#define HTTPS false
#define OM2M_ORGIN "AirPoll@20:22uHt@Sas"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "AE-DT"
// #define OM2M_DATA_CONT "Node-1/Data"
// #define OM2M_DATA_CONT "Node-2/Data"
#define OM2M_DATA_CONT "Node-3/Data"

void setup() {
  Serial.begin(115200);
  tempSensor.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void post_onem2m() {
  String data = "[" + String(Temp) +  " , " + String(tdsValue_without_temp) + " , " + String(tdsValue) + " , " + String(Voltage) +"]";

  String server = "http://" + String(CSE_IP) + ":" + String(CSE_PORT) + String(OM2M_MN);

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
  http.end();
  Serial.println("HTTP Response Code: " + String(code));
  delay(1000);
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    tdssensorValue = analogRead(tdssensorPin);
    Voltage = tdssensorValue * 3.3 / 1024.0;

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
    
    tempSensor.requestTemperaturesByIndex(0);
    Temp = tempSensor.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.print(Temp);
    Serial.println(" °C");
    
    post_onem2m();
    // delay(600000);
    delay(5000);
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
