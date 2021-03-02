#include <M5StickC.h>
#include <WiFi.h>
#include "time.h"

// WiFi凭据
char* ssid       = "M5Demo";
char* password   = "12345678"; 

// NTP服务器地址
char* ntpServer =  "de.pool.ntp.org";

// 时区选择
int timeZone = 7200;

// 延迟设置
int tcount = 0;

// LCD状态
bool LCD = true;

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

void buttons_code() {
  // 按钮A控制LCD（开/关）
  if (M5.BtnA.wasPressed()) {
    if (LCD) {
      M5.Lcd.writecommand(ST7735_DISPOFF);
      M5.Axp.ScreenBreath(0);
      LCD = !LCD;
    } else {
      M5.Lcd.writecommand(ST7735_DISPON);
      M5.Axp.ScreenBreath(255);
      LCD = !LCD;
    }
  }
  // 如果按下按钮B两秒，则执行时间重新同步
  if (M5.BtnB.pressedFor(2000)) {
    timeSync();
  }
}

// LCD时间显示
void doTime() {
  if (timeToDo(1000)) {
    M5.Lcd.setTextSize(2);
    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);
    M5.Lcd.setCursor(27, 15);
    M5.Lcd.printf("%02d:%02d:%02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    M5.Lcd.setCursor(27, 50);
    M5.Lcd.setTextSize(1);
    M5.Lcd.printf("Date: %04d-%02d-%02d\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
  }
}

// 使用这个工作区通常会延迟停止一切
bool timeToDo(int tbase) {
  tcount++;
  if (tcount == tbase) {
    tcount = 0;
    return true;    
  } else {
    return false;
  }  
}

// 从NTP服务器同步时间
void timeSync() {
    M5.Lcd.setTextSize(1);
    Serial.println("Syncing Time");
    Serial.printf("Connecting to %s ", ssid);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 15);
    M5.Lcd.println("connecting WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" CONNECTED");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 15);
    M5.Lcd.println("Connected");
    // 将NTP时间设置到为本地
    configTime(timeZone, 0, ntpServer);

    // 获取本地时间
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
      // 设置RTC时间
      RTC_TimeTypeDef TimeStruct;
      TimeStruct.Hours   = timeInfo.tm_hour;
      TimeStruct.Minutes = timeInfo.tm_min;
      TimeStruct.Seconds = timeInfo.tm_sec;
      M5.Rtc.SetTime(&TimeStruct);

      RTC_DateTypeDef DateStruct;
      DateStruct.WeekDay = timeInfo.tm_wday;
      DateStruct.Month = timeInfo.tm_mon + 1;
      DateStruct.Date = timeInfo.tm_mday;
      DateStruct.Year = timeInfo.tm_year + 1900;
      M5.Rtc.SetData(&DateStruct);
      Serial.println("Time now matching NTP");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(20, 15);
      M5.Lcd.println("S Y N C");
      delay(500);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(40, 0, 2);
      M5.Lcd.println("Disk love Aya");
    }
}

void setup() {
  M5.begin();

  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE,BLACK);
  M5.Lcd.setCursor(40, 0, 2);
  M5.Lcd.println("Disk love Aya");
  // timeSync(); //如果您希望每次打开设备时都有timesync（如果没有WIFI，则取消注释）
}

void loop() {
  M5.update();
  buttons_code();
  doTime();
}
