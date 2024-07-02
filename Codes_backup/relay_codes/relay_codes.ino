#include <WiFi.h>
#include <HTTPClient.h>

#define RELAY_PIN_1 15
#define RELAY_PIN_2 2
#define RELAY_PIN_3 0
#define RELAY_PIN_4 4
#define RELAY_PIN_5 32
#define RELAY_PIN_6 33
#define RELAY_PIN_7 25
#define RELAY_PIN_8 26

const char *ssid = "SCRC_LAB_IOTT";
const char *password = "Scrciiith@123";

#define CSE_IP    "10.3.1.117"
#define CSE_PORT  8200
#define HTTPS  false
#define OM2M_ORGIN  "admin:admin"
#define OM2M_MN  "/~/in-cse/in-name/"
#define OM2M_AE   "AE-DT"
#define OM2M_DATA_CONT "DT-1/Data"

WiFiClient client;
HTTPClient http;
String data;
void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  // pinMode(13, OUTPUT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH);  
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_2, HIGH);  
  pinMode(RELAY_PIN_3, OUTPUT);
  digitalWrite(RELAY_PIN_3, HIGH);  
  pinMode(RELAY_PIN_4, OUTPUT);
  digitalWrite(RELAY_PIN_4, HIGH);  
  pinMode(RELAY_PIN_5, OUTPUT);
  digitalWrite(RELAY_PIN_5, HIGH);  
  pinMode(RELAY_PIN_6, OUTPUT);
  digitalWrite(RELAY_PIN_6, HIGH);  
}

void case1() //00
{
  Serial.println("1. Turning On Clear flow");

  digitalWrite(RELAY_PIN_1, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_1, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_2, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_3, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_4, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_5, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_6, HIGH);  
  delay(1000);

  data = "[" + String(1) + "," + String(0) + "," + String(0) + "," + String(1) + "," + String(0) + "," + String(0) + "]";
}

void case2() //10
{
  Serial.println("2. Turning on soil container");
  digitalWrite(RELAY_PIN_1, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_2, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_3, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_4, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_5, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_6, HIGH);  
  delay(1000);

  data = "[" + String(0) + "," + String(1) + "," + String(1) + "," + String(1) + "," + String(0) + "," + String(0) + "]";

}

void case3() //01
{
  Serial.println("3. Turning on sand container");
  digitalWrite(RELAY_PIN_1, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_2, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_3, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_4, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_5, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_6, LOW);  
  delay(1000);
  
  data = "[" + String(1) + "," + String(0) + "," + String(0) + "," + String(0) + "," + String(1) + "," + String(1) + "]";
}

void case4() //11
{
  Serial.println("4. Turning on sand and soil container");
  digitalWrite(RELAY_PIN_1, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_2, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_3, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_4, HIGH);  
  delay(1000);
  digitalWrite(RELAY_PIN_5, LOW);  
  delay(1000);
  digitalWrite(RELAY_PIN_6, LOW);  
  delay(1000);

  data = "[" + String(0) + "," + String(1) + "," + String(1) + "," + String(0) + "," + String(1) + "," + String(1) + "]";
}
void post_onem2m() {
    
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

void loop() 
{
  case1();
  case2();
  case3();
  case4();
  post_onem2m();
  delay(3000);
}