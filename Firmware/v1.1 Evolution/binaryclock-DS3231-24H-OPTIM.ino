#include <DS3232RTC.h>    // Install "DS3232RTC" by Jack Christensen
#include <TimeLib.h>      // Install "Time" by Paul Stoffregen
#include <Wire.h>

DS3232RTC RTC;
time_t getTajms() {
    return RTC.get();
}

// Pin 19 (A5) released for RTC SCL. Last LED moved from pin 19 to pin 17 (A3).
const uint8_t hourLEDs [] = {4,3,2,17}; 
const uint8_t minuteLEDs [] = {10,9,8,7,6,5};  
const uint8_t secondLEDs [] = {16,15,14,13,12,11};  
const uint8_t loopLEDs[] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,17};
int lastSecond = -1;

// spin millis
bool isSpinning = false;
unsigned long lastSpinMillis = 0;
uint8_t spinIdx = 0;
uint16_t totalSteps = 0;
uint16_t currentStep = 0;

void setup()
{
  Wire.begin();
  // Wire.setClock(400000);
  Serial.begin(115200); // initialize Serial communication 115200 baud and No line ending
  // while(!Serial);

  Serial.println(F("========================================"));
  Serial.println(F("Project: RGB Binary Clock v1.1"));
  Serial.println(F("Copyright Marek Bujko"));
  Serial.println(F("Licensed under MIT License"));
  Serial.println(F("Format to set: HH MM SS DD MM YYYY"));
  Serial.println(F("========================================"));

  setSyncProvider(getTajms);

  if (timeStatus() != timeSet) {
    Serial.println(F("RTC Sync Error!"));
  } else {
    Serial.print(F("RTC Sync OK! Current: "));
    
    // Date: DD.MM.YYYY
    if (day() < 10) Serial.print('0'); Serial.print(day());
    Serial.print('.');
    if (month() < 10) Serial.print('0'); Serial.print(month());
    Serial.print('.'); Serial.print(year());
    
    Serial.print(F(" | "));

    // Time: HH:MM:SS
    if (hour() < 10) Serial.print('0'); Serial.print(hour());
    Serial.print(':');
    if (minute() < 10) Serial.print('0'); Serial.print(minute());
    Serial.print(':');
    if (second() < 10) Serial.print('0'); Serial.println(second());
  }
  
  for (uint8_t i = 0; i<4; i++)
  {
    pinMode(hourLEDs[i], OUTPUT);
  }
  for (uint8_t i = 0; i<6; i++)
  {
    pinMode(minuteLEDs[i], OUTPUT);
  }
  for (uint8_t i = 0; i<6; i++)
  {
    pinMode(secondLEDs[i], OUTPUT);
  }
 }

void loop(){
  if (Serial.available() > 0) {
    int h  = Serial.parseInt(); 
    int m  = Serial.parseInt(); 
    int s  = Serial.parseInt(); 
    int d  = Serial.parseInt(); 
    int mo = Serial.parseInt(); 
    int y  = Serial.parseInt(); 

    if (h >= 0 && h < 24) {
      tmElements_t tm;
      tm.Hour = h; 
      tm.Minute = m; 
      tm.Second = s;
      tm.Day = (d > 0) ? d : day();
      tm.Month = (mo > 0) ? mo : month();
      
      if (y > 0) {
        if (y < 100) y += 2000; 
        tm.Year = CalendarYrToTm(y);
      } else {
        tm.Year = CalendarYrToTm(year());
      }

      RTC.write(tm);
      setTime(makeTime(tm));
      Serial.println(F("* Date and time updated via Serial *"));
    }
    while(Serial.available() > 0) Serial.read(); // Clear buffer
  }

  updateSpin();

  int s = second();
  if (s != lastSecond) {

    if (minute() == 0 && s == 0 && !isSpinning) {
      startSpin(hour());
    }
 
    if (!isSpinning) {
      updateDisplay();
    }

    // Print Date
    if (day() < 10) Serial.print('0'); Serial.print(day());
    Serial.print('.');
    if (month() < 10) Serial.print('0'); Serial.print(month());
    Serial.print('.'); Serial.print(year());
    
    Serial.print(F(" | "));

    // Print Time (24h)
    if (hour() < 10) Serial.print('0'); Serial.print(hour());
    Serial.print(':');
    if (minute() < 10) Serial.print('0'); Serial.print(minute());
    Serial.print(':');
    if (second() < 10) Serial.print('0'); Serial.print(s);

    // Print Temperature (DS3231/DS3232 RTC)
    float tempC = RTC.temperature() / 4.0;

    // To use Celsius: Keep these lines active
    Serial.print(F(" | Temp: ")); 
    Serial.print(tempC); 
    Serial.println(F(" C"));

    /* 
    To use Fahrenheit: 
    Comment out the three lines above (Serial.print for Celsius)
    Uncomment the lines below:
    float tempF = (tempC * 9.0 / 5.0) + 32.0;
    Serial.print(F(" | Temp: ")); 
    Serial.print(tempF); 
    Serial.println(F(" F"));
    */

    lastSecond = s;
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

void setOutput(const uint8_t *ledArray, uint8_t numLEDs, int value) {
  for (uint8_t i = 0; i < numLEDs; i++) {
    digitalWrite(ledArray[i], bitRead(value, i));
  }
}

void startSpin(int count) {
  int loops = (count == 0) ? 12 : (count > 12 ? count - 12 : count);
  totalSteps = loops * 16;
  currentStep = 0;
  spinIdx = 0;
  isSpinning = true;
}

void updateSpin() {
  if (!isSpinning) return;

  if (millis() - lastSpinMillis >= 30) {
    lastSpinMillis = millis();

    for(uint8_t i=0; i<16; i++) digitalWrite(loopLEDs[i], LOW);

    digitalWrite(loopLEDs[spinIdx], HIGH);
    
    spinIdx = (spinIdx + 1) % 16;
    currentStep++;

    if (currentStep >= totalSteps) {
      digitalWrite(loopLEDs[spinIdx], LOW);
      isSpinning = false;
    }
  }
}
// Copyright Marek Bujko