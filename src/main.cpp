#include <Arduino.h>

#include "MyOledDisplay.h"
#include "MyWifiManager.h"
#include "MyTime.h"
#include "MySensor.h"
#include "MyMqtt.h"

// WiFi Manager instance
MyWifiManager* wifiManager;

int displayState = 0;

unsigned long lastTimeUpdate = 0;
#define UPDATE_INTERVAL 3000 // Update time every 3 seconds

static unsigned long lastMqttPublish = 0;
#define PUBLISH_INTERVAL 1000 // Publish MQTT data every second

/**
 * Check for serial commands
 * Commands:
 *   reset - Reset WiFi credentials and restart
 *   status - Show current WiFi status
 */
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toLowerCase();
        
        if (command == "reset") {
            Serial.println("\n=== Resetting WiFi Credentials ===");
            wifiManager->resetCredentials();
            delay(1000);
            Serial.println("Restarting device...");
            ESP.restart();
        } 
        else if (command == "status") {
            Serial.println("\n=== WiFi Status ===");
            if (wifiManager->isConnected()) {
                Serial.print("Connected to: ");
                Serial.println(wifiManager->getSSID());
                Serial.print("IP Address: ");
                Serial.println(wifiManager->getIP());
            } else if (wifiManager->isConfigMode()) {
                Serial.println("Configuration Mode Active");
                Serial.print("AP SSID: ESP8266-Setup");
                Serial.print(" @ ");
                Serial.println(wifiManager->getAPIP());
            } else {
                Serial.println("Not connected");
            }
            Serial.println("==================\n");
        }
        else if (command.length() > 0) {
            Serial.println("Unknown command. Available commands:");
            Serial.println("  reset  - Reset WiFi credentials");
            Serial.println("  status - Show WiFi status");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    printI2CScanner();
    initOledDisplay();
    
    // Initialize WiFi Manager
    displayWiFiInfo();
    wifiManager = new MyWifiManager();
    
    // Wait for WiFi connection
    while (!wifiManager->isConnected()) {
        wifiManager->loop();
        delay(100);
    }
    
    // Once connected, display WiFi info and initialize services
    displayWiFiInfo(true, wifiManager->getSSID(), wifiManager->getIP());
    delay(2000);  // Show WiFi info for 2 seconds
    
    initTime();
    initBME280();
    initMqtt();
    
    Serial.println("\n=== Device Ready ===");
    Serial.println("Serial commands available:");
    Serial.println("  reset  - Reset WiFi credentials");
    Serial.println("  status - Show WiFi status");
    Serial.println("====================\n");
}

void loop() {
    // Check for serial commands
    handleSerialCommands();
    
    // IMPORTANT: Call WiFi Manager loop
    wifiManager->loop();
    
    // Only run application code when connected
    if (!wifiManager->isConnected()) {
        return;  // Skip rest of loop if not connected
    }
    
    switch (displayState) {
        case 0:
            displayWiFiInfo(true, wifiManager->getSSID(), wifiManager->getIP());
            break;
        case 1:
            displayTime(getLocalTimeString());
            break;
        case 2:
            displaySensorValues(
                bme.readTemperature(),
                bme.readHumidity(),
                bme.readPressure() / 100.0F,
                bme.readAltitude(SEALEVELPRESSURE_HPA)
            );
            break;
        default:
            displayState = 0;
            break;
    }

    if (millis() - lastTimeUpdate >= UPDATE_INTERVAL) {
        lastTimeUpdate = millis();
        Serial.print(displayState);
        Serial.print(" -> ");
        displayState = (displayState + 1) % 3; // Toggle display state
        Serial.println(displayState);
    }

    loopMqtt();

    if (millis() - lastMqttPublish >= PUBLISH_INTERVAL) {
        lastMqttPublish = millis();
        String temperature = String(bme.readTemperature());
        String humidity = String(bme.readHumidity());
        String pressure = String(bme.readPressure() / 100.0F);
        String altitude = String(bme.readAltitude(SEALEVELPRESSURE_HPA));
        publishSensorData(temperature, humidity, pressure, altitude);
        publishTimeStamp(getLocalTimeString());
    }
}
