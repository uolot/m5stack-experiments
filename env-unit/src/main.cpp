// vim: ft=arduino
/*
    note: need add library Adafruit_BMP280 from library manage
*/

#include <Arduino.h>
#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h> //The DHT12 uses I2C comunication.
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <MQTT.h>
#include "settings.h"


/* const char *ssid = "3sTK Hyperoptic 1Gb Fibre 2.4Ghz"; */
/* const char *password = "25sMPZwLGwTRZ"; */
const char *ssid = WLAN_SSID;
const char *password = WLAN_PASSWORD;

DHT12 dht12; //Preset scale CELSIUS and ID 0x5c.
Adafruit_BMP280 bme;
WiFiClient wifi;
MQTTClient mqtt;

#define HISTORY 10

float temperature, humidity, pressure;
float temperature_sum, humidity_sum, pressure_sum;

float temperature_hist[HISTORY];
float humidity_hist[HISTORY];
float pressure_hist[HISTORY];

char buffer[10];


void connectWLAN() {
    Serial.printf("Connecting to %s\r\n", ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void connectMQTT() {
    Serial.print("\nconnecting...");
    while (!mqtt.connect("m5stack", MQTT_USERNAME, MQTT_PASSWORD)) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    /* mqtt.subscribe("/hello"); */
    // mqtt.unsubscribe("/hello");
}

void populateSensorHistory() {
    while (!bme.begin(0x76)){
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    }

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

void setup() {
    M5.begin();
    M5.Power.begin();
    Wire.begin();
    WiFi.begin(ssid, password);
    mqtt.begin(MQTT_HOST, MQTT_PORT, wifi);

    Serial.println("Hello.");

    connectWLAN();
    connectMQTT();
    populateSensorHistory();

    M5.Lcd.setBrightness(20);
    M5.Lcd.clear();
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

        /* Serial.printf("[%d]: T: %f, H: %f, P: %f\r\n", i, temperature_hist[i-1], humidity_hist[i-1], pressure_hist[i-1]); */
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

    if (!mqtt.connected()) {
        connectWLAN();
        connectMQTT();
    }
    sprintf(buffer, "%f", temperature);
    mqtt.publish(TEMPERATURE_VALUE_TOPIC, buffer);
    sprintf(buffer, "%f", humidity);
    mqtt.publish(HUMIDITY_VALUE_TOPIC, buffer);
    sprintf(buffer, "%f", pressure);
    mqtt.publish(PRESSURE_VALUE_TOPIC, buffer);

    delay(10000);
}
