// vim: ft=arduino
/*
    note: need add library Adafruit_BMP280 from library manage
*/

#include <Arduino.h>
#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h> //The DHT12 uses I2C comunication.
#include <Adafruit_BMP280.h>

DHT12 dht12; //Preset scale CELSIUS and ID 0x5c.
Adafruit_BMP280 bme;

#define HISTORY 10

float temperature, humidity, pressure;
float temperature_sum, humidity_sum, pressure_sum;

float temperature_hist[HISTORY];
float humidity_hist[HISTORY];
float pressure_hist[HISTORY];


void setup() {
    M5.begin();
    M5.Power.begin();
    Wire.begin();

    M5.Lcd.setBrightness(20);

    Serial.println(F("ENV Unit(DHT12 and BMP280) test..."));

    while (!bme.begin(0x76)){
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
    }
    M5.Lcd.clear(BLACK);
    M5.Lcd.println("ENV Unit test...");

    for (int i = 0; i < 10; i++) {
        temperature = dht12.readTemperature();
        humidity = dht12.readHumidity();
        pressure = bme.readPressure() / 100;

        temperature_hist[i] = temperature;
        humidity_hist[i] = humidity;
        pressure_hist[i] = pressure;

        delay(50);
    }
}

void loop() {
    temperature = dht12.readTemperature();
    humidity = dht12.readHumidity();
    pressure = bme.readPressure() / 100;
    Serial.printf("READ: T: %2.2f*C  H: %0.2f%%  P: %0.2fhPa\r\n", temperature, humidity, pressure);

    temperature_sum = 0;
    humidity_sum = 0;
    pressure_sum = 0;

    for (int i = 1; i < HISTORY; i++) {
        temperature_sum += temperature_hist[i];
        temperature_hist[i-1] = temperature_hist[i];

        humidity_sum += humidity_hist[i];
        humidity_hist[i-1] = humidity_hist[i];

        pressure_sum += pressure_hist[i];
        pressure_hist[i-1] = pressure_hist[i];

        Serial.printf("[%d]: T: %f, H: %f, P: %f\r\n", i, temperature_hist[i-1], humidity_hist[i-1], pressure_hist[i-1]);
    }

    temperature_sum += temperature;
    humidity_sum += humidity;
    pressure_sum += pressure;

    temperature_hist[HISTORY-1] = temperature;
    humidity_hist[HISTORY-1] = humidity;
    pressure_hist[HISTORY-1] = pressure;

    temperature = round(10 * temperature_sum / HISTORY) / 10;
    humidity = round(10 * humidity_sum / HISTORY) / 10;
    pressure = round(10 * pressure_sum / HISTORY) / 10;

    Serial.printf("CALC: T: %2.2f*C  H: %0.2f%%  P: %0.2fhPa\r\n", temperature, humidity, pressure);

    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(4);
    M5.Lcd.printf("T: %2.1fC  \r\nH: %2.0f%%  \r\nP: %2.0fhPa  \r\n", temperature, humidity, pressure);

    delay(10000);
}
