#include <Servo.h>
Servo myservo;  // create servo object to control a servo
int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void forward() {
  digitalWrite(LeftMotorPower, HIGH);
  digitalWrite(LeftMotorDirection, HIGH);
  digitalWrite(RightMotorPower, HIGH);
  digitalWrite(RightMotorDirection, HIGH);
}

void backward() {
  digitalWrite(LeftMotorPower, HIGH);
  digitalWrite(LeftMotorDirection, LOW);
  digitalWrite(RightMotorPower, HIGH);
  digitalWrite(RightMotorDirection, LOW);
}

void setup() {
  myservo.attach(4);
  Serial.begin(9600);
}

void loop() {
  float sensorValue = analogRead(A4);
  float millivolt = (sensorValue/1023)*5;
  
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  // val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  val = map(0, 0, 0, 10, 0);
  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(30);    
  
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  
  Serial.print("Voltage: ");
  Serial.print(millivolt*1000);
  Serial.println(" mV");
  Serial.println("");
  delay(250);  
  
  if((millivolt * 1000) > lowMiliVoltValue){
    forward();
  }else{
    digitalWrite(LeftMotorPower, LOW);
    digitalWrite(RightMotorPower, LOW);
  }
}
