#include <Servo.h>  // Include the Servo library

// Pin declaration 
int motorA_direction = 2;
int motoA_speed = 5;
int motorB_direction = 4;
int motoB_speed = 6;
int servo_rotation = 10;
int intrared_reciever = 3;
int light_sensorleft = A0;
int light_sensorright = A3;
int Obstacle_avoidanceleft = 8;
int Obstacle_avoidanceright = 11;
int Line_trackingleft = 7;
int Line_trackingright = 9;
int ultrasonic_trigger = 12;
int ultrasonic_echi = 13;

int flashlight_threshold_right = 180;
int flashlight_threshold_left = 130;

Servo myServo;  // Create a servo object

void setup() {
  pinMode(motorA_direction, OUTPUT);
  pinMode(motoA_speed, OUTPUT);
  pinMode(motorB_direction, OUTPUT);
  pinMode(motoB_speed, OUTPUT);
  pinMode(servo_rotation, OUTPUT);
  pinMode(intrared_reciever, INPUT);
  pinMode(A2, INPUT);
  pinMode(Line_trackingleft, INPUT);
  pinMode(Line_trackingright, INPUT);
  pinMode(ultrasonic_trigger, OUTPUT);
  pinMode(ultrasonic_echi, INPUT);
  pinMode(light_sensorleft, INPUT);
  pinMode(light_sensorright, INPUT);

  pinMode(Obstacle_avoidanceleft, INPUT);
  pinMode(Obstacle_avoidanceright, INPUT);

  Serial.begin(9600);  // Start serial communication

  // Attach and rotate the servo at startup
  myServo.attach(servo_rotation);
  myServo.write(0);      // Move to 0 degrees
  delay(1000);           // Wait for 1 second
  myServo.write(180);    // Move to 180 degrees
  delay(1000);           // Wait for 1 second
  myServo.write(90);     // Return to 90 degrees
  delay(500);            // Hold for 0.5 second
  myServo.detach();      // Stop spinning after the one-time motion
}

void loop() {
  int leftSensor = analogRead(light_sensorleft);  // Read left light sensor
  int rightSensor = analogRead(light_sensorright); // Read right light sensor
  int lineLeft = digitalRead(Line_trackingleft);   // Read left line tracking sensor
  int lineRight = digitalRead(Line_trackingright); // Read right line tracking sensor

  Serial.print("Left:");
  Serial.print(leftSensor);
  Serial.print("\t");

  Serial.print("Right:");
  Serial.println(rightSensor);

  // Read distance from ultrasonic sensor
  long distance = getUltrasonicDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Stop robot if an object is closer than 5 cm
  if (distance <= 5 && distance > 0) {
    stopMotor();
    return;  // Skip rest of the loop if object is detected
  }

  // Line tracking logic
  if (lineLeft == 1 && lineRight == 1) {
    stopMotor();
  } else {
    // Flashlight following logic
    if (leftSensor < flashlight_threshold_left || rightSensor < flashlight_threshold_right) {
      int dif = abs(leftSensor - rightSensor);
      if ((leftSensor < rightSensor) && (dif > 30)) {
        moveLeft(50);
      } else if ((leftSensor > rightSensor) && (dif > 30)) {
        moveRight(50);
      } else {
        moveFoward(50);
      }
    } else {
      stopMotor();
    }
  }

  delay(100);  // Short delay to avoid fast flickering
}

void moveBack(int speed) {
  digitalWrite(motorA_direction, LOW);
  analogWrite(motoA_speed, speed);
  digitalWrite(motorB_direction, HIGH);
  analogWrite(motoB_speed, speed);
}

void moveFoward(int speed) {
  digitalWrite(motorA_direction, HIGH);
  analogWrite(motoA_speed, speed);
  digitalWrite(motorB_direction, LOW);
  analogWrite(motoB_speed, speed);
}

void moveRight(int speed) {
  digitalWrite(motorA_direction, HIGH);
  analogWrite(motoA_speed, speed);
  digitalWrite(motorB_direction, HIGH);
  analogWrite(motoB_speed, speed);
}

void moveLeft(int speed) {
  digitalWrite(motorA_direction, LOW);
  analogWrite(motoA_speed, speed);
  digitalWrite(motorB_direction, LOW);
  analogWrite(motoB_speed, speed);
}

void stopMotor() {
  analogWrite(motoA_speed, 0);
  analogWrite(motoB_speed, 0);
}

void line_tracking(int speed) {
  int lineLeft = digitalRead(Line_trackingleft);  // Prevented local variable shadowing
  int lineRight = digitalRead(Line_trackingright);

  Serial.print("Line Left:\t");
  Serial.print(lineLeft);
  Serial.print("\t(");
  if (lineLeft == 1) {
    Serial.print("LINE DETECTED");
  } else {
    Serial.print("NO LINE");
  }
  Serial.print(")");

  Serial.print("\tLine Right:\t");
  Serial.print(lineRight);
  Serial.print("\t(");
  if (lineRight == 1) {
    Serial.print("LINE DETECTED");
  } else {
    Serial.print("NO LINE");
  }
  Serial.println(")");

  delay(500);  // Wait for 500ms before reading again
}

// Measure distance using ultrasonic sensor
long getUltrasonicDistance() {
  digitalWrite(ultrasonic_trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonic_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonic_trigger, LOW);

  long duration = pulseIn(ultrasonic_echi, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}
