// Early prototype: fixed threshold, one servo.
// Cleaned-up version of archive/original_early_sketch.ino. The original called
// motor pins and a threshold variable that were never defined (left over from a
// rover example), and its servo line used map() with identical input bounds.
// Here the threshold is a constant you set by watching the serial monitor.
#include <Servo.h>

Servo myservo;

const int SERVO_PIN = 4;
const int EMG_PIN = A4;
const float THRESHOLD_MV = 1500.0;
const int OPEN_ANGLE = 0;
const int CLOSED_ANGLE = 180;

void setup() {
  myservo.attach(SERVO_PIN);
  myservo.write(OPEN_ANGLE);
  Serial.begin(9600);
}

void loop() {
  float sensorValue = analogRead(EMG_PIN);
  float millivolt = (sensorValue / 1023.0) * 5.0 * 1000.0;

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  Serial.print("Voltage: ");
  Serial.print(millivolt);
  Serial.println(" mV");
  Serial.println("");

  myservo.write(millivolt > THRESHOLD_MV ? CLOSED_ANGLE : OPEN_ANGLE);
  delay(250);
}
