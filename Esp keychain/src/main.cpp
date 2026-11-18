#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <time.h>
#include <sys/time.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
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
WebServer server(80);
File upFile;

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
  server.begin();
  wifiOn = true;
}

void wifistop() {
  server.stop();
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



const char page[] PROGMEM = R"rawmaterial(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>My Esp Keychainnnn </title>
</head>
<body style="font-family:sans-serif; text-align:center;margin-top:40px">
<h1>ESP keychain site</h1>
<p id="msg">Time sync inprocess...........</p>
<h3>Add an animation</h3>
<p>paste the ardino code from the site 1.oledanimationmaker.com and 2.animator.wokwi.com</p>
<br>
<p> instructions in Readme</p>
<textarea id="code" rows="8" style="width:100%"></textarea>
<p>
Name <input id="name" value="animifst" size="10"><br>
Width <input id="w" type="number" value="128" style="width:60px">
Height <input id="h" type="number" value="64" style"width:60px">
Delay ms <input id = "d" type="number" value="100" style="width:70px">
<br>
Bit order <select id="order">
<option value="msb">Adafruit/Wowki</option>
<option valeu="lsb">U8g2</option>
</select>
</p>
<button onclick="send()">Uploadd</button>
<p id="out"></p>
<h2>saved</h2>
<pre id="list"></pre>
<script>
fetch('/time?t=' + Math.floor(Date.now() / 1000))
.then(r => r.text())
.then(t => document.getElementById('msg').textContent = t);

function $(id) {return document.getElementById(id); }

function showlist() {
fetch('/list').then(r => r.text()).then(t => $('list').textContent = t);
}
showList();

function rev(b) {
let r=0;
for(let i = 0; i <8; i++) r= (r <<1 ) | ((b >> i) & 1);
return r;
}

function send() {
}

</script>
</body>
</html>
)rawmaterial";



void handelroot() {
  server.send(200, "text/html", page);
}

void handeltime() {
  if(!server.hasArg("t")) {
    server.send(400, "text/plain", "no time went");
    return;
  }
  time_t secs = (time_t) strtoll(server.arg("t").c_str(), nullptr, 10);
  struct timeval tv = {secs, 0};
  settimeofday(&tv, nullptr);
  timeset = true;
  server.send(200, "text/plain", "time synced, Lets goo");
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
  server.on("/", handelroot);
  server.on("/time", handeltime);
}
void loop() {
  if(butpressedcount() && !wifiOn) showClock = !showClock;
  if(wifibtnpressed()) {
    if(wifiOn) wifistop();
    else wifiStart();
  }
  if(wifiOn) server.handleClient();

  unsigned long now = millis();
  if(now - lastframe < framepause_MS) return;
  lastframe = now;

  screen.clearBuffer();
  if(wifiOn) drawWifi();
  else if(showClock) drawtime();
  else drawAnimation();
  screen.sendBuffer();
  

}
















