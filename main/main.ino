///* Установите здесь свои SSID и пароль */
//const char* ssid = "K-WiFi";   // SSID
//const char* password = "9190242449";  // пароль

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "MHZ19.h"

//"пин для датчика CO2
uint8_t pwmPin = D6;
//uint8_t LedPin = D7;
// Раскомментируйте одну из строк ниже в зависимости от того, какой датчик вы используете!
#define DHTTYPE DHT11   // DHT 11
#define PIN_PHOTO_SENSOR A0   //Датчик освещенности
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

/* Введите свои SSID и пароль */
const char* ssid = "K-WiFi";  // SSID
const char* password = "9190242449"; // пароль

ESP8266WebServer server(80);

// датчик DHT
uint8_t DHTPin = D2; 
               
// инициализация датчика DHT.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;
float CO2;
float Light;
int prevVal = LOW;
long th, tl, h, l, ppm;
 
void setup() 
{
  Serial.begin(115200);
  delay(100);
  pinMode(pwmPin, INPUT);
  pinMode(DHTPin, INPUT);
//  pinMode(LedPin, OUTPUT);

  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // подключаемся к локальной wi-fi сети
  WiFi.begin(ssid, password);

  // проверить, подключился ли wi-fi модуль к wi-fi сети
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
  
void loop() 
{
  server.handleClient();
}

void handle_OnConnect() 
{
  Temperature = dht.readTemperature(); // получить значение температуры
  Humidity = dht.readHumidity();       // получить значение влажности
  
  int lightning = analogRead(PIN_PHOTO_SENSOR);  //получить значение освещенности

  long tt = millis();
  int myVal = digitalRead(pwmPin);

  //Если обнаружили изменение
  if (myVal == HIGH) {
    if (myVal != prevVal) {
      h = tt;
      tl = h - l;
      prevVal = myVal;
    }
  }  else {
    if (myVal != prevVal) {
      l = tt;
      th = l - h;
      prevVal = myVal;
      ppm = 5000 * (th - 2) / (th + tl - 4);
//      Serial.println("PPM = " + String(ppm));
    }
  }
  
  CO2 = ppm;       // получить значение углекислого
  Light = lightning;   // получить значение освещенности
  server.send(200, "text/html", SendHTML(Temperature,Humidity,CO2,Light)); 
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat,float Humiditystat, float CO2stat, float Lightstat)
{
  String ptr = "{\"temperature\":";
  ptr +=(int)Temperaturestat;
  ptr += ",\"humidity\":";
  ptr +=(int)Humiditystat;
  ptr += ",\"co2\":";
  ptr +=(int)CO2stat;
  ptr += ",\"light\":";
  ptr += (int)Lightstat;
  ptr += "}";
  
//  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
//  ptr +="<title>ESP8266 Weather Report</title>\n";
//  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
//  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
//  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
//  ptr +="</style>\n";
//  ptr +="</head>\n";
//  ptr +="<body>\n";
//  ptr +="<div id=\"webpage\">\n";
//  ptr +="<h1>ESP8266 NodeMCU Weather Report</h1>\n";
//  
//  ptr +="<p>Temperature: ";
//  ptr +=(int)Temperaturestat;
//  ptr +="°C</p>";
//  ptr +="<p>Humidity: ";
//  ptr +=(int)Humiditystat;
//  ptr +="%</p>";
//  ptr +="<p>CO2: ";
//  ptr +=(int)CO2stat;
//  ptr +="ppm</p>";
//  ptr +="<p>Light: ";
//  ptr +=(int)Lightstat;
//  ptr +="Lk</p>";
//  
//  ptr +="</div>\n";
//  ptr +="</body>\n";
//  ptr +="</html>\n";
  return ptr;
}
