/*
SEN0308, 5V, 2 GND
*/

int PUMP_PIN = 2;
int SENSOR_PIN = A1;

int SENSOR_AIR_VALUE = 582;
int SENSOR_WATER_VALUE = 17;  // At 100mm - "recommended" depth

float HUMIDITY_THRESHOLD = 0.4;
int HUMIDITY_CHECK_INTERVAL_SEC = 5;
int TURN_PUMP_FOR_SEC = 2;

float calcHumidity(int sensorValue) {
  return (float)(sensorValue - SENSOR_AIR_VALUE) / (float)(SENSOR_WATER_VALUE - SENSOR_AIR_VALUE);
}

void turnPumpFor(int ms) {
  digitalWrite(PUMP_PIN, HIGH);
  delay(ms);
  digitalWrite(PUMP_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(SENSOR_PIN);
  float humidity = calcHumidity(sensorValue);
  Serial.print("Moisture Sensor Value:");
  Serial.println(sensorValue);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  if (humidity < HUMIDITY_THRESHOLD) {
    turnPumpFor(TURN_PUMP_FOR_SEC * 1000);
  }
  delay(HUMIDITY_CHECK_INTERVAL_SEC * 1000);
}
