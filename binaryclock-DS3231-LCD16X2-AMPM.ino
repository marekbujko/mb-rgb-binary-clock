#include <DS3232RTC.h>    // Install "DS3232RTC" by Jack Christensen
#include <TimeLib.h>      // Install "Time" by Paul Stoffregen
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Install "LiquidCrystal_I2C" by Martin Kubovčík

DS3232RTC RTC;
LiquidCrystal_I2C lcd(0x3F, 16, 2); // I2C settings for your LCD

time_t getTajms() {
    return RTC.get();
}

// Pin 19 (A5) released for RTC SCL. Last LED moved from pin 19 to pin 17 (A3).
int hourLEDs [] = {4,3,2,17}; 
int minuteLEDs [] = {10,9,8,7,6,5};  
int secondLEDs [] = {16,15,14,13,12,11};  
int loopLEDs[] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,17};
int lastSecond = -1;

void setup()
{
  Wire.begin();
  Serial.begin(9600); // initialize Serial communication 9600 baud and No line ending
  // while(!Serial);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RGB Clock v1.1");
  lcd.setCursor(0, 1);
  lcd.print("(c) Marek Bujko");
  delay(2000);
  lcd.clear();

  Serial.println(F("============================================"));
  Serial.println(F("Project: RGB Binary Clock v1.1 (US)"));
  Serial.println(F("Copyright Marek Bujko"));
  Serial.println(F("Licensed under MIT License"));
  Serial.println(F("Format to set: HH MM SS AM_PM MM DD YYYY"));
  Serial.println(F("AM_PM: 0 = AM, 1 = PM"));
  Serial.println(F("Example: 02 30 00 1 12 25 2026 (2:30:00 PM)"));
  Serial.println(F("============================================"));

  setSyncProvider(getTajms);

  if (timeStatus() != timeSet) {
    Serial.println(F("RTC Sync Error!"));
    lcd.setCursor(0, 0);
    lcd.print("RTC Sync Error!");
  } else {
    Serial.print(F("RTC Sync OK! Current: "));
    
    // US Startup Date: MM/DD/YYYY
    if (month() < 10) Serial.print('0'); Serial.print(month());
    Serial.print('/');
    if (day() < 10) Serial.print('0'); Serial.print(day());
    Serial.print('/'); Serial.print(year());
    
    Serial.print(F(" | "));

    // US Startup Time: 12h AM/PM
    int h = hour();
    String ampm = (h >= 12) ? " PM" : " AM";
    if (h > 12) h -= 12;
    if (h == 0) h = 12;
    if (h < 10) Serial.print('0'); Serial.print(h);
    Serial.print(':');
    if (minute() < 10) Serial.print('0'); Serial.print(minute());
    Serial.print(':');
    if (second() < 10) Serial.print('0'); Serial.print(second());
    Serial.println(ampm);
  }
  
  for (int i = 0; i<4; i++)
  {
    pinMode(hourLEDs[i], OUTPUT);
  }
  for (int i = 0; i<6; i++)
  {
    pinMode(minuteLEDs[i], OUTPUT);
  }
  for (int i = 0; i<6; i++)
  {
    pinMode(secondLEDs[i], OUTPUT);
  }
 }

void loop(){
  // US Input Logic: HH MM SS AM_PM MM DD YYYY
  if (Serial.available() > 0) {
    int h    = Serial.parseInt(); 
    int m    = Serial.parseInt(); 
    int s    = Serial.parseInt(); 
    int p    = Serial.parseInt(); // 0 = AM, 1 = PM
    int mo   = Serial.parseInt(); // Month first
    int d    = Serial.parseInt(); // Day second
    int y    = Serial.parseInt(); 

    // Convert 12h input + AM/PM to 24h for RTC storage
    if (p == 1 && h < 12) h += 12; // PM case
    if (p == 0 && h == 12) h = 0;  // 12 AM case (midnight)

    if (h >= 0 && h < 24) {
      tmElements_t tm;
      tm.Hour = h; tm.Minute = m; tm.Second = s;
      tm.Month = (mo > 0) ? mo : month();
      tm.Day = (d > 0) ? d : day();
      if (y > 0) {
        if (y < 100) y += 2000; 
        tm.Year = CalendarYrToTm(y);
      } else {
        tm.Year = CalendarYrToTm(year());
      }
      RTC.write(tm);
      setTime(makeTime(tm));
      Serial.println(F("* Date and time updated via Serial (US Format) *"));

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time updated!");
      delay(1000);
      lcd.clear();
    }
    while(Serial.available() > 0) Serial.read(); // Clear buffer
  }

if (minute() == 0 && second() == 0)
  {
    spin(hour());
  }
 
  updateDisplay();

  if (second() != lastSecond) {
    // Date: MM/DD/YYYY
    if (month() < 10) Serial.print('0'); Serial.print(month());
    Serial.print('/');
    if (day() < 10) Serial.print('0'); Serial.print(day());
    Serial.print('/'); Serial.print(year());
    
    Serial.print(F(" | "));

    // Time: 12h Format with AM/PM
    int h24 = hour();
    int h12 = h24;
    String ampm = (h24 >= 12) ? " PM" : " AM";
    if (h12 > 12) h12 -= 12;
    if (h12 == 0) h12 = 12;

    if (h12 < 10) Serial.print('0'); Serial.print(h12);
    Serial.print(':');
    if (minute() < 10) Serial.print('0'); Serial.print(minute());
    Serial.print(':');
    if (second() < 10) Serial.print('0'); Serial.print(second());
    Serial.print(ampm);

    // Print Temperature (DS3231/DS3232 RTC)
    float tempC = RTC.temperature() / 4.0;
    float tempF = (tempC * 9.0 / 5.0) + 32.0;
    Serial.print(F(" | Temp: ")); Serial.print(tempF); Serial.println(F(" F"));

    lcd.setCursor(0, 0);
    if (h12 < 10) lcd.print('0'); lcd.print(h12);
    lcd.print(':');
    if (minute() < 10) lcd.print('0'); lcd.print(minute());
    lcd.print(':');
    if (second() < 10) lcd.print('0'); lcd.print(second());
    lcd.print(ampm);

    lcd.setCursor(0, 1);
    if (month() < 10) lcd.print('0'); lcd.print(month());
    lcd.print('/');
    if (day() < 10) lcd.print('0'); lcd.print(day());
    lcd.print('/');
    lcd.print(year());
    
    lcd.print(" ");
    lcd.print((int)tempF); 
    lcd.print((char)223);
    lcd.print("F");

    lastSecond = second();
  }
}

void updateDisplay() {
  time_t t = now();
  int h = hour(t);
  if (h > 12) h -= 12;
  if (h == 0) h = 12; 

  setOutput(hourLEDs, 4, h);
  setOutput(minuteLEDs, 6, minute(t));
  setOutput(secondLEDs, 6, second(t));
}

void setOutput(int *ledArray, int numLEDs, int value) {
  for (int i = 0; i < numLEDs; i++) {
    digitalWrite(ledArray[i], bitRead(value, i));
  }
}

void spin(int count) {
  int loops = (count == 0) ? 12 : (count > 12 ? count - 12 : count);
  for (int i = 0; i < loops; i++) {
    for (int j = 0; j < 16; j++) {
      digitalWrite(loopLEDs[j], HIGH);
      delay(50);
      digitalWrite(loopLEDs[j], LOW);
    }
  }
}
// Copyright Marek Bujko