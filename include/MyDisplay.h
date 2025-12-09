/**
 * MyOledDisplay.h
 * Benjamin Hartmann | 10/2025
 */

#ifndef _MY_DISPLAY_H_
#define _MY_DISPLAY_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "MyLogos.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class MyDisplay {
   private:
    uint8_t _address;
    Adafruit_SH1106G _display;

   public:
    MyDisplay(uint8_t address = 0x3C)
        : _address(address),
          _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

    /**
     * Initialize the OLED display.
     */
    void begin() {
        _display.begin(_address, true);

        // _display.setContrast(0); // dim display

        // Show image buffer on the display hardware.
        // Since the buffer is intialized with an Adafruit splashscreen
        // internally, this will display the splashscreen.
        _display.display();

        delay(2000);

        // Clear the buffer
        _display.clearDisplay();
    }

    /**
     * I2C Scanner: prints to Serial.
     */
    void scanI2C() {
        Serial.println("I2C Scanner");
        Wire.begin();

        uint8_t count = 0;

        for (uint8_t i = 1; i < 120; i++) {
            Wire.beginTransmission(i);
            if (Wire.endTransmission() == 0) {
                Serial.printf("- Found address: %d (0x%02X)\n", i, i);
                count++;
                delay(1);
            }
        }

        Serial.printf("Done. Found %d device(s).\n", count);
    }

    /**
     * Display WiFi info.
     */
    void showWiFiInfo(bool connected, const String& ssid, const String& ip) {
        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 0);

        _display.println("WiFi Info:");
        _display.println("----------");

        if (!connected) {
            _display.println("Connecting...");
        } else {
            _display.print("SSID: ");
            _display.println(ssid);
            _display.print("IP: ");
            _display.println(ip);
        }

        _display.display();
    }

    /**
     * Show AP info.
     */
    void showAPInfo(const String& ssid, const String& password,
                    const String& ip) {
        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 0);

        _display.println("WiFi Info:");
        _display.println("----------");
        _display.print("SSID: ");
        _display.println(ssid);
        _display.print("Password: ");
        _display.println(password);
        _display.print("IP: ");
        _display.println(ip);

        _display.display();
    }

    /** Overload: no args = "connecting" */
    void showWiFiInfo() { showWiFiInfo(false, "", ""); }

    /**
     * Display current time.
     */
    void showTime(const String& timeStr) {
        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 0);

        _display.println("Current Time:");
        _display.println("-------------");
        _display.println(timeStr);

        _display.display();
    }

    /**
     * Display BME280 sensor data.
     */
    void showSensorValues(float temperature, float humidity, float pressure,
                          float altitude) {
        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SH110X_WHITE);
        _display.setCursor(0, 0);

        _display.println("Sensor Values:");
        _display.println("--------------");

        _display.printf("Temp: %.2f %c\n", temperature, (char)247);  // ° symbol
        _display.printf("Pres: %.2f hPa\n", pressure);
        _display.printf("Hum:  %.2f %%\n", humidity);
        _display.printf("Alt:  %.2f m\n", altitude);

        _display.display();
    }
};

#endif  // _MY_DISPLAY_H_
