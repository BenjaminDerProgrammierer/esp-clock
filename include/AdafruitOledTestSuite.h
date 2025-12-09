#ifndef _ADAFRUIT_OLED_TEST_SUITE_H_
#define _ADAFRUIT_OLED_TEST_SUITE_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>

#include "MyDisplay.h"
#include "MyLogos.h"

void testdrawline(Adafruit_SH110X& display);
void testdrawrect(Adafruit_SH110X& display);
void testfillrect(Adafruit_SH110X& display);
void testdrawcircle(Adafruit_SH110X& display);
void testfillcircle(Adafruit_SH110X& display);
void testdrawroundrect(Adafruit_SH110X& display);
void testfillroundrect(Adafruit_SH110X& display);
void testdrawtriangle(Adafruit_SH110X& display);
void testfilltriangle(Adafruit_SH110X& display);
void testdrawchar(Adafruit_SH110X& display);
void testdrawstyles(Adafruit_SH110X& display);
void testdrawbitmap(Adafruit_SH110X& display);
void testanimate(const uint8_t* bitmap, uint8_t w, uint8_t h,
                 Adafruit_SH110X& display);
void runAdafruitTestSuite(Adafruit_SH110X& display);

/**
 * Run the Adafruit OLED Test Suite on the global `display` object.
 * You need to have initialized the display before calling this function.
 */
void runAdafruitTestSuite(Adafruit_SH110X& display) {
    // draw a single pixel
    display.drawPixel(10, 10, SH110X_WHITE);
    // Show the display buffer on the hardware.
    // NOTE: You _must_ call display after making any drawing commands
    // to make them visible on the display hardware!
    display.display();
    delay(2000);
    display.clearDisplay();

    testdrawline(display);       // Draw many lines
    testdrawrect(display);       // Draw rectangles (outlines)
    testfillrect(display);       // Draw rectangles (filled)
    testdrawcircle(display);     // Draw circles (outlines)
    testfillcircle(display);     // Draw circles (filled)
    testdrawroundrect(display);  // Draw rounded rectangles (outlines)
    testfillroundrect(display);  // Draw rounded rectangles (filled)
    testdrawtriangle(display);   // Draw triangles (outlines)
    testfilltriangle(display);   // Draw triangles (filled)
    testdrawchar(display);       // Draw characters of the default font
    testdrawstyles(display);     // Draw 'stylized' characters
    testdrawbitmap(display);     // Draw a small bitmap image

    testanimate(adafruitLogoBmp, ADAFRUIT_LOGO_WIDTH, ADAFRUIT_LOGO_HEIGHT,
                display);  // Animate bitmaps
}

// Adafruit Test Suite

