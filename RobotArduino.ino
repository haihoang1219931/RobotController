// #define DEBUG
#ifndef DEBUG
#include "SAL/ApplicationArduino.h"

ApplicationArduino* app;
void setup() {
  app = new ApplicationArduino();
  delay(1000);
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
// #define DEBUG_DC
// #ifndef DEBUG_DC
// #include "DC_driver.h"
// #define IN1	42
// #define IN2	44
// #define IN3	5
// #define IN4	4
// #define MAX_SPEED 150 //từ 0-255
// #define MIN_SPEED 0

// DC_driver* dcDriver = new DC_driver(IN1,IN2);

// void setup()
// {
//   int lastValue = 0;
//   Serial.begin(38400);
//   dcDriver->setSpeed(1000);
//   dcDriver->setCurrentPosition(500);
//   dcDriver->moveTo(lastValue);
//   Serial.println("===========Start");
//   while(1) {
//     if(dcDriver->isFinished()){
//       lastValue = 500 - lastValue;
//       dcDriver->moveTo(lastValue);
//       // break;
//     } else {
//       dcDriver->runSpeed();
//     }
//     // delay(1000);
//   }
//   Serial.println("Finished========");
// }

// void loop()
// {
  
// }
// #else
// #include <Arduino.h>
// int speedPin = 3;
// int dirPin1 = 4;
// int dirPin2 = 5;
// void setup() {
//   pinMode(speedPin, OUTPUT);
//   pinMode(dirPin1, OUTPUT);
//   pinMode(dirPin2, OUTPUT);

//   analogWrite(speedPin,255);
//   digitalWrite(dirPin1,LOW);
//   digitalWrite(dirPin2,HIGH);
// }
// void loop() {

// }
// #endif

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
//   Serial.println("Ready");
//   delay(1000);
// }
// void loop() {
//   // if(!servoDriver->isFinished()) 
//   {
//     servoDriver->runSpeed();
//     Serial.println("Runspeed");
//   } 
//   // else {
//   //   Serial.println("Done");
//   // }
//   delay(1000);
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
