/*
Relay shield (DFRobot)
Sensors: SEN0193
LCD: LCD1602 Module v1.0 (DFRobot)
Flowers: Ivy

DFR0554 lib is required for LCD
*/

#include "DFR0554.h"

DFR0554 lcd = DFR0554();
byte LCD_BRIGHTNESS = 100; // from 0 to 255
boolean lcdIsOn;

int NUM_SENSORS = 1;

int DIAL_PIN = A0;
int SENSOR_PINS[] = {A1};
int PUMP_PINS[] = {2};
int SENSOR_PIN = A1;

int DIAL_MIN_VALUE = 1023;
int DIAL_MAX_VALUE = 0;

int SENSOR_AIR_VALUES[] = {512};
int SENSOR_WATER_VALUES[] = {238};

//Ivy
float HUMIDITY_THRESHOLDS[] = {0.27};

int HUMIDITY_CHECK_INTERVAL_SEC = 5;
int TURN_PUMP_FOR_SEC = 2;


void setup() {
  lcd.begin(&Wire);
  for (int i = 0; i < NUM_SENSORS; ++i) {
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
  for (int i = 0; i < NUM_SENSORS; ++i) {
    float humidity = calcHumidity(i, analogRead(SENSOR_PINS[i]));
    if (humidity < HUMIDITY_THRESHOLDS[i]) {
      pump(i);
    }
  }
  delay(HUMIDITY_CHECK_INTERVAL_SEC * 1000);
}

float calcHumidity(int sensorIndex, int sensorValue) {
  return (float)(sensorValue - SENSOR_AIR_VALUES[sensorIndex]) / (float)(SENSOR_WATER_VALUES[sensorIndex] - SENSOR_AIR_VALUES[sensorIndex]);
}

//0 - display is turned off, 1..NUM_SENSORS - show humidity value for given sensor
int calcDialPosition(int dialValue) {
  float dialPercentage = (float)(dialValue - DIAL_MIN_VALUE) / (float)(DIAL_MAX_VALUE - DIAL_MIN_VALUE);
  int value = dialPercentage * (NUM_SENSORS + 1);
  if (value < 0) return 0;
  if (value > NUM_SENSORS) return NUM_SENSORS;
  return value;
}

void pump(int pumpIndex) {
  digitalWrite(PUMP_PINS[pumpIndex], HIGH);
  delay(TURN_PUMP_FOR_SEC * 1000);
  digitalWrite(PUMP_PINS[pumpIndex], LOW);
}

void showOnLcd(int sensorIndex) {
  int sensorValue = analogRead(SENSOR_PINS[sensorIndex]);
  int humidityPercent = 100 * calcHumidity(sensorIndex, sensorValue);
  lcdOn();
  lcd.clear();
  lcd.print("S");
  lcd.print(sensorIndex + 1);
  lcd.print(": ");
  lcd.print(humidityPercent);
  lcd.print("%, (");
  lcd.print(sensorValue);
  lcd.print(")");
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
