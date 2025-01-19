#include "ApplicationArduino.h"

ApplicationArduino* app;
void setup() {
  app = new ApplicationArduino();
}
void loop() {
  app->checkInput();
  app->loop();
}

// #include <AccelStepper.h>

// int Stepper1Pulse = 2;  // **** for CNC shield
// int Stepper1Direction = 5;  // **** for CNC shield
// const byte enablePin = 8;   // **** for CNC shield
// int speedpot = A0;

// int Motor1speed = -100;
// int speedmin = 0;
// int speedmax = 4000;
// AccelStepper step1(1, Stepper1Pulse, Stepper1Direction);

// void setup()
// {
//    step1.setMaxSpeed (speedmax);
//    step1.setSpeed(0);
//    step1.setAcceleration(500);
//    pinMode(enablePin, OUTPUT);  // **** for CNC shield
//    digitalWrite(enablePin, LOW);   // **** for CNC shield
//    Serial.begin(115200); // ************** faster baud rate
//    Serial.println("Running: StepperDriverTest");
//    step1.setSpeed(Motor1speed);
// }

// void loop()
// {
//       step1.runSpeed();
// }