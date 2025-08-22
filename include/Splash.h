// Splash screen drawing and audio helpers
#pragma once

#include <M5ez.h>
#include <M5Stack.h>
#include <math.h>

#define MAHOGANY_RGB565   M5.Lcd.color565(74, 0, 4)
#define LADYBUG_RGB565    M5.Lcd.color565(190, 49, 26)

#define NOTE_C7  2093
#define NOTE_D7  2349
#define NOTE_E7  2637
#define NOTE_F7  2794

void drawVerticalGradient(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint16_t colorStart, uint16_t colorEnd) {
  for (int i = 0; i < h; i++) {
    float ratio = (float)i / (float)h;
    uint8_t r = ((1 - ratio) * ((colorStart >> 11) & 0x1F) +
                 ratio * ((colorEnd >> 11) & 0x1F));
    uint8_t g = ((1 - ratio) * ((colorStart >> 5) & 0x3F) +
                 ratio * ((colorEnd >> 5) & 0x3F));
    uint8_t b = ((1 - ratio) * (colorStart & 0x1F) + ratio * (colorEnd & 0x1F));
    uint16_t color = (r << 11) | (g << 5) | b;
    M5.Lcd.drawFastHLine(x, y + i, w, color);
  }
}

void playnote(int x, int time, int frequency) {
  if (x == time) M5.Speaker.tone(frequency, 1);
}

void silence(int x, int time) {
  if (x == time) M5.Speaker.mute();
}

void drawGaussianWave(uint16_t color, int baseline = 150, int offset = 0) {
  float amplitude = 30.0, center = 160.0, sigma = 40.0, k = 0.3;
  for (int x = 0; x < 320; x++) {
    float gauss = exp(-pow(((x + offset) - center), 2) / (2 * sigma * sigma));
    float y = amplitude * gauss * sin(k * (x + offset));
    float gauss_next = exp(-pow((x + offset + 1 - center), 2) /
                           (2 * sigma * sigma));
    float y_next = amplitude * gauss * sin(k * (x + offset + 1));
    int c = gauss * 255;
    color = M5.Lcd.color565(c, c, c);
    M5.Lcd.drawLine(x, baseline + (int)y, x + 1, baseline + (int)y_next, color);
    M5.Lcd.drawLine(x, baseline + (int)y + 1, x + 1, baseline + (int)y_next + 1,
                    color);
    M5.Lcd.drawLine(x + 1, baseline + (int)y, x + 2,
                    baseline + (int)y_next, color);
    M5.Lcd.drawLine(x + 1, baseline + (int)y + 1, x + 2,
                    baseline + (int)y_next + 1, color);

    int ret = 10;
    playnote(x, ret + 71, NOTE_C7);
    silence(x, ret + 87);
    playnote(x, ret + 88, NOTE_D7);
    silence(x, ret + 104);
    playnote(x, ret + 105, NOTE_F7);
    silence(x, ret + 122);
    playnote(x, ret + 123, NOTE_E7);
    silence(x, ret + 139);
    playnote(x, ret + 140, NOTE_D7);
    silence(x, ret + 156);
    playnote(x, ret + 157, NOTE_F7);
    silence(x, ret + 250);

    delay(6);
  }
}

inline void showSplash() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(1);
  int cshift = 10;
  uint16_t RURED = M5.Lcd.color565(227, 0, 11);
  auto bg = RURED;
  auto fg = ez.theme->foreground;

  drawVerticalGradient(0, 0, 320, 240, MAHOGANY_RGB565, LADYBUG_RGB565);
  ez.setFreeFont(&FreeSansBold24pt7b);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(fg, bg);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setTextColor(TFT_LIGHTGREY);
  M5.Lcd.drawString("Rudicore-M5", 159, 40 + cshift);
  ez.setFreeFont(&FreeSans12pt7b);
  M5.Lcd.drawString("v"+Firmware_Version, 160, 90 + cshift);
  drawGaussianWave(MAHOGANY_RGB565, 150 + cshift, 51);
  delay(1500);
}
