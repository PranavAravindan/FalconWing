// EMG-controlled prosthetic hand with per-user calibration.
//
// Flex the muscle -> hand closes. Relax -> hand opens (whole-hand control).
// Same pins as the early prototype: EMG on A4, servo on D4. No extra hardware.
//
// Every power-up runs a short calibration so the threshold fits whoever is
// wearing it: relax for 3 s (records the resting level), then flex for 3 s
// (records the peak), and the threshold is set halfway between the two.
//
// Note: the code from the final build was not recovered. This version was
// rebuilt from the early prototype sketch to match how the final hand behaved.
#include <Servo.h>

Servo handServo;

// Pins
const int SERVO_PIN = 4;
const int EMG_PIN = A4;
const int STATUS_LED = 13;          // on during the "flex" phase of calibration

// Servo positions: adjust to your finger mechanism
const int OPEN_ANGLE = 0;
const int CLOSED_ANGLE = 180;

// Calibration
const unsigned long PHASE_MS = 3000;
const float MIN_SPAN_MV = 300.0;    // flex must beat rest by this much, otherwise retry
const float HYSTERESIS_MV = 75.0;   // keeps the hand from flickering near the threshold

// Smoothing
const int SMOOTH_N = 10;

float thresholdMv = 1500.0;
bool handClosed = false;

float readMv() {
  long sum = 0;
  for (int i = 0; i < SMOOTH_N; i++) {
    sum += analogRead(EMG_PIN);
    delay(1);
  }
  return (sum / (float)SMOOTH_N / 1023.0) * 5.0 * 1000.0;
}

void setHand(bool close) {
  handServo.write(close ? CLOSED_ANGLE : OPEN_ANGLE);
  handClosed = close;
}

void calibrate() {
  while (true) {
    Serial.println("Calibration: relax your muscle...");
    delay(1500);
    unsigned long start = millis();
    float restSum = 0; long restN = 0;
    while (millis() - start < PHASE_MS) { restSum += readMv(); restN++; }
    float rest = restSum / restN;

    Serial.println("Now flex and hold...");
    digitalWrite(STATUS_LED, HIGH);
    delay(1000);
    start = millis();
    float peak = 0;
    while (millis() - start < PHASE_MS) {
      float v = readMv();
      if (v > peak) peak = v;
    }
    digitalWrite(STATUS_LED, LOW);

    Serial.print("rest = "); Serial.print(rest);
    Serial.print(" mV, peak = "); Serial.print(peak); Serial.println(" mV");

    if (peak - rest >= MIN_SPAN_MV) {
      thresholdMv = rest + (peak - rest) / 2.0;
      Serial.print("Threshold set to "); Serial.print(thresholdMv); Serial.println(" mV");
      return;
    }
    Serial.println("Signal too weak, check the electrode placement. Trying again.");
    delay(1000);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(STATUS_LED, OUTPUT);
  handServo.attach(SERVO_PIN);
  setHand(false);
  calibrate();
}

void loop() {
  float mv = readMv();

  if (!handClosed && mv > thresholdMv + HYSTERESIS_MV) setHand(true);
  else if (handClosed && mv < thresholdMv - HYSTERESIS_MV) setHand(false);

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 250) {
    lastPrint = millis();
    Serial.print("Voltage: "); Serial.print(mv);
    Serial.print(" mV  threshold: "); Serial.print(thresholdMv);
    Serial.println(handClosed ? "  CLOSED" : "  OPEN");
  }
  delay(10);
}
