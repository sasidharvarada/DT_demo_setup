#include "WiFi.h"
// #include "ESPAsyncWebServer.h"
#include "ESPAsyncWebSrv.h"
#include <string.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define PORT 8100
#define BAUD 115200

// #define RELAY_PIN_1 15
// #define RELAY_PIN_2 2
// #define RELAY_PIN_3 0
// #define RELAY_PIN_4 4
// #define RELAY_PIN_5 32
// #define RELAY_PIN_6 33
// #define RELAY_PIN_7 25
// #define RELAY_PIN_8 26

#define RELAY_PIN_1 26
#define RELAY_PIN_2 25
#define RELAY_PIN_3 33
#define RELAY_PIN_4 32
#define RELAY_PIN_5 4
#define RELAY_PIN_6 0
#define RELAY_PIN_7 2
#define RELAY_PIN_8 15

#define CSE_IP          "dev-onem2m.iiit.ac.in"
#define CSE_PORT        443
#define HTTPS           false
#define OM2M_ORGIN     "AirPoll@20:22uHt@Sas"
#define OM2M_MN         "/~/in-cse/in-name/"
#define OM2M_AE         "AE-DT"
#define OM2M_DATA_CONT "Acknowledgement"

String data;
int soil = 0, sand = 0;

WiFiClient client;
HTTPClient http;
// WiFi credentials
const char *ssid = "esw-m19@iiith";
const char *password = "e5W-eMai@3!20hOct";

// space required by the packages recieved
StaticJsonDocument<512> doc;
// hosted on port 8100
AsyncWebServer server(PORT);

void action(char *data_input)
{
    const char *data_in = data_input;
    //Add your function here!!!
}

void wifi_init()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
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
    String conv_data;
    conv_data = m2m_cin_con;
    Serial.print("DATA : ");
    Serial.println(conv_data);
    Serial.println("======== oneM2M DATA PARSED! ==========");

    // If the data required needs to be in the format of string then use 
    // action(conv_data);
    
    //If the data required needs to be in the format of char* then use
    char *c = const_cast<char *>(conv_data.c_str());
    action(c);
    sscanf(conv_data.c_str(), "[%d,%d]", &soil, &sand);

    Serial.print("soil: ");
    Serial.println(soil);
    Serial.print("sand: ");
    Serial.println(sand);
    conv_data.trim();
    if (!conv_data.isEmpty())
    {
      isDataReceived = true;
    }

    // send back a response!
    request->send(200, "application/json", stri);
}

void motor_setup()
{
    pinMode(RELAY_PIN_1, OUTPUT);
    digitalWrite(RELAY_PIN_1, LOW);
    pinMode(RELAY_PIN_2, OUTPUT);
    digitalWrite(RELAY_PIN_2, HIGH);
    pinMode(RELAY_PIN_3, OUTPUT);
    digitalWrite(RELAY_PIN_3, HIGH);
    pinMode(RELAY_PIN_4, OUTPUT);
    digitalWrite(RELAY_PIN_4, LOW);
    pinMode(RELAY_PIN_5, OUTPUT);
    digitalWrite(RELAY_PIN_5, HIGH);
    pinMode(RELAY_PIN_6, OUTPUT);
    digitalWrite(RELAY_PIN_6, HIGH);
}

void case1() //00
{
    Serial.println("1. Turning On Clear flow");

    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, HIGH);
    digitalWrite(RELAY_PIN_3, HIGH);
    digitalWrite(RELAY_PIN_4, LOW);
    digitalWrite(RELAY_PIN_5, HIGH);
    digitalWrite(RELAY_PIN_6, HIGH);

    data = "[" + String(1) + "," + String(0) + "," + String(0) + "," + String(1) + "," + String(0) + "," + String(0) + "]";
}

void case2() //10
{
    Serial.println("2. Turning on soil container");
    digitalWrite(RELAY_PIN_1, HIGH);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_3, LOW);
    digitalWrite(RELAY_PIN_4, LOW);
    digitalWrite(RELAY_PIN_5, HIGH);
    digitalWrite(RELAY_PIN_6, HIGH);

    data = "[" + String(0) + "," + String(1) + "," + String(1) + "," + String(1) + "," + String(0) + "," + String(0) + "]";
}

void case3() //01
{
    Serial.println("3. Turning on sand container");
    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, HIGH);
    digitalWrite(RELAY_PIN_3, HIGH);
    digitalWrite(RELAY_PIN_4, HIGH);
    digitalWrite(RELAY_PIN_5, LOW);
    digitalWrite(RELAY_PIN_6, LOW);

    data = "[" + String(1) + "," + String(0) + "," + String(0) + "," + String(0) + "," + String(1) + "," + String(1) + "]";
}

void case4() //11
{
    Serial.println("4. Turning on sand and soil container");
    digitalWrite(RELAY_PIN_1, HIGH);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_3, LOW);
    digitalWrite(RELAY_PIN_4, HIGH);
    digitalWrite(RELAY_PIN_5, LOW);
    digitalWrite(RELAY_PIN_6, LOW);

    data = "[" + String(0) + "," + String(1) + "," + String(1) + "," + String(0) + "," + String(1) + "," + String(1) + "]";
}

void motor_case()
{
    if (soil == 0 && sand == 0)
    {
        case1();
    }
    else if (soil == 1 && sand == 0)
    {
        case2();
    }
    else if (soil == 0 && sand == 1)
    {
        case3();
    }
    else if (soil == 1 && sand == 1)
    {
        case4();
    }
    else
    {
        Serial.println("Debug: Unexpected case, defaulting to case1");
        case1();
    }
}

void post_onem2m()
{
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

void setup()
{
    Serial.begin(115200);
    wifi_init();    
    server.on(
        "/",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        NULL,
        data_recieve);
    server.begin();
    motor_setup();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();
    }
    if (isDataReceived)
    {
        motor_case();
        post_onem2m();
        isDataReceived = false;
        delay(100);  // Reset the flag
    }
}
