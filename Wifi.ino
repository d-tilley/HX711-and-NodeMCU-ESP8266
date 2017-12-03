#include "HX711.h"
#include <ESP8266WiFi.h>

// Values obtained from the SparkFun_HX711_Calibration sketch
#define calibration_factor -9400
#define zero_factor 183979

// Define our data and clock pins
#define DOUT 4 // D2 maps to GPIO4
#define CLK 5  // D1 maps to GPIO5

// Initialize our HX711 interface
HX711 scale(DOUT, CLK);

// WiFi network name and password
const char* ssid = "****";
const char* password = "****";

const char* server = "api.thingspeak.com";

WiFiClient client;

void setup() {
    Serial.begin(115200);
 
    WiFi.begin(ssid, password);

    Serial.println();
    Serial.print("Connecting");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("success!");
    Serial.print("IP Address is: ");
    Serial.println(WiFi.localIP());

    scale.set_scale(calibration_factor);
    scale.set_offset(zero_factor);
}

void loop() {
    scale.power_up();
  
    float data = scale.get_units();
    sendWeight(data);

    scale.power_down();
  
    Serial.println(data, 2);
    delay(60000);
}

int sendWeight(float data) {
    if (client.connect(server, 80)) {
        String apiKey = "****";
        
        String body = "field1=" + String(data, 2);

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(body.length());
        client.print("\n\n");
        client.print(body);
        client.print("\n\n");
    }

    client.stop();
}
