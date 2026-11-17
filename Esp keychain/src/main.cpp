#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <time.h>
#include <sys/time.h>
#include <WiFi.h>
#include "config.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);

bool showClock = false;
bool timeset = false;
unsigned long lastframe = 0;
int scrollX = 128;
bool lastread = HIGH;
bool stableread = HIGH;
unsigned long lastChange = 0;
bool wifiOn = false;
bool cplastread = HIGH;
bool cpstableread = HIGH;
unsigned long cplastchange = 0;

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


bool butpressedcount() {
  bool reading = digitalRead(TIMEDATE_BTN);
  if(reading != lastread) lastChange = millis();
  lastread = reading;

  if(millis() - lastChange > but_press && reading != stableread) {
    stableread = reading;
    if(stableread == LOW) return true;
  }
  return false;
}


bool wifibtnpressed() {
  bool reading = digitalRead(WIFI_BTN);
  if(reading != cplastread) cplastchange = millis();
  cplastread = reading;

  if (millis() - cplastchange > but_press && reading != cpstableread) {
    cpstableread = reading;
    if(cpstableread == LOW) return true;
  }
  return false;
}

void wifiStart() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(hotspot_name, hotspot_pass);
  wifiOn = true;
}

void wifistop() {
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  wifiOn = false;
}

void drawWifi() {
  screen.setFont(u8g2_font_6x10_tr);
  screen.drawStr(0, 12, "Wifi On");
  screen.drawStr(0, 26, hotspot_name);
  screen.drawStr(0, 44, hotspot_pass);
  screen.drawStr(0,60, "192.168.4.1");
}


























void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  screen.begin();

  setenv("TZ", time_zone, 1);
  tzset();
  if (use_fk_time)setClock(fk_year, fk_month, fk_day, fk_hour, fk_min);
  pinMode(TIMEDATE_BTN, INPUT_PULLUP);
  pinMode(WIFI_BTN, INPUT_PULLUP);
}
void loop() {
  if(butpressedcount() && !wifiOn) showClock = !showClock;
  if(wifibtnpressed()) {
    if(wifiOn) wifistop();
    else wifiStart();
  }

  unsigned long now = millis();
  if(now - lastframe < framepause_MS) return;
  lastframe = now;

  screen.clearBuffer();
  if(wifiOn) drawWifi();
  else if(showClock) drawtime();
  else drawAnimation();
  screen.sendBuffer();
  

}
















