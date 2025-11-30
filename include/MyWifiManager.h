/**
 * MyWifiManager.h
 * Benjamin Hartmann | 11/2025
 * 
 * WiFi Manager with captive portal for ESP8266
 * Handles WiFi configuration with web interface and credential storage
 */

#ifndef _MY_WIFI_MANAGER_H_
#define _MY_WIFI_MANAGER_H_

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// Configuration
#define DNS_PORT 53
#define WEB_SERVER_PORT 80
#define AP_SSID "ESP8266-Setup"
#define AP_PASSWORD ""  // Empty = open AP
#define CONNECT_TIMEOUT 20000  // ms
#define CREDENTIALS_FILE "/wifi_config.json"

class MyWifiManager {
private:
  // Access Point Configuration
  IPAddress apIP;
  IPAddress netMask;
  String apSsid;
  String apPassword;

  // Server components (only active in AP mode)
  DNSServer* dnsServer;
  ESP8266WebServer* webServer;

  // Connection state
  bool isAPMode;
  bool shouldTryConnect;
  String pendingSSID;
  String pendingPassword;

  /**
   * Start Access Point mode for configuration
   */
  void startAP() {
    Serial.println("[WiFiManager] Starting Access Point...");
    
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, netMask);
    
    if (apPassword.length() > 0) {
      WiFi.softAP(apSsid.c_str(), apPassword.c_str());
    } else {
      WiFi.softAP(apSsid.c_str());
    }
    
    isAPMode = true;
    
