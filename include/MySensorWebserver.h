/**
 * MySensorWebserver.h
 * Benjamin Hartmann | 11/2025
 */

#ifndef _MY_SENSOR_WEBSERVER_H_
#define _MY_SENSOR_WEBSERVER_H_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Updater.h>

class MySensorWebserver {
   private:
    AsyncWebServer* _server;
    AsyncEventSource* _events;
    unsigned long lastEventSend = 0;

   public:
    bool isBegun = false;

    void begin() {
        Serial.println("[Webserver] Starting sensor webserver...");

        _server = new AsyncWebServer(8080);
        _events = new AsyncEventSource("/events");

        _server->serveStatic("/", LittleFS, "/sensor-gauges/").setDefaultFile("index.html");
    
        _events->onConnect([](AsyncEventSourceClient* client) {
            Serial.printf("[Webserver] SSE client connected from %s\n", client->client()->remoteIP().toString().c_str());
        });
        _server->addHandler(_events);

        _server->onNotFound([](AsyncWebServerRequest* request) {
            Serial.printf("[Webserver] 404: %s\n", request->url().c_str());
            request->send(404, "text/plain", "Not Found");
        });

        // OTA Update handler
        _server->on("/update", HTTP_POST,
            [](AsyncWebServerRequest* request) {
                AsyncWebServerResponse* response;
                if (Update.hasError()) {
                    response = request->beginResponse(200, "text/plain", "Update Failed!");
                    response->addHeader("Connection", "close");
                    request->send(response);
                } else {
                    response = request->beginResponse(302, "text/plain", "");
                    response->addHeader("Location", "/");
                    response->addHeader("Connection", "close");
                    request->send(response);
                    delay(1000);
                    ESP.restart();
                }
            },
            [](AsyncWebServerRequest* request, String filename, size_t index,
               uint8_t* data, size_t len, bool final) {
                if (!index) {
                    Serial.printf("[OTA Update] Update Start: %s\n", filename.c_str());
                    Update.runAsync(true);
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace, U_FLASH)) {
                        Update.printError(Serial);
                    }
                }
                if (len) {
                    if (Update.write(data, len) != len) {
                        Update.printError(Serial);
                    }
                }
                if (final) {
                    if (Update.end(false)) {
                        Serial.printf("[OTA Update] Update Success: %uB\n", index + len);
                    } else {
                        Update.printError(Serial);
                    }
                }
            });

        _server->begin();
        Serial.println("[Webserver] Server started on port 8080");
        isBegun = true;
    }

    void sendEvents(float temperatureC, float temperatureF, float humidity,
                    float pressure, float altitude) {
        // Throttle to once per second
        if (millis() - lastEventSend < 1000) return;
        lastEventSend = millis();

        JsonDocument document = JsonDocument();
        document["temperatureC"] = temperatureC;
        document["temperatureF"] = temperatureF;
        document["humidity"] = humidity;
        document["pressure"] = pressure;
        document["altitude"] = altitude;

        String documentStr = "";
        serializeJson(document, documentStr);

        _events->send(documentStr.c_str(), "readings", millis());
    }
};
#endif  // _MY_SENSOR_WEBSERVER_H_
