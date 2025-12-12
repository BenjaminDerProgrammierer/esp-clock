/**
 * MyTime.h
 * Benjamin Hartmann | 10/2025
 */

#ifndef _MY_TIME_H_
#define _MY_TIME_H_

#include <Arduino.h>
#include <time.h>

class MyTime {
   private:
    const char* _timezone;

   public:
    MyTime(const char* timezone) : _timezone(timezone) {}

    /**
     * Confiure the time server
     */
    void begin() {
        struct tm timeinfo;

        configTime(0, 0, "pool.ntp.org");
        setTimezone(_timezone);

        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }

        Serial.printf("Time initialized: %s\n", getLocalTimeString().c_str());
    }

    void begin(const char* timezone) {
        _timezone = timezone;
        begin();
    }

    /**
     * Change the tmezone.
     */
    void setTimezone(const char* timezone) {
        _timezone = timezone;
        setenv("TZ", _timezone, 1);
        tzset();
    }

    /**
     * Get the Time Struct object to format a time string to your liking.
     */
    struct tm getTimeStruct() {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
        }
        return timeinfo;
    }

    /**
     * Get the current local time as a formatted string.
     */
    String getLocalTimeString() {
        String timeStr;

        struct tm timeinfo = getTimeStruct();

        char buf[64];
        if (strftime(buf, sizeof(buf), "%A, %B %d %Y %H:%M:%S (zone %Z %z)",
                     &timeinfo)) {
            timeStr = String(buf);
        } else {
            return "Failed to format time";
        }
        return timeStr;
    }
};

#endif  // _MY_TIME_H_