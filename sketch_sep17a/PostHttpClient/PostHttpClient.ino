/**
   PostHTTPClient.ino

    Created on: 21.11.2016

*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SERVER_IP "192.168.0.102:8080"

#ifndef STASSID
#define STASSID "K-WiFi"
#define STAPSK  "9190242449"
#endif

//"пин для датчика CO2
uint8_t pwmPin = D6;
uint8_t LedPin = D7;

// датчик DHT
#define DHTTYPE DHT11   // DHT 11
uint8_t DHTPin = D2; 

// инициализация датчика DHT.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
float CO2;
int prevVal = LOW;
long th, tl, h, l, ppm;

void setup() {

  Serial.begin(115200);
  delay(100);
  pinMode(pwmPin, INPUT);
  pinMode(DHTPin, INPUT);
  pinMode(LedPin, OUTPUT);
  dht.begin();

  Serial.println("Connecting to ");
  Serial.println();
//  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  Temperature = dht.readTemperature(); // получить значение температуры
  Humidity = dht.readHumidity();       // получить значение влажности
Serial.println(Temperature);
Serial.println(Humidity);
  long tt = millis();
  int myVal = digitalRead(pwmPin);

  //Если обнаружили изменение
  if (myVal == HIGH) {
    digitalWrite(LedPin, HIGH);
    if (myVal != prevVal) {
      h = tt;
      tl = h - l;
      prevVal = myVal;
    }
  }  else {
    digitalWrite(LedPin, LOW);
    if (myVal != prevVal) {
      l = tt;
      th = l - h;
      prevVal = myVal;
      ppm = 5000 * (th - 2) / (th + tl - 4);
      Serial.println("PPM = " + String(ppm));
    }
  }
  
  CO2 = ppm;  

  
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/"); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    String tempSTR = "{\"temperature\":";
    String humiditySRT = ",\"humidity\":";
    String co2STR = ",\"co2\":";
    String reqBody = tempSTR + Temperature + humiditySRT + Humidity + co2STR +CO2 + "}";
    Serial.print(reqBody);
    int httpCode = http.POST(reqBody);

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
