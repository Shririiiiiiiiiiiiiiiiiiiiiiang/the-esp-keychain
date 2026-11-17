#pragma once

const int OLED_SDA = 5;
const int OLED_SCL = 6;

const int WIFI_BTN = 1;
const int TIMEDATE_BTN = 2;

const char* const start_text = "Hellooooo";
const int framepause_MS = 100;
const int pixls_pf = 3;
const unsigned long but_press = 30;

const char* const time_zone = "IST-5:30";
const bool use_fk_time = true; //giving it fake vars just so till this connects to my phone so it can get the real time and all
const int fk_year = 2026;
const int fk_month = 11;
const int fk_day = 21;
const int fk_hour = 15;
const int fk_min = 17;  
const int scroll_speed = 3;