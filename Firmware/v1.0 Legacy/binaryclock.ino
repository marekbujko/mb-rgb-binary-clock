#include <Time.h>
#include <TimeLib.h>

int hourLEDs [] = {4,3,2,19};  // least significant bit first
int minuteLEDs [] = {10,9,8,7,6,5};  
int secondLEDs [] = {16,15,14,13,12,11};  
int loopLEDs[] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,19};
int switchPin = 17;

void setup()
{
Serial.begin(9600); // initialize Serial communication
  while(!Serial) ;

  Serial.println("Project: RGB Binary Clock v1.0");
  Serial.println("Copyright Marek Bujko");
  Serial.println("Licensed under MIT License");
  Serial.println("--------------------------");
  
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
setTime(0);
 }


void loop(){
  if (digitalRead(switchPin))
  {
    adjustTime(1);
  }
  else if (minute() == 0 && second() == 0)
  {spin(hour());
  }
 
  updateDisplay();
  delay(1);

  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());

  Serial.println();

}


void updateDisplay()
{
  time_t t = now();
  setOutput(hourLEDs, 4, hour(t));
  setOutput(minuteLEDs, 6, minute(t));
  setOutput(secondLEDs, 6, second(t));
}

void setOutput(int *ledArray, int numLEDs, int value)
{
  for (int i = 0; i < numLEDs; i++)
  {
    digitalWrite(ledArray[i],
    bitRead(value, i));
  }
}

void spin(int count)
{
  for (int i = 0; i < count; i++)
  {
    for (int j = 0; j<16; j++)
    {
      digitalWrite(loopLEDs[j], HIGH);
      delay(50);
      digitalWrite(loopLEDs[j],LOW);
    }
  }
}
// Copyright Marek Bujko