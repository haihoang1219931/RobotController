#include "ApplicationArduino.h"

ApplicationArduino* app;
void setup() {
  app = new ApplicationArduino();
}
void loop() {
  app->checkInput();
  app->loop();
}

// #include <Servo.h>

// Servo servo;  // create servo object to control a servo

// void setup() {
//   servo.attach(13);  // attaches the servo on pin 9 to the servo objectư
//   servo.write(0);   // rotate slowly servo to 0 degrees immediately
// }

// void loop() {
//   // for (int angle = 0; angle <= 100; angle += 1) {  // rotate slowly from 0 degrees to 180 degrees, one by one degree
//   //   // in steps of 1 degree
//   //   servo.write(angle);  // control servo to go to position in variable 'angle'
//   //   delay(1);         // waits 10ms for the servo to reach the position
//   // }

//   // for (int angle = 100; angle >= 0; angle -= 1) {  // rotate from 180 degrees to 0 degrees, one by one degree
//   //   servo.write(angle);                        // control servo to go to position in variable 'angle'
//   //   delay(1);                               // waits 10ms for the servo to reach the position
//   // }
//   servo.write(100);
//   delay(1000);
//   servo.write(0);
//   delay(1000);
// }