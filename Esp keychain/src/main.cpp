#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <time.h>
#include <sys/time.h>

#include "config.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);

bool showClock = false;
bool timeset = false;
unsigned long lastframe = 0;
int scrollX = 128;

void drawAnimation() {
  screen.setFont(u8g2_font_ncenB10_tr);
  screen.drawStr(scrollX, 36, start_text);
  scrollX -= scroll_speed;
  if(scrollX < -screen.getStrWidth(start_text)) scrollX = 128;

}


































void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  screen.begin();
}
void loop() {
  unsigned long now = millis();
  if(now - lastframe < framepause_MS) return;
  lastframe = now;

  screen.clearBuffer();
  drawAnimation();
  screen.sendBuffer();
  

}




















