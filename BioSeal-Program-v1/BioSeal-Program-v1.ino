/*
===============================================================
                         Project: Bioseal
===============================================================
 Description:
    This program powers the Bioseal system — a smart,
    sensor-driven solution for monitoring fermentation 
    conditions such as CO2 levels, temperature, and humidity. 
    Data is collected via environmental sensors and displayed 
    on an OLED screen, providing real-time status updates.

 Proponents:
    - Zachiebelle Avril Canapi
    - Princess Joy Garcia
    - Clare Elisha Antonio

 Programmed & Embedded by:
    - Kian Clarel R. Bunagan

===============================================================
*/

#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_SSD1306.h>      // SSD1306 OLED display driver
#include "SparkFun_SCD4x_Arduino_Library.h"  // SCD40/41

// ===================== DISPLAY CONFIG (u8g2) =====================
// _1_ page buffer variant => minimal RAM usage
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ===================== SENSOR =============================
SCD4x scd40;

// ===================== FERMENTATION LOGIC =====================
#define CO2_ACTIVE_MIN_PPM  800
#define CO2_VENT_PPM        10000
#define T_GOOD_MIN_C        18.0
#define T_GOOD_MAX_C        24.0
#define RH_LOW_WARN         60.0

static inline uint8_t fermentStatus(uint16_t co2, float tC, float rh) {
  if (co2 >= CO2_VENT_PPM)                      return 2; // VENT
  if (tC < (T_GOOD_MIN_C - 2.0))                return 3; // COLD
  if (tC > (T_GOOD_MAX_C + 2.0))                return 4; // WARM
  if (co2 < CO2_ACTIVE_MIN_PPM)                 return 1; // IDLE
  if (tC >= T_GOOD_MIN_C && tC <= T_GOOD_MAX_C) return 0; // GOOD
  if (rh < RH_LOW_WARN)                         return 5; // DRY
  return 6; // OK
}

// ===================== DRAW HELPERS ============================
// Use a single medium font for readability that still fits four rows
// (Monospace helps predictable widths.)
#define FONT_MED  u8g2_font_8x13_mf

static void drawSplash() {
  // Bigger, centered BIOSEAL + lightweight loading bar animation (u8g2 page buffered)
  const char* title = "BIOSEAL";

  const uint8_t *titleFont = u8g2_font_logisoso20_tf; // bigger, clean, still fits 128x64
  u8g2.setFont(titleFont);

  // Compute centered coordinates
  uint16_t tw = u8g2.getStrWidth(title);
  int16_t x = (128 - (int16_t)tw) / 2;  // center horizontally
  int16_t ascent = u8g2.getAscent();    // positive
  int16_t descent = u8g2.getDescent();  // negative
  int16_t th = ascent - descent;        // font height
  int16_t baseline = (64 - th) / 2 + ascent; // vertical center baseline

  // Loading bar just below the text
  const int barW = 96;
  const int barH = 6;
  const int barX = (128 - barW) / 2;
  const int barY = baseline + 10; // below text; fits within 64px

  // Animate in a few lightweight steps (no extra buffers)
  for (int w = 0; w <= barW; w += 16) {
    u8g2.firstPage();
    do {
      u8g2.setFont(titleFont);
      u8g2.drawStr(x, baseline, title);
      u8g2.drawFrame(barX - 1, barY - 1, barW + 2, barH + 2);
      if (w > 0) {
        u8g2.drawBox(barX, barY, w, barH);
      }
    } while (u8g2.nextPage());
    delay(20); // short, smooth, SRAM-free
  }
  // minimal delay before data
  delay(40);
}

static void drawDegreeC(int x_center, int baseline_y) {
  // baseline_y is the text baseline; font height ~13 => draw a tiny circle above baseline
  int y_center = baseline_y - 7; // tune for 8x13 font
  u8g2.drawCircle(x_center, y_center, 2, U8G2_DRAW_ALL);
}

static void drawScreen(uint16_t co2, float tC, float rh, uint8_t status) {
  char buf[24];

  u8g2.firstPage();
  do {
    u8g2.setFont(FONT_MED);

    uint8_t y = 14;               // four rows fit within 64px

    // CO2
    u8g2.drawStr(2, y, "CO2:");
    dtostrf((double)co2, 0, 0, buf); // integer ppm
    u8g2.drawStr(52, y, buf);

    // TEMP
    y += 16;
    u8g2.drawStr(2, y, "TEMP:");
    dtostrf((double)tC, 0, 1, buf);
    u8g2.drawStr(52, y, buf);
    // degree glyph + C
    int xdeg = 52 + u8g2.getStrWidth(buf) + 4;
    drawDegreeC(xdeg, y);
    u8g2.drawStr(xdeg + 6, y, "C");

    // HUMD
    y += 16;
    u8g2.drawStr(2, y, "HUMD:");
    dtostrf((double)rh, 0, 1, buf);
    u8g2.drawStr(52, y, buf);
    int xpc = 52 + u8g2.getStrWidth(buf) + 2;
    u8g2.drawStr(xpc, y, "%");

    // STATUS (show mapped text for all codes)
    y += 16;
    u8g2.drawStr(2, y, "FERM:");
    const char* sTxt = "OK";
    switch (status) {
      case 0: sTxt = "GOOD"; break;
      case 1: sTxt = "IDLE"; break;
      case 2: sTxt = "VENT"; break;
      case 3: sTxt = "COLD"; break;
      case 4: sTxt = "WARM"; break;
      case 5: sTxt = "DRY"; break;
      default: sTxt = "OK"; break;
    }
    u8g2.drawStr(52, y, sTxt);

  } while (u8g2.nextPage());
}

// ===================== SETUP ==============================
void setup() {
  Serial.begin(115200);
  Wire.begin();
  u8g2.begin();

  drawSplash();

  if (!scd40.begin()) {
    if (Serial) Serial.println(F("SCD40 not detected. Check wiring."));
    for(;;){}
  }
  scd40.stopPeriodicMeasurement();
  scd40.startPeriodicMeasurement();
}

// ===================== LOOP ===============================
void loop() {
  if (scd40.readMeasurement()) {
    float co2f = scd40.getCO2();
    float tC   = scd40.getTemperature();
    float rh   = scd40.getHumidity();

    uint16_t co2 = (uint16_t)co2f;
    uint8_t status = fermentStatus(co2, tC, rh);

    drawScreen(co2, tC, rh, status);

    if (Serial) {
      Serial.print(F("CO2:")); Serial.print(co2);
      Serial.print(F(" ppm\tT:")); Serial.print(tC, 1);
      Serial.print(F(" C\tRH:")); Serial.print(rh, 1);
      Serial.print(F(" %\t"));
      switch (status) {
        case 0: Serial.print(F("GOOD")); break;
        case 1: Serial.print(F("IDLE")); break;
        case 2: Serial.print(F("VENT")); break;
        case 3: Serial.print(F("COLD")); break;
        case 4: Serial.print(F("WARM")); break;
        case 5: Serial.print(F("DRY")); break;
        default: Serial.print(F("OK")); break;
      }
      Serial.println();
    }
  }

  delay(15000); // 15 seconds
}
