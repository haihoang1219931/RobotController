#define DEBUG
#ifndef DEBUG
#include "ApplicationArduino.h"

ApplicationArduino* app;
void setup() {
  app = new ApplicationArduino();
}
void loop() {
  app->checkInput();
  app->loop();
}

#else

#include <AccelStepper.h>

// Define the stepper motor and the pins that is connected to
AccelStepper stepper1(1, 2, 5); // (Type of driver: with 2 pins, STEP, DIR)
float speed =  1000.0f;
// long maxPosition = (long)270.0f*13.7f*4.5f*200/360.0f;
long maxPosition = 1000;
int printf(const char *fmt, ...) {
    char m_buffer[64];
    va_list args;
    va_start(args, fmt);
    int rc = vsprintf(m_buffer, fmt, args);
    va_end(args);
    Serial.print(m_buffer);
    return rc;
}
void setup() {
  Serial.begin(115200);
  const int enPin=8;
  const int stepXPin = 2; //X.STEP
  const int dirXPin = 5; // X.DIR
  const int stepYPin = 3; //Y.STEP
  const int dirYPin = 6; // Y.DIR
  const int stepZPin = 4; //Z.STEP
  const int dirZPin = 7; // Z.DIR

  const int limitX = 9;
  const int limitY = 10;
  const int limitZ = 11;
  // Set maximum speed value for the stepper

  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  
  stepper1.setCurrentPosition(0);
  stepper1.moveTo(maxPosition);
  stepper1.setMaxSpeed(speed*2);
  stepper1.setSpeed(speed);
  Serial.println("POS");
}
bool printStop = true;
void loop() {
  Serial.println("POS");
  if(stepper1.distanceToGo() != 0){
    stepper1.runSpeed();
    if(printStop) {
      printStop = false;
    }
  } else {
    if(!printStop) {
      printStop = true;
      speed = -speed;
      Serial.println(stepper1.currentPosition());
      stepper1.moveTo(maxPosition - stepper1.currentPosition());
      stepper1.setMaxSpeed(speed*2);
      Serial.print("Speed:");
      Serial.println(speed);
      stepper1.setSpeed(speed);
      delay(1000);
    }
  }
  delay(1);
}
#endif
// Define pin connections & motor's steps per revolution
// const int enPin = 8;
// const int dirPin = 5;
// const int stepPin = 2;
// const int stepsPerRevolution = 2000;
// int sleepTime = 500;
// void setup()
// {
//   pinMode(enPin, OUTPUT);
//     digitalWrite(enPin, LOW);
// 	// Declare pins as Outputs
// 	pinMode(stepPin, OUTPUT);
// 	pinMode(dirPin, OUTPUT);
// }
// void loop()
// {
// 	// Set motor direction clockwise
// 	digitalWrite(dirPin, HIGH);
//   delay(1000); // Wait a second
//   digitalWrite(dirPin, HIGH);
// 	// Spin motor slowly
// 	for(int x = 0; x < stepsPerRevolution; x++)
// 	{
// 		digitalWrite(stepPin, HIGH);
// 		delayMicroseconds(sleepTime);
// 		digitalWrite(stepPin, LOW);
// 		delayMicroseconds(sleepTime);
// 	}
// 	delay(1000); // Wait a second
	
// 	// Set motor direction counterclockwise
// 	digitalWrite(dirPin, LOW);
//   delay(1000); // Wait a second
//   digitalWrite(dirPin, LOW);
// 	// Spin motor quickly
// 	for(int x = 0; x < stepsPerRevolution; x++)
// 	{
// 		digitalWrite(stepPin, HIGH);
// 		delayMicroseconds(sleepTime);
// 		digitalWrite(stepPin, LOW);
// 		delayMicroseconds(sleepTime);
// 	}
// 	delay(1000); // Wait a second
// }

// int incomingByte = 0; // for incoming serial data

//     void setup() {
//       Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
//     }

//     void loop() {
//       // send data only when you receive data:
//       if (Serial.available() > 0) {
//         // read the incoming byte:
//         incomingByte = Serial.read();

//         // say what you got:
//         Serial.print("I received: ");
//         Serial.println(incomingByte, DEC);
//       }
//     }


// #define IN1	42
// #define IN2	44
// #define IN3	5
// #define IN4	4
// #define MAX_SPEED 150 //từ 0-255
// #define MIN_SPEED 0
// void setup()
// {
// 	pinMode(IN1, OUTPUT);
// 	pinMode(IN2, OUTPUT);
// 	pinMode(IN3, OUTPUT);
// 	pinMode(IN4, OUTPUT);
// }
 
// void motor_1_Dung() {
// 	digitalWrite(IN1, LOW);
// 	digitalWrite(IN2, LOW);
// }
 
// void motor_1_Tien(int speed) { //speed: từ 0 - MAX_SPEED
// 	speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
// 	digitalWrite(IN1, HIGH);// chân này không có PWM
// 	analogWrite(IN2, 255 - speed);
// }
 
// void motor_1_Lui(int speed) {
// 	speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
// 	digitalWrite(IN1, LOW);// chân này không có PWM
// 	analogWrite(IN2, speed);
// }

// void loop()
// {
// 	motor_1_Tien(MAX_SPEED); // motor 1 tiến
// 	delay(2000);//tiến 5 s
//   motor_1_Dung();
// 	delay(1000);//dừng 1s
// 	motor_1_Lui(MAX_SPEED); //motor 2 lùi
// 	delay(2000);//tiến 2 s
	
// }
// #define DEBUG_SERVO
// #ifndef DEBUG_SERVO
// #include <Arduino.h>
// #include "Servo_driver.h"
// Servo_driver* servoDriver = new Servo_driver(45);
// void setup() {
//   Serial.begin(38400);
//   servoDriver->write(55);
//   servoDriver->setCurrentPosition(55);
//   servoDriver->setSpeed(15.0f);
//   servoDriver->moveTo(100);
// }
// void loop() {
  
//   servoDriver->runSpeed();
// }
// #else
// #include <Arduino.h>
// #include <Servo.h>

// Servo servo;  // create servo object to control a servo

// void setup() {
//   servo.attach(45);  // attaches the servo on pin 9 to the servo objectư
//   servo.write(0);   // rotate slowly servo to 0 degrees immediately
// }

// void loop() {
//   servo.write(100);
//   delay(1000);
//   servo.write(55);
//   delay(1000);
// }
// #endif