    Serial.print("[WiFiManager] AP Started: ");
    Serial.print(apSsid);
    Serial.print(" @ ");
    Serial.println(apIP);
  }

  /**
   * Stop Access Point and clean up servers
   */
  void stopAP() {
    if (!isAPMode) return;
    
    Serial.println("[WiFiManager] Stopping Access Point...");
    
    if (dnsServer) {
      dnsServer->stop();
      delete dnsServer;
      dnsServer = nullptr;
    }
    
    if (webServer) {
      webServer->stop();
      delete webServer;
      webServer = nullptr;
    }
    
    WiFi.softAPdisconnect(true);
    isAPMode = false;
  }

  /**
   * Initialize DNS server for captive portal
   */
  void startDNSServer() {
    if (dnsServer) return;
    
    dnsServer = new DNSServer();
    int result = dnsServer->start(DNS_PORT, "*", apIP);
    Serial.printf("[WiFiManager] DNS Server started: %d\n", result);
  }

  /**
   * Initialize web server with routes
   */
  void startWebServer() {
    if (webServer) return;
    
    webServer = new ESP8266WebServer(WEB_SERVER_PORT);
    

    webServer->serveStatic("/", LittleFS, "/www/portal/index.html");
    webServer->serveStatic("/portal.css", LittleFS, "/www/portal/portal.css");
    webServer->serveStatic("/portal.js", LittleFS, "/www/portal/portal.js");
    
    // API: Get available networks
    webServer->on("/networks", HTTP_GET, [this]() {
      handleNetworkScan();
    });
    
    // API: Connect to WiFi
    webServer->on("/connect", HTTP_POST, [this]() {
      handleConnect();
    });
    
    // Captive Portal API (RFC 8908)
    webServer->on("/captive-portal/api", HTTP_GET, [this]() {
      handleCaptivePortalAPI();
    });
    
    // Handle 404 - redirect to portal
    webServer->onNotFound([this]() {
      handleNotFound();
    });
    
    webServer->begin();
    Serial.println("[WiFiManager] Web server started");
  }

  /**
   * Handle network scan request
   */
  void handleNetworkScan() {
    if (!webServer) return;
    
    Serial.println("[WiFiManager] Scanning networks...");
    int n = WiFi.scanNetworks();
    
    DynamicJsonDocument doc(2048);
    JsonArray networks = doc.to<JsonArray>();
    
    for (int i = 0; i < n; i++) {
      JsonObject network = networks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      network["encryption"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "open" : "encrypted";
    }
    
    String response;
    serializeJson(doc, response);
    
    webServer->send(200, "application/json", response);
    WiFi.scanDelete();
  }

  /**
   * Handle WiFi connection request
   */
  void handleConnect() {
    if (!webServer) return;
    
    if (!webServer->hasArg("plain")) {
      webServer->send(400, "application/json", "{\"success\":false,\"message\":\"No data\"}");
      return;
    }
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, webServer->arg("plain"));
    
    if (error) {
      webServer->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
      return;
    }
    
    pendingSSID = doc["ssid"].as<String>();
    pendingPassword = doc["password"].as<String>();
    
    if (pendingSSID.length() == 0) {
      webServer->send(400, "application/json", "{\"success\":false,\"message\":\"SSID required\"}");
      return;
    }
    
    Serial.printf("[WiFiManager] Attempting connection to: %s\n", pendingSSID.c_str());
    
    // Send immediate response
    webServer->send(200, "application/json", "{\"success\":true,\"message\":\"Connecting...\"}");
    
    shouldTryConnect = true;
  }

  /**
   * Handle captive portal API request (RFC 8908)
   */
  void handleCaptivePortalAPI() {
    if (!webServer) return;
    
    bool captive = (WiFi.status() != WL_CONNECTED);
    String userPortalUrl = String("http://") + apIP.toString() + "/";
    
    String json = "{\"captive\":";
    json += captive ? "true" : "false";
    json += ",\"user-portal-url\":\"" + userPortalUrl + "\"}";
    
    webServer->sendHeader("Cache-Control", "private");
    webServer->send(200, "application/captive+json", json);
  }

  /**
   * Handle 404 - redirect to portal
   */
  void handleNotFound() {
    if (!webServer) return;
    
    Serial.printf("[WiFiManager] 404: %s from %s\n", 
                  webServer->uri().c_str(), 
                  webServer->hostHeader().c_str());
    
    // If requesting captive portal API, handle it
    if (webServer->uri().startsWith("/captive-portal/api")) {
      handleCaptivePortalAPI();
      return;
    }
    
    // Otherwise redirect to portal
    webServer->sendHeader("Location", String("http://") + apIP.toString() + "/", true);
    webServer->send(302, "text/plain", "");
  }

  /**
   * Attempt to connect to WiFi with saved credentials
   */
  bool tryConnectToWiFi(const String& ssid, const String& password) {
    Serial.printf("[WiFiManager] Connecting to: %s\n", ssid.c_str());
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    unsigned long startTime = millis();
    while (millis() - startTime < CONNECT_TIMEOUT) {
      delay(200);
      
      // Process DNS/Web requests while waiting
      if (isAPMode && dnsServer && webServer) {
        dnsServer->processNextRequest();
        webServer->handleClient();
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WiFiManager] Connected!");
        Serial.print("[WiFiManager] IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
      }
    }
    
    Serial.println("[WiFiManager] Connection failed");
    return false;
  }

  /**
   * Save WiFi credentials to LittleFS
   */
  bool saveCredentials(const String& ssid, const String& password) {
    DynamicJsonDocument doc(512);
    doc["ssid"] = ssid;
    doc["password"] = password;
    
    File file = LittleFS.open(CREDENTIALS_FILE, "w");
    if (!file) {
      Serial.println("[WiFiManager] Failed to open credentials file for writing");
      return false;
    }
    
    serializeJson(doc, file);
    file.close();
    
    Serial.println("[WiFiManager] Credentials saved");
    return true;
  }

  /**
   * Load WiFi credentials from LittleFS
   */
  bool loadCredentials(String& ssid, String& password) {
    if (!LittleFS.exists(CREDENTIALS_FILE)) {
      Serial.println("[WiFiManager] No saved credentials");
      return false;
    }
    
    File file = LittleFS.open(CREDENTIALS_FILE, "r");
    if (!file) {
      Serial.println("[WiFiManager] Failed to open credentials file");
      return false;
    }
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
      Serial.println("[WiFiManager] Failed to parse credentials");
      return false;
    }
    
    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();
    
    Serial.printf("[WiFiManager] Loaded credentials for: %s\n", ssid.c_str());
    return true;
  }

