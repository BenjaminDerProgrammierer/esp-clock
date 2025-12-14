/**
 * ESP Clock
 * Benjamin Hartmann | 10/2025-12/2025
 */

#include <Arduino.h>

#include "MyDisplay.h"
#include "MyMqtt.h"
#include "MySensor.h"
#include "MySmarterWifi.h"
#include "MyTime.h"
#include "MySensorWebserver.h"

#define TZ "CET-1CEST,M3.5.0,M10.5.0/3"  // Europe/Vienna

MySensor sensor = MySensor();
MyDisplay display = MyDisplay();
MySmarterWifi wifi = MySmarterWifi();
MyTime theTime = MyTime(TZ);  // variable name "time" is already taken.
MyMqtt mqtt = MyMqtt("ESP8266", "Bedroom", "My_SmartHome/Benjamin/");
MySensorWebserver server = MySensorWebserver();

int state = 0;
unsigned long lastAction1s = 0;
unsigned long lastAction3s = 0;
String serialInput = "";

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
    if (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            if (serialInput == "help") {
                Serial.println("Available commands:");
                Serial.println("help - Show this help message");
                Serial.println("status - Show current status");
                Serial.println("reset - Reset WiFi settings");
            } else if (serialInput == "status") {
                Serial.println("Status command received.");
                Serial.printf("WiFi Connected: %s, SSID: %s, IP: %s, MAC: %s\n",
                              wifi.getConnectedState() ? "Yes" : "No",
                              wifi.getWifiSSID().c_str(),
                              wifi.getWifiIP().c_str(),
                              wifi.getWifiMAC().c_str());
                sensor.printValues();
                Serial.printf("The current time is %s.\n",
                              theTime.getLocalTimeString().c_str());

            } else if (serialInput == "reset") {
                Serial.println("Resetting WiFi settings...");
                wifi.resetCredentials();
                ESP.restart();
            } else {
                Serial.println(
                    "Unknown command. Type 'help' for a list of commands.");
            }
            serialInput = "";
        } else {
            serialInput += c;
        }
    }

    wifi.loop();

    switch (state) {
        case 0:
            display.showSensorValues(
                sensor.readTemperatureC(), sensor.readHumidity(),
                sensor.readPressure(), sensor.readAltitude());
            break;
        case 1:
            if (wifi.getConnectedState()) {
                display.showWiFiInfo(true, wifi.getWifiSSID(), wifi.getWifiIP());

            } else {
                display.showAPInfo(wifi.getApSSID(), wifi.getApPassword(),
                                   wifi.getApIp());
            }
            break;
        case 2:
            if (!wifi.getConnectedState()) {
                state = (state + 1) % 3;
            }
            display.showTime(theTime.getLocalTimeString());
            break;
    }

    if (millis() - lastAction3s > 3000) {
        state = (state + 1) % 3;
        lastAction3s = millis();
    }

    if (!wifi.getConnectedState()) return;
    if (!server.isBegun) server.begin();
    mqtt.loop();
    server.sendEvents(sensor.readTemperatureC(), sensor.readTemperatureF(), sensor.readHumidity(), sensor.readPressure(), sensor.readAltitude());

    if (millis() - lastAction1s > 1000 && wifi.getConnectedState()) {
        mqtt.publishSensorData(sensor.readTemperatureC(), sensor.readHumidity(),
                               sensor.readPressure(), sensor.readAltitude());
        mqtt.publishTimeStamp(theTime.getLocalTimeString());
        lastAction1s = millis();
    }
}