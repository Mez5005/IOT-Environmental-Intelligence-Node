#include <WiFi.h>
#include <HTTPClient.h>
// Name of WIFI and pass, server name and the copy of the api key from thingspeak node
const char* ssid     = "Mez";           
const char* password = "mezo_5005"; 
const char* serverName = "http://api.thingspeak.com/update";
const char* apiKey     = "JATCNDV64XD6FWVS"; 

// Pin 34 is Input-Only, which works perfectly for strictly receiving data (RX)
#define RX2_PIN 34
#define TX2_PIN -1  // Disables the transmit pin since it's not needed

void setup() {
  Serial.begin(115200);   
  
  // Custom routing Serial2 to use Pin 34 for listening
  Serial2.begin(115200, SERIAL_8N1, RX2_PIN, TX2_PIN);  
  
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
}

void loop() {
  if (Serial2.available() > 0) {
    String incomingData = Serial2.readStringUntil('\n');
    incomingData.trim();
    
    if (incomingData.startsWith("[") && incomingData.endsWith("]")) {
      incomingData = incomingData.substring(1, incomingData.length() - 1);
      
      int comma1 = incomingData.indexOf(',');
      int comma2 = incomingData.indexOf(',', comma1 + 1);
      int comma3 = incomingData.indexOf(',', comma2 + 1);
      
      String temp = incomingData.substring(0, comma1);
      String hum  = incomingData.substring(comma1 + 1, comma2);
      String gas  = incomingData.substring(comma2 + 1, comma3);
      String ph   = incomingData.substring(comma3 + 1);
      
      Serial.printf("Received Data -> Temp: %s | Hum: %s | Gas: %s | pH: %s\n", temp.c_str(), hum.c_str(), gas.c_str(), ph.c_str());
      
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(serverName) + "?api_key=" + apiKey 
                     + "&field1=" + temp + "&field2=" + hum + "&field3=" + gas + "&field4=" + ph;
        http.begin(url);
        int httpResponseCode = http.GET();
        Serial.print(">>> Website Sync Status: ");
        Serial.println(httpResponseCode); 
        http.end();
      }
    }
  }
}