public:
  /**
   * Constructor
   * @param _apIP Access Point IP address
   * @param _netMask Network mask
   * @param _apSsid Access Point SSID (default: ESP8266-Setup)
   * @param _apPassword Access Point password (default: empty/open)
   */
  MyWifiManager(IPAddress _apIP = IPAddress(192, 168, 4, 1),
                IPAddress _netMask = IPAddress(255, 255, 255, 0),
                String _apSsid = AP_SSID,
                String _apPassword = AP_PASSWORD)
      : apIP(_apIP),
        netMask(_netMask),
        apSsid(_apSsid),
        apPassword(_apPassword),
        dnsServer(nullptr),
        webServer(nullptr),
        isAPMode(false),
        shouldTryConnect(false) {
    
    // Initialize LittleFS
    if (!LittleFS.begin()) {
      Serial.println("[WiFiManager] ERROR: Failed to mount LittleFS");
      return;
    }
    Serial.println("[WiFiManager] LittleFS mounted");
    
    // Try to connect with saved credentials
    String savedSSID, savedPassword;
    if (loadCredentials(savedSSID, savedPassword)) {
      if (tryConnectToWiFi(savedSSID, savedPassword)) {
        // Successfully connected - stay in STA mode, no AP needed
        return;
      }
    }
    
    // No saved credentials or connection failed - start AP mode
    Serial.println("[WiFiManager] Starting configuration mode");
    startAP();
    startDNSServer();
    startWebServer();
    
    // Start async network scan
    WiFi.scanNetworks(true);
  }

  /**
   * Destructor - clean up resources
   */
  ~MyWifiManager() {
    stopAP();
  }

  /**
   * Main loop - must be called regularly from main loop
   */
  void loop() {
    // Only process servers if in AP mode
    if (isAPMode) {
      if (dnsServer) dnsServer->processNextRequest();
      if (webServer) webServer->handleClient();
    }
    
    // Handle pending connection attempt
    if (shouldTryConnect) {
      shouldTryConnect = false;
      
      if (tryConnectToWiFi(pendingSSID, pendingPassword)) {
        // Connection successful
        saveCredentials(pendingSSID, pendingPassword);
        stopAP();  // Stop AP mode and servers
        WiFi.mode(WIFI_STA);  // Switch to Station mode only
      } else {
        // Connection failed - restart AP if needed
        if (!isAPMode) {
          startAP();
          startDNSServer();
          startWebServer();
        }
      }
      
      // Restart network scan
      WiFi.scanNetworks(true);
    }
  }

  /**
   * Check if WiFi is connected
   * @return true if connected to WiFi network
   */
  bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
  }

  /**
   * Check if in configuration mode (AP mode)
   * @return true if Access Point is active
   */
  bool isConfigMode() {
    return isAPMode;
  }

  /**
   * Get current WiFi SSID
   * @return SSID of connected network or empty string
   */
  String getSSID() {
    return WiFi.SSID();
  }

  /**
   * Get current IP address
   * @return IP address as string
   */
  String getIP() {
    return WiFi.localIP().toString();
  }

  /**
   * Get Access Point IP (when in config mode)
   * @return AP IP address as string
   */
  String getAPIP() {
    return apIP.toString();
  }

  /**
   * Reset saved credentials and restart in config mode
   */
  void resetCredentials() {
    Serial.println("[WiFiManager] Resetting credentials");
    
    if (LittleFS.exists(CREDENTIALS_FILE)) {
      LittleFS.remove(CREDENTIALS_FILE);
    }
    
    WiFi.disconnect();
    
    if (!isAPMode) {
      startAP();
      startDNSServer();
      startWebServer();
    }
  }
};

#endif  // _MY_WIFI_MANAGER_H_
