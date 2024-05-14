#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

const char* ssid = "";
const char* password = "";
const char* TOKEN = "DISCORD_TOKEN";
const char* CHANNEL_ID = "DISCORD_CHANNEL_ID";

const int PIN_RED   = 23;
const int PIN_GREEN = 22;
const int PIN_BLUE  = 21;

const int PIN_RED_2   = 15;
const int PIN_GREEN_2 = 0;
const int PIN_BLUE_2  = 4;

bool on = false;

void turnOffOnLED(){
  if(!on){
      analogWrite(PIN_RED,   255);
      analogWrite(PIN_GREEN, 0);
      analogWrite(PIN_BLUE,  255);

      analogWrite(PIN_RED_2,   255);
      analogWrite(PIN_GREEN_2, 0);
      analogWrite(PIN_BLUE_2,  255);

      on = true;
    }
    else {
      analogWrite(PIN_RED,   0);
      analogWrite(PIN_GREEN, 0);
      analogWrite(PIN_BLUE,  0);

      analogWrite(PIN_RED_2,   0);
      analogWrite(PIN_GREEN_2, 0);
      analogWrite(PIN_BLUE_2,  0);
      on = false;
    }
}

bool getDelMessages() {
  const char* url = "https://discord.com/api/v10/channels/CHANNEL_ID/messages";
  HTTPClient http;
  http.begin(url);
  http.addHeader("Authorization", "Bot " + String(TOKEN));

  int httpCode = http.GET();
  bool buzzed = false;

  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      if (doc.is<JsonArray>()) {

        HTTPClient http2;
        if (doc.as<JsonArray>().size() > 0) {
          turnOffOnLED();
          Serial.println("here");
        }
        for (const JsonObject& message : doc.as<JsonArray>()) {

          const char* messageId = message["id"];

          if (messageId) {
            Serial.print("Message ID: ");
            Serial.println(messageId);
            String delete_url = "https://discord.com/api/v10/channels/CHANNEL_ID/messages/" + String(messageId);
            http2.begin(delete_url.c_str());
            http2.addHeader("Authorization", "Bot " + String(TOKEN));
            int code = http2.sendRequest("DELETE");
            Serial.printf("" + code);
            http2.end();
          } else {
            Serial.println("No 'id' found in one of the messages.");
          }
        }
      } else {
        Serial.println("Invalid JSON format: Not an array.");
      }
    } else {
      // Print the HTTP error code
      Serial.printf("[HTTP] GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  } else {
    // Failed to connect
    Serial.printf("[HTTP] GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  // Close the connection
  http.end();

  // Return whether the request was successful
  return httpCode == HTTP_CODE_OK;
}


void connectWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi Network ..");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  tone(17, 573, 1000); // connected
  delay(1000);
}

void setup() {
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
  
  pinMode(PIN_RED_2,   OUTPUT);
  pinMode(PIN_GREEN_2, OUTPUT);
  pinMode(PIN_BLUE_2,  OUTPUT);

  Serial.begin(115200);
  connectWifi();
}


void loop() {
  getDelMessages();
  // number of seconds you want is the first one
  delay(5 * 1000);
}
