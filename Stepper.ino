#include "ApplicationArduino.h"

ApplicationArduino* app;
void setup() {
  app = new ApplicationArduino();
}
void loop() {
  app->checkInput();
  app->loop();
}

// void moveToTargetPostion() {
//     if(!m_moveSequence[m_currentMoveID].moveInit) {
//         for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
//             m_motorList[axis].moveTo(m_moveSequence[m_currentMoveID].armAngle[axis]);
//         }
//         m_moveSequence[m_currentMoveID].moveInit = true;
//     }
//     bool allMotorsAreHome = true;
//     for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
//         if(m_motorList[axis].distanceToGo() != 0){
//             allMotorsAreHome = false;
//             m_motorList[axis].run();
//         }
//     }
    
//     if(allMotorsAreHome) {
//       delay(100);
//       m_currentMoveID++;
//     }
// }
// void setup() {
//   Serial.begin(115200);
//   printf("CNC Shield Initialized UART[%ld]\r\n",115200);
//   pinMode(enPin, OUTPUT);
//   digitalWrite(enPin, LOW);
//   // Set the maximum speed in steps per second:
//   m_motorList[MOTOR::MOTOR_BASE].setMaxSpeed(2000.0);
//   m_motorList[MOTOR::MOTOR_BASE].setAcceleration(2000.0);
//   // m_motorList[MOTOR::MOTOR_BASE].moveTo(200);
//   m_motorList[MOTOR::MOTOR_ARM1].setMaxSpeed(2000.0);
//   m_motorList[MOTOR::MOTOR_ARM1].setAcceleration(2000.0);
//   // m_motorList[MOTOR::MOTOR_ARM1].moveTo(100);
//   m_motorList[MOTOR::MOTOR_ARM2].setMaxSpeed(2000.0);
//   m_motorList[MOTOR::MOTOR_ARM2].setAcceleration(2000.0);
//   // m_motorList[MOTOR::MOTOR_ARM2].moveTo(100);

//   m_moveSequence[0] = {false,{0,0,0}};
//   m_moveSequence[1] = {false,{0,900,900}};
//   m_moveSequence[2] = {false,{0,700,700}};
//   m_moveSequence[3] = {false,{0,900,100}};
//   m_moveSequence[4] = {false,{0,1140,300}};
//   m_numberMove = 5;
//   m_currentMoveID = 0;
//   delay(2000);
//   Serial.println("Start");
// }

// void loop() { 
//   if(m_currentMoveID >= m_numberMove) {
//     m_currentMoveID = 0;
//     for(int i=0; i< m_numberMove; i++) {
//       m_moveSequence[i].moveInit = false;
//     }
//   }
//   moveToTargetPostion();
// }