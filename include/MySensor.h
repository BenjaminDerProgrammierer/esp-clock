/**
 * MySensor.h
 * Benjamin Hartmann | 10/2025
 */

#ifndef _MY_SENSOR_H_
#define _MY_SENSOR_H_

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Wire.h>

#define SEALEVELPRESSURE_HPA (1013.25)

class MySensor {
   private:
    uint8_t _address;
    Adafruit_BME280 _bme;

   public:
    MySensor(uint8_t address = 0x76) : _address(address) {}

    /**
     * Initialize the BME280 sensor.
     */
    void begin() {
        if (!_bme.begin(_address)) {
            Serial.println(
                "Could not find a valid BME280 sensor, check wiring!");
            while (1);
        }
    }

    /**
     * Print BME280 sensor values to the Serial Monitor.
     */
    void printValues() {
        Serial.println("BME280 Sensor Values:");
        Serial.printf("Temperature = %.2f °C\n", readTemperatureC());
        Serial.printf("Temperature = %.2f °F\n", readTemperatureF());
        Serial.printf("Pressure = %.2f hPa\n", readPressure());
        Serial.printf("Approx. Altitude = %.2f m\n", readAltitude());
        Serial.printf("Humidity = %.2f %%\n", readHumidity());
        Serial.println();
    }

    /**
     * Read temperature in Celsius.
     */
    float readTemperatureC() { return _bme.readTemperature(); }

    /**
     * Read temperature in Fahrenheit.
     *
     */
    float readTemperatureF() { return 1.8 * _bme.readTemperature() + 32; }

    /**
     * Read pressure in hPa.
     */
    float readPressure() { return _bme.readPressure() / 100.0F; }

    /**
     * Read humidity in percentage.
     */
    float readHumidity() { return _bme.readHumidity(); }

    /**
     * Read altitude in meters.
     */
    float readAltitude() { return _bme.readAltitude(SEALEVELPRESSURE_HPA); }
};

#endif  // _MY_SENSOR_H_
