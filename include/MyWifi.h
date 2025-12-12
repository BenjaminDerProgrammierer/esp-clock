/**
 * MyWifi.h
 * Benjamin Hartmann | 11/2025
 */

#ifndef _MY_WIFI_H_
#define _MY_WIFI_H_

#include <ESP8266WiFi.h>

#include "WifiCredentials.h"

class MyWifi {
   protected:
    unsigned long _lastWifiCheckTime = 0;

   public:
    /**
     * Connect to a WiFi network using the predefined credentials from
     * ``WifiCredentials.h``.
     */
    void connect(IPAddress ip, IPAddress gateway, IPAddress subnet,
                       IPAddress dns) {
        WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
        if (ip && gateway && subnet && dns) {
            WiFi.config(ip, gateway, subnet, dns);
        }
        Serial.print("Connecting to WiFi");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println(" - Connected to WiFi!");

        // WiFi.setAutoReconnect(true);
        // WiFi.persistent(true);
    }

    void connect() {
        connect(IPAddress(), IPAddress(), IPAddress(), IPAddress());
    }

    /**
     * Get the current WiFi connection status.
     * @return true if connected, false otherwise
     */
    bool getConnectedState() { return WiFi.status() == WL_CONNECTED; }

    /**
     * Get the current WiFi SSID.
     * @return The SSID of the connected WiFi network.
     */
    String getWifiSSID() { return WiFi.SSID(); }

    /**
     * Get the current WiFi IP address.
     * @return The IP address of the connected WiFi network.
     */
    String getWifiIP() { return WiFi.localIP().toString(); }

    /**
     * Get the current WiFi MAC address.
     * @return The MAC address of the WiFi interface.
     */
    String getWifiMAC() { return WiFi.macAddress(); }

    /**
     * Ensure the WiFi connection is active, reconnecting if necessary. To be
     * placed in the main loop.
     */
    void ensureConnected() {
        if (millis() - _lastWifiCheckTime > 3000) {
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("WiFi connection lost. Reconnecting...");
                connect();
            }
            _lastWifiCheckTime = millis();
        }
    }
};
#endif  // _MY_WIFI_H_
