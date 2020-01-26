// vim: ft=arduino
#include <Arduino.h>
#include <M5Stack.h>
#include "WiFi.h"

void setup() {
    // (screen, sd, serial, i2c)
    M5.begin(true, false, true, true);
    M5.Power.begin();

    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(YELLOW);

    M5.Lcd.setTextSize(2);
    M5.Lcd.println("Hello world!");

    M5.Lcd.setTextSize(6);
    M5.Lcd.print("00:00:00");
}

void loop() {
    M5.update();

    if (M5.BtnA.wasPressed()) {
        M5.Lcd.print("a");
    }
    if (M5.BtnA.wasReleased()) {
        M5.Lcd.print("A");
    }
}