void testdrawline(Adafruit_SH110X& display) {
    int16_t i;

    display.clearDisplay();  // Clear display buffer

    for (i = 0; i < display.width(); i += 4) {
        display.drawLine(0, 0, i, display.height() - 1, SH110X_WHITE);
        display.display();  // Update screen with each newly-drawn line
        delay(1);
    }
    for (i = 0; i < display.height(); i += 4) {
        display.drawLine(0, 0, display.width() - 1, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();

    for (i = 0; i < display.width(); i += 4) {
        display.drawLine(0, display.height() - 1, i, 0, SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (i = display.height() - 1; i >= 0; i -= 4) {
        display.drawLine(0, display.height() - 1, display.width() - 1, i,
                         SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();

    for (i = display.width() - 1; i >= 0; i -= 4) {
        display.drawLine(display.width() - 1, display.height() - 1, i, 0,
                         SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (i = display.height() - 1; i >= 0; i -= 4) {
        display.drawLine(display.width() - 1, display.height() - 1, 0, i,
                         SH110X_WHITE);
        display.display();
        delay(1);
    }
    delay(250);

    display.clearDisplay();

    for (i = 0; i < display.height(); i += 4) {
        display.drawLine(display.width() - 1, 0, 0, i, SH110X_WHITE);
        display.display();
        delay(1);
    }
    for (i = 0; i < display.width(); i += 4) {
        display.drawLine(display.width() - 1, 0, i, display.height() - 1,
                         SH110X_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);  // Pause for 2 seconds
}

void testdrawrect(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = 0; i < display.height() / 2; i += 2) {
        display.drawRect(i, i, display.width() - 2 * i,
                         display.height() - 2 * i, SH110X_WHITE);
        display.display();  // Update screen with each newly-drawn rectangle
        delay(1);
    }

    delay(2000);
}

void testfillrect(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = 0; i < display.height() / 2; i += 3) {
        // The INVERSE color is used so rectangles alternate white/black
        display.fillRect(i, i, display.width() - i * 2,
                         display.height() - i * 2, SH110X_INVERSE);
        display.display();  // Update screen with each newly-drawn rectangle
        delay(1);
    }

    delay(2000);
}

void testdrawcircle(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = 0; i < max(display.width(), display.height()) / 2;
         i += 2) {
        display.drawCircle(display.width() / 2, display.height() / 2, i,
                           SH110X_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testfillcircle(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = max(display.width(), display.height()) / 2; i > 0;
         i -= 3) {
        // The INVERSE color is used so circles alternate white/black
        display.fillCircle(display.width() / 2, display.height() / 2, i,
                           SH110X_INVERSE);
        display.display();  // Update screen with each newly-drawn circle
        delay(1);
    }

    delay(2000);
}

void testdrawroundrect(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
        display.drawRoundRect(i, i, display.width() - 2 * i,
                              display.height() - 2 * i, display.height() / 4,
                              SH110X_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testfillroundrect(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
        // The INVERSE color is used so round-rects alternate white/black
        display.fillRoundRect(i, i, display.width() - 2 * i,
                              display.height() - 2 * i, display.height() / 4,
                              SH110X_INVERSE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testdrawtriangle(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = 0; i < max(display.width(), display.height()) / 2;
         i += 5) {
        display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                             display.width() / 2 - i, display.height() / 2 + i,
                             display.width() / 2 + i, display.height() / 2 + i,
                             SH110X_WHITE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testfilltriangle(Adafruit_SH110X& display) {
    display.clearDisplay();

    for (int16_t i = max(display.width(), display.height()) / 2; i > 0;
         i -= 5) {
        // The INVERSE color is used so triangles alternate white/black
        display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                             display.width() / 2 - i, display.height() / 2 + i,
                             display.width() / 2 + i, display.height() / 2 + i,
                             SH110X_INVERSE);
        display.display();
        delay(1);
    }

    delay(2000);
}

void testdrawchar(Adafruit_SH110X& display) {
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE);  // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.cp437(true);  // Use full 256 char 'Code Page 437' font

    // Not all the characters will fit on the display. This is normal.
    // Library will draw what it can and the rest will be clipped.
    for (int16_t i = 0; i < 256; i++) {
        if (i == '\n')
            display.write(' ');
        else
            display.write(i);
    }

    display.display();
    delay(2000);
}

void testdrawstyles(Adafruit_SH110X& display) {
    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE);  // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Hello, world!"));

    display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // Draw 'inverse' text
    display.println(3.141592);

    display.setTextSize(2);  // Draw 2X-scale text
    display.setTextColor(SH110X_WHITE);
    display.print(F("0x"));
    display.println(0xDEADBEEF, HEX);

    display.display();
    delay(2000);
}

void testdrawbitmap(Adafruit_SH110X& display) {
    display.clearDisplay();

    display.drawBitmap((display.width() - ADAFRUIT_LOGO_WIDTH) / 2,
                       (display.height() - ADAFRUIT_LOGO_HEIGHT) / 2,
                       adafruitLogoBmp, ADAFRUIT_LOGO_WIDTH,
                       ADAFRUIT_LOGO_HEIGHT, 1);
    display.display();
    delay(1000);
}

#define XPOS 0  // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2

void testanimate(const uint8_t* bitmap, uint8_t w, uint8_t h,
                 Adafruit_SH110X& display) {
    int8_t NUMFLAKES = 10;
    int8_t f, icons[NUMFLAKES][3];

    // Initialize 'snowflake' positions
    for (f = 0; f < NUMFLAKES; f++) {
        icons[f][XPOS] = random(1 - ADAFRUIT_LOGO_WIDTH, display.width());
        icons[f][YPOS] = -ADAFRUIT_LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
        Serial.print(F("x: "));
        Serial.print(icons[f][XPOS], DEC);
        Serial.print(F(" y: "));
        Serial.print(icons[f][YPOS], DEC);
        Serial.print(F(" dy: "));
        Serial.println(icons[f][DELTAY], DEC);
    }

    while (true) {
        display.clearDisplay();  // Clear the display buffer

        // Draw each snowflake:
        for (f = 0; f < NUMFLAKES; f++) {
            display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h,
                               SH110X_WHITE);
        }

        display.display();  // Show the display buffer on the screen
        delay(200);         // Pause for 1/10 second

        // Then update coordinates of each flake...
        for (f = 0; f < NUMFLAKES; f++) {
            icons[f][YPOS] += icons[f][DELTAY];
            // If snowflake is off the bottom of the screen...
            if (icons[f][YPOS] >= display.height()) {
                // Reinitialize to a random position, just off the top
                icons[f][XPOS] =
                    random(1 - ADAFRUIT_LOGO_WIDTH, display.width());
                icons[f][YPOS] = -ADAFRUIT_LOGO_HEIGHT;
                icons[f][DELTAY] = random(1, 6);
            }
        }
    }
}

#endif  // _ADAFRUIT_OLED_TEST_SUITE_H_