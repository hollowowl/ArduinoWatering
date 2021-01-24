/*
Relay shield (DFRobot)
Sensors: SEN0114, SEN0308 (2GND)
LCD: LCD1602 Module v1.0 (DFRobot)
Flowers: Ivy, Thuja

DFR0554 lib is required for LCD
*/

#include "DFR0554.h"

DFR0554 lcd = DFR0554();
byte LCD_BRIGHTNESS = 100; // from 0 to 255
boolean lcdIsOn;

int NUM_PLANTS = 2;

int DIAL_PIN = A0;
// Don't use A4 and A5 - they acts also as SCL and SDA for IIC port (used for LCD)
int SENSOR_PINS[] = {A1, A3};
int PUMP_PINS[] = {2, 7};

int DIAL_MIN_VALUE = 1023;
int DIAL_MAX_VALUE = 0;

int SENSOR_AIR_VALUES[] = {0, 582};
int SENSOR_WATER_VALUES[] = {877, 17};

//Values are 0..100
int HUMIDITY_THRESHOLDS[] = {30, 40};

int HUMIDITY_CHECK_INTERVAL_SEC = 5;
int TURN_PUMP_FOR_SEC = 2;

long noPumpCycles[] = {0, 0};

void setup() {
  lcd.begin(&Wire);
  for (int i = 0; i < NUM_PLANTS; ++i) {
    pinMode(PUMP_PINS[i], OUTPUT);
  }
}

void loop() {
  int dialPosition = calcDialPosition(analogRead(DIAL_PIN));
  if (dialPosition) {
    showOnLcd(dialPosition - 1);
  } else {
    lcdOff();
  }
  for (int i = 0; i < NUM_PLANTS; ++i) {
    int humidity = calcHumidity(i, analogRead(SENSOR_PINS[i]));
    if (humidity < HUMIDITY_THRESHOLDS[i]) {
      pump(i);
      noPumpCycles[i] = 0;
    } else {
      ++noPumpCycles[i];
    }
  }
  delay(HUMIDITY_CHECK_INTERVAL_SEC * 1000);
}

int calcHumidity(int sensorIndex, int sensorValue) {
  return map(sensorValue, SENSOR_AIR_VALUES[sensorIndex], SENSOR_WATER_VALUES[sensorIndex], 0, 100);
}

//0 - display is turned off, 1..NUM_PLANTS - show humidity value for given sensor
int calcDialPosition(int dialValue) {
  float dialPercentage = (float)(dialValue - DIAL_MIN_VALUE) / (float)(DIAL_MAX_VALUE - DIAL_MIN_VALUE);
  int value = dialPercentage * (NUM_PLANTS + 1);
  if (value < 0) return 0;
  if (value > NUM_PLANTS) return NUM_PLANTS;
  return value;
}

void pump(int pumpIndex) {
  digitalWrite(PUMP_PINS[pumpIndex], HIGH);
  delay(TURN_PUMP_FOR_SEC * 1000);
  digitalWrite(PUMP_PINS[pumpIndex], LOW);
}

void showOnLcd(int sensorIndex) {
  int sensorValue = analogRead(SENSOR_PINS[sensorIndex]);
  int humidity = calcHumidity(sensorIndex, sensorValue);
  lcdOn();
  lcd.clear();
  lcd.print("S");
  lcd.print(sensorIndex + 1);
  lcd.print(": ");
  lcd.print(humidity);
  lcd.print("%, (");
  lcd.print(sensorValue);
  lcd.print(")");
  lcd.setCursorPosition(1, 0);
  lcd.print("NPC: ");
  lcd.print(noPumpCycles[sensorIndex]);
}

void lcdOn() {
  if (!lcdIsOn) {
    lcd.turnOn();
    lcd.setLdrStateAll(LDR_STATE_IND_GRP);
    lcd.setGroupControlMode(GROUP_CONTROL_MODE_DIMMING);
    lcd.setRGB(LCD_BRIGHTNESS, LCD_BRIGHTNESS, LCD_BRIGHTNESS);
    lcdIsOn = true;
  }
}

void lcdOff() {
  if (lcdIsOn) {
    lcd.turnOff();
    lcdIsOn = false;
  }
}
