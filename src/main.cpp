/**
 * ESP Clock
 * Benjamin Hartmann | 10/2025-12/2025
 */

#include <Arduino.h>

// TODO: MyWifiManager & 11/12/17+ WebServer

#include "MyDisplay.h"
#include "MyMqtt.h"
#include "MySensor.h"
#include "MyTime.h"
#include "MyWifi.h"

#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"  // Europe/Vienna

MySensor sensor = MySensor();
MyDisplay display = MyDisplay();
MyWifi wifi = MyWifi();
MyTime theTime = MyTime(TZ);  // variable name "time" is already taken.
MyMqtt mqtt = MyMqtt("ESP8266", "Bedroom", "My_SmartHome/Benjamin/");

int state = 0;
unsigned long lastAction1s = 0;
unsigned long lastAction3s = 0;

void setup() {
    Serial.begin(115200);
    Serial.println();

    sensor.begin();
    display.begin();
    display.scanI2C();
    display.showWiFiInfo();
    wifi.connect();
    theTime.begin();
    mqtt.begin();
}

void loop() {
    // Serial.printf("WiFi Connected: %s, SSID: %s, IP: %s, MAC: %s\n", wifi.getWifiConnectedState() ? "Yes" : "No", wifi.getWifiSSID().c_str(), wifi.getWifiIP().c_str(), wifi.getWifiMAC().c_str());
    // sensor.printValues();
    // Serial.printf("The current time is %s.\n", theTime.getLocalTimeString().c_str());
    wifi.ensureConnected();
    mqtt.loop();

    switch (state) {
        case 0:
            display.showSensorValues(
                sensor.readTemperatureC(), sensor.readHumidity(),
                sensor.readPressure(), sensor.readAltitude());
            break;
        case 1:
            if (wifi.getConnectedState()) {
                display.showWiFiInfo(true, wifi.getWifiSSID(),
                                     wifi.getWifiIP());

            } else {
                // display.showAPInfo("MySSID", "MyPassword", "192.168.1.1");
                display.showWiFiInfo();
            }
            break;
        case 2:
            display.showTime(theTime.getLocalTimeString());
            break;
    }

    if (millis() - lastAction3s > 3000) {
        state = (state + 1) % 3;
        lastAction3s = millis();
    }

    if (millis() - lastAction1s > 1000) {
        mqtt.publishSensorData(sensor.readTemperatureC(), sensor.readHumidity(), sensor.readPressure(), sensor.readAltitude());
        mqtt.publishTimeStamp(theTime.getLocalTimeString());
        lastAction1s = millis();
    }
}