#include <DHT.h>
#include <DHT_U.h>

#include <Adafruit_Sensor.h>

#define DHTPIN 2 // Тот самый номер пина
DHT dht(DHTPIN, DHT11); //Инициация датчика

#define PIN_PHOTO_SENSOR A3
void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
   int val = analogRead(PIN_PHOTO_SENSOR);
  Serial.println(val);
  delay(2000);

  delay(2000); // 2 секунды задержки
  float h = dht.readHumidity(); //Измеряем влажность
  float t = dht.readTemperature(); //Измеряем температуру
  if (isnan(h) || isnan(t)) {  // Проверка. Если не удается считать показания, выводится «Ошибка считывания», и программа завершает работу
    Serial.println("Ошибка считывания");
    return;
  }
  Serial.print("Влажность: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Температура: ");
  Serial.print(t);
  Serial.println(" *C "); //Вывод показателей на экран
}
