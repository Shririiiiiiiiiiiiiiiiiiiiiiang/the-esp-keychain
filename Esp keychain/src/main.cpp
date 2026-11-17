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

void setClock(int y, int m, int d, int h, int mn) {
  struct tm t = {};
t.tm_year = y - 1900;
t.tm_mon = m - 1;
t.tm_mday = d;
t.tm_hour = h;
t.tm_min = mn;
time_t stamp = mktime(&t);
struct timeval tv = { stamp, 0};
settimeofday(&tv, nullptr);
timeset = true;
  
    
}


void drawAnimation() {
  screen.setFont(u8g2_font_ncenB10_tr);
  screen.drawStr(scrollX, 36, start_text);
  scrollX -= scroll_speed;
  if(scrollX < -screen.getStrWidth(start_text)) scrollX = 128;

}

void drawtime () {
  if(!timeset) {
    screen.setFont(u8g2_font_6x10_tr);
    screen.drawStr(10, 28, "time not set");
    return;
  }


time_t now = time(nullptr);
struct tm t;
localtime_r(&now, &t);
char buf[24];
 strftime(buf, sizeof(buf), "%H:%M:%S", &t);
 screen.setFont(u8g2_font_logisoso20_tr);
 screen.drawStr((128 - screen.getStrWidth(buf)) / 2, 42, buf);

}
































void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  screen.begin();

  setenv("TZ", time_zone, 1);
  tzset();
  if (use_fk_time)setClock(fk_year, fk_month, fk_day, fk_hour, fk_min);
  showClock = true;
}
void loop() {
  unsigned long now = millis();
  if(now - lastframe < framepause_MS) return;
  lastframe = now;

  screen.clearBuffer();
  if(showClock) drawtime();
  else drawAnimation();
  drawAnimation();
  screen.sendBuffer();


}




















