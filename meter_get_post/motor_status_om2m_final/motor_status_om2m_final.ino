#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Time.h>

// #define MAIN_SSID "SCRC_LAB_IOT"
// #define MAIN_PASS "Scrciiith@123"

#define MAIN_SSID "TP-Link_20D1"
#define MAIN_PASS "79069571"

#define CSE_IP          "onem2m.iiit.ac.in"
#define CSE_PORT        443
#define HTTPS           false
#define OM2M_ORIGIN     "DeviceMon@20:9G&6OnuL1iZ"
#define OM2M_MN         "/~/in-cse/in-name/"
#define OM2M_AE         "AE-DM"
#define OM2M_DATA_CONT  "DM-KH98-60/Data"

WiFiClient client;
HTTPClient http;

int code ;

unsigned long prev_time;
unsigned long last_update;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com");
ESP32Time rtc(0);

#define MIN_VALID_TIME 1672531200
#define MAX_VALID_TIME 2082844799

static uint64_t ntp_epoch_time = 0;
#if defined(ESP32)
#define PZEM_RX_PIN     16
#define PZEM_TX_PIN     17
#define PZEM_SERIAL     Serial2
#define CONSOLE_SERIAL  Serial
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);
#endif

String status = ""; // String to store status (ON/OFF)
float voltage = 0;
float current = 0;
float power = 0;
float energy = 0;
float frequency = 0;
float pf = 0;

void initWiFi() {
  WiFi.begin(MAIN_SSID, MAIN_PASS);
  Serial.println("Connecting to WiFi");
  unsigned long startTime = millis(); // Get the current time in milliseconds

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
    Serial.print('.');
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());    
    Serial.print("Mac Address :");
    Serial.println(WiFi.macAddress());
    long rssi = WiFi.RSSI();
    Serial.print("Signal strength (RSSI): ");
    Serial.print(rssi);
  } else {
    Serial.print("Mac Address :");
    Serial.println(WiFi.macAddress());
    Serial.println("\nFailed to connect to WiFi within 30 seconds. Restarting...");
    ESP.restart(); // Restart the device
  }
}

//................................. time setup...........................................................................................................///////

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
      Serial.print("ntp_time:");
      Serial.println(ntp_time);
      // Validate the obtained NTP time
      if (ntp_time != 0 && ntp_time >= MIN_VALID_TIME && ntp_time <= MAX_VALID_TIME) {
        // Set the RTC time using the NTP time
        rtc.setTime(ntp_time);
        ntp_epoch_time = rtc.getEpoch();
        // Serial.println(ntp_epoch_time);
        if(ntp_epoch_time < 1672000000){
          sync_time();
        }
        // Print the universal time and epoch time
          Serial.print("Universal time: " + rtc.getDateTime(true) + "\t");
          Serial.println(ntp_epoch_time, DEC); // For UTC, take timeoffset as 0
          // Serial.println("");
        // Update the last_update time
        prev_time = millis();
        return;  // Exit the function after successful synchronization
      } else {
        // Invalid NTP time, reduce the number of remaining attempts
        Serial.println("Invalid NTP time, retrying...");
        num_tries--;
        delay(1000);  
        Serial.println("All attempts failed, performing restarting time function ...");
        ESP.restart();   
      }
    }
  }
}

void setup() {
    // Debugging Serial port
    Serial.begin(115200);
    WiFi.begin(MAIN_SSID, MAIN_PASS);
    initWiFi();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

void current_data() {
    // Print the custom address of the PZEM
    Serial.print("Custom Address: ");
    Serial.println(pzem.readAddress(), HEX);

    // Read the data from the sensor
    voltage = pzem.voltage();
    current = pzem.current();
    power = pzem.power();
    energy = pzem.energy();
    frequency = pzem.frequency();
    pf = pzem.pf();

    // Determine the status based on the address

    // status = (pzem.readAddress() == 1) ? "ON" : "OFF";
    // status = (pzem.readAddress() == 1) ? "1" : "0";

    // Determine the status based on the current

    // status = (current > 0.0) ? "ON" : "OFF";
    status = (current > 0.0) ? "1" : "0";



    // Check if the data is valid
    // if (isnan(voltage) || isnan(current) || isnan(power) || isnan(energy) || isnan(frequency) || isnan(pf)) {
    //     Serial.println("Error reading data from PZEM sensor");
    //     return;
    // }
    // Check if any data is invalid, if yes, set them to 0 and print error message
    if (isnan(voltage) || isnan(current) || isnan(power) || isnan(energy) || isnan(frequency) || isnan(pf)) {
    voltage = 0; current = 0; power = 0; energy = 0; frequency = 0; pf = 0;
    Serial.println("Error reading data from PZEM sensor");
    }

    // Print the values to the Serial console
    Serial.println("Readings from PZEM sensor:");
    Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    Serial.print("Current: "); Serial.print(current); Serial.println("A");
    Serial.print("Power: "); Serial.print(power); Serial.println("W");
    Serial.print("Energy: "); Serial.print(energy, 3); Serial.println("kWh");
    Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    Serial.print("Power Factor: "); Serial.println(pf);
}

void post_om2m() {
    // String data = "[" + String(ntp_epoch_time) + String(address) + "," + String(voltage) + "," + String(current) + "," + String(power) + "," + String(energy, 3) + "," + String(frequency, 1) + "," + String(pf) + "]";
    String data = "[" + String(ntp_epoch_time) + "," + status + "," + String(voltage) + "," + String(current) + "," + String(power) + "," + String(energy, 3) + "," + String(frequency, 1) + "," + String(pf) + "]";
    String server = "http";
    if (HTTPS) {
        server += "s";
    }
    server += "://" + String(CSE_IP) + ":" + String(CSE_PORT) + String(OM2M_MN);

    http.begin(client, server + OM2M_AE + "/" + OM2M_DATA_CONT);
    http.addHeader("X-M2M-Origin", OM2M_ORIGIN);
    http.addHeader("Content-Type", "application/json;ty=4");

    String req_data = String() + "{\"m2m:cin\": {"
                      + "\"con\": \"" + data + "\","
                      + "\"rn\": \"" + "cin_" + String(millis()) + "\","
                      + "\"cnf\": \"text\""
                      + "}}";

    Serial.println("Server URL: " + server);
    Serial.println("Request Data: " + req_data);
    code = http.POST(req_data);
    Serial.println(code);
    http.end();
    delay(1000);
    http.addHeader("Connection", "close");
}

void loop() {
    sync_time();  
    current_data();
    post_om2m();

// int maxAttempts = 10;
// for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
//     post_om2m();
//     delay(5000);

//     if (code == 201) {
//         break; // Exit the loop when statusCode is 201
//     }

//     if (attempt == maxAttempts) {
//         Serial.println("Restart"); // Print "reset" after 10 attempts
//         delay(1000);
//         ESP.restart(); // Restart the device
//         }                       // Reset the microcontroller   
//  }
 delay(3000);
}  
