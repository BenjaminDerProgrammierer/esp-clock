/**
 * MyMqtt.h
 * Benjamin Hartmann | 10/2025
 */

#ifndef _MY_MQTT_H_
#define _MY_MQTT_H_

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "MqttCredentials.h"

#define QOS 1        // Quality of Service Level
#define RETAIN true  // retained message

class MyMqtt {
   private:
    String _clientId =
        "ESP-" + String(ESP.getChipId()) + "-" + String(random(0xffff), HEX);
    WiFiClient _wifi;
    PubSubClient _client = PubSubClient(_wifi);

    String _allTopics;
    String _lwtTopic;
    String _deviceNameTopic;
    String _devicePlaceTopic;
    String _wifiSsidTopic;
    String _wifiIpTopic;
    String _timestampTopic;
    String _debugTopic;
    String _bmeTemperatureTopic;
    String _bmeHumidityTopic;
    String _bmePressureTopic;
    String _bmeAltitudeTopic;

    String _deviceName;
    String _devicePlace;

    /**
     * Send initial MQTT messages (LWT, device info, WiFi info).
     */
    void sendInitMessages() {
        _client.publish(_lwtTopic.c_str(), "online", RETAIN);
        _client.publish(_deviceNameTopic.c_str(), _deviceName.c_str(), RETAIN);
        _client.publish(_devicePlaceTopic.c_str(), _devicePlace.c_str(), RETAIN);
        _client.publish(_wifiSsidTopic.c_str(), WiFi.SSID().c_str(), RETAIN);
        _client.publish(_wifiIpTopic.c_str(), WiFi.localIP().toString().c_str(),
                       RETAIN);
    }

    /**
     * Connect to the MQTT broker.
     * This function is blocking.
     */
    void connect() {
        // Loop until we're reconnected
        while (!_client.connected()) {
            Serial.print("Attempting MQTT connection...");

            // Attempt to connect
            if (_client.connect(_clientId.c_str(), MY_MQTT_USERNAME, MY_MQTT_PASSWORD, _lwtTopic.c_str(), QOS, RETAIN, "offline")) {
                Serial.printf(" - Connected to MQTT Broker: %s:%d\n", MY_MQTT_BROKER, MY_MQTT_PORT);
                _client.subscribe(_allTopics.c_str());
                sendInitMessages();
            } else {
                Serial.printf("failed, rc=%d, trying again in 5 seconds\n", _client.state());
                delay(5000);
            }
        }
    }

   public:
    MyMqtt(String deviceName, String devicePlace, String topicBase) {
        _allTopics = topicBase + "#";
        _lwtTopic = topicBase + "WifiStatus";

        _deviceNameTopic = topicBase + "DeviceName";
        _devicePlaceTopic = topicBase + "DevicePlace";
        _wifiSsidTopic = topicBase + "WiFi_SSID";
        _wifiIpTopic = topicBase + "WiFi_IP";
        _timestampTopic = topicBase + "TimeStamp";
        _debugTopic = topicBase + "Debug_Info";

        _bmeTemperatureTopic = topicBase + "BME280_Temperature_°C";
        _bmeHumidityTopic = topicBase + "BME280_Humidity_%";
        _bmePressureTopic = topicBase + "BME280_Pressure_hPa";
        _bmeAltitudeTopic = topicBase + "BME280_Altitude_m";

        _deviceName = deviceName;
        _devicePlace = devicePlace;
    }

    /**
     * Initialize MQTT client and connect to broker.
     */
    void begin() {
        _client.setServer(MY_MQTT_BROKER, MY_MQTT_PORT);
        _client.setCallback(
            [this](const char* topic, const byte* payload, unsigned int length) {
                if (strcmp(topic, _allTopics.c_str()) == 0) {
                    Serial.print("Message arrived [");
                    Serial.print(topic);
                    Serial.print("] ");
                    for (unsigned int i = 0; i < length; i++) {
                        Serial.print((char)payload[i]);
                    }
                    Serial.println();
                }
            });

        connect();
    }

    /**
     * Maintain MQTT connection and process incoming messages.
     */
    void loop() {
        // Ensure the client is connected
        if (!_client.connected()) {
            connect();
        }

        // Maintain MQTT connection
        _client.loop();
    }

    /**
     * Publish BME280 sensor data to MQTT topics.
     */
    void publishSensorData(float temperature, float humidity, float pressure, float altitude) {
        _client.publish(_bmeTemperatureTopic.c_str(), String(temperature).c_str(), RETAIN);
        _client.publish(_bmeHumidityTopic.c_str(), String(humidity).c_str(), RETAIN);
        _client.publish(_bmePressureTopic.c_str(), String(pressure).c_str(), RETAIN);
        _client.publish(_bmeAltitudeTopic.c_str(), String(altitude).c_str(), RETAIN);
    }

    /**
     * Publish current time to MQTT topic.
     */
    void publishTimeStamp(String timeStamp) {
        _client.publish(_timestampTopic.c_str(), timeStamp.c_str(), RETAIN);
    }
};

#endif  // _MY_MQTT_H_
