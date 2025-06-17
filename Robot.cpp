#include "Arduino.h"
#include "HardwareSerial.h"
#include "Robot.h"
#include "Button.h"
#include "ApplicationController.h"
#include <math.h>
Robot::Robot(ApplicationController* app) :
    m_app(app),
    m_state(ROBOT_INIT)
{
    m_distanceToChessBoard = 73.0f; // mm
    m_chessBoardSquareLength = 31.5f; // mm
    m_crawlLength = 38.0f;
    m_crawlHeight = 132.0f;
    m_chessHeight = 24.0f;
    m_armHomeSpeed[MOTOR::MOTOR_BASE] = -1000.0f;
    m_armHomeSpeed[MOTOR::MOTOR_ARM1] = -50.0f;
    m_armHomeSpeed[MOTOR::MOTOR_ARM2] = 15.0f;
    m_armHomeSpeed[MOTOR::MOTOR_ARM3] = 1.0f;
    m_armRatio[MOTOR::MOTOR_BASE] = 14.0f*4.50f*200.0f/360.0f;
    m_armRatio[MOTOR::MOTOR_ARM1] = 2.0f*200.0f/360.0f;
    m_armRatio[MOTOR::MOTOR_ARM2] = 1.0f;
    m_armRatio[MOTOR::MOTOR_ARM3] = 1.0f;
    m_armMaxPosition[MOTOR::MOTOR_BASE] = (long)(120.0f*14.0f*4.5*200.0f/360.0f);
    m_armMaxPosition[MOTOR::MOTOR_ARM1] = (long)(270.0f*2.0f*200.0f/360.0f);
    m_armMaxPosition[MOTOR::MOTOR_ARM2] = 100.0f;
}

void Robot::setState(ROBOT_STATE newState) {
  if(m_state != newState) {
    m_state = newState;
    m_app->printf("ROBOT STATE: %d\r\n",m_state);
  }
}

void Robot::loop() {
  switch(m_state) {
    case ROBOT_GO_HOME: {
      executeGohome();
    }
    break;
    case ROBOT_EXECUTE_SEQUENCE: {
      executeGoToPosition();
    }
    break;
    case ROBOT_EXECUTE_ROTATE_COMMAND: {
      executeRotateAngle();
    }
    break;
  }
}
void Robot::goHome() {
  m_app->printf("GO HOME\r\n");
  for(int i=0; i< MOTOR_MAX; i++)
      m_app->printf("MAX POS[%d] %d\r\n",i,m_armMaxPosition[i]);
  for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
    m_app->setHomePosition(motor);
    m_app->setMaxSpeed(motor,m_armHomeSpeed[motor]);
    m_app->setSpeed(motor,m_armHomeSpeed[motor]);
    m_app->printf("MOTOR[%d] POS[%ld] \r\n",motor,m_app->currentPosition(motor)); 
  }
  setState(ROBOT_GO_HOME);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
  m_app->msleep(100);
}
void Robot::resetMoveSequene() {
  m_numberMove = 0;
  m_currentMoveID = 0;
}

void Robot::appendMoveSequence(long* armAngle,int angleCount) {
  if(m_numberMove >= MOTOR_MAX_SEQUENCE) return;
  float moveTime = 0;
  for(int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
    float angleToMove = fabs((float)(armAngle[motor] - m_app->currentPosition(motor)));
    float maxSpeed = fabs((float)(m_armHomeSpeed[motor]));
    if(angleToMove/maxSpeed > moveTime) moveTime = angleToMove/maxSpeed;
    
  }
  moveTime = pow(2, (int)(log(moveTime)+1));
  m_moveSequence[m_numberMove].moveInit = false;
  for(int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
    float angleToMove = fabs((float)(armAngle[motor] - m_app->currentPosition(motor)));
    m_moveSequence[m_numberMove].armAngle[motor] = armAngle[motor];
    m_moveSequence[m_numberMove].armSpeed[motor] = angleToMove/moveTime;
    Serial.print("Motor[");
    Serial.print(motor);
    Serial.print("]");
    Serial.print(m_moveSequence[m_numberMove].armAngle[motor]);
    Serial.print(" ");
    Serial.print(m_moveSequence[m_numberMove].armSpeed[motor]);
    Serial.print(" ");
    Serial.println(moveTime);
  }
  m_numberMove++;
}
void Robot::ablsoluteAngle(long angleBase, long angleArm1, long angleArm2, long angleServo) {
  long armAngle[MOTOR::MOTOR_MAX] = {angleBase,angleArm1,angleArm2,angleServo};
  resetMoveSequene();
  appendMoveSequence(armAngle,MOTOR::MOTOR_MAX);
  setState(ROBOT_EXECUTE_SEQUENCE);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}

void Robot::rotateAngle(MOTOR motorID, long angle, float speed) {
  m_motorID = motorID;
  m_app->printf("MOVE MOTOR[%d] to POS[%ld] SPEED[%ld]\r\n", motorID, angle, speed);
  m_app->setTargetPos(motorID,angle);
  m_app->setSpeed(motorID, fabs(speed)*(m_app->currentPosition(motorID) < angle ? 1.0:-1.0));
  setState(ROBOT_EXECUTE_ROTATE_COMMAND);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}

void Robot::executeGohome() {
  bool allMotorsAtHome = true;
  for(unsigned int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
    if(!m_app->isMotorHomed(motor)) {
      allMotorsAtHome = false;
      m_app->goHome(motor);
    }
  }
  
  if(allMotorsAtHome) {
    for(unsigned int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
      m_app->setHomePosition(motor);
    }
    m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
  }
}
    
void Robot::goToPosition(int startCol, int startRow, int stopCol, int stopRow, bool attack, bool castle, char promote) {
  m_app->printf("Go to Pos [%d,%d] to [%d,%d] attack[%s] castle[%s] promote[%c]\r\n",
                  startCol, startRow, stopCol, stopRow, attack?"yes":"no", castle?"yes":"no", promote);
  int centerCol = 10;
  int centerRow = 0;
  float squareLength = 20.0f;
  float xPos = (float)(startCol-centerCol) * squareLength;
  float yPos = (float)(startRow-centerRow) * squareLength;
  float upAngle = 45.0f/180.0f*PI;
  float a1 = 47.6f + 228.0f;
  float a2Sin = 55.2f;
  float a2Cos = 50.1f + 84.3f*cos(upAngle)+21.8f+14.4f;
  float a2 = (float)sqrt(a2Sin*a2Sin + a2Cos*a2Cos);
  float q2Offset = atan(a2Sin/a2Cos);
  float q1 = 0;
  float q2 = 0;
  m_app->printf("xPos[%d]\r\n",(int)xPos);
  m_app->printf("yPos[%d]\r\n",(int)yPos);
  m_app->printf("a1[%d]\r\n",(int)a1);
  m_app->printf("a2[%d]\r\n",(int)a2);
  calculateRobotArm(xPos, yPos, a1, a2, &q1, &q2);
  m_app->printf("arm1Angle[%d] arm2Angle[%d]=[%d] q2Offset[%d]\r\n",
  (int)(q1/PI*180.0f),
  (int)((q2)/PI*180.0f),
  (int)((PI-q2-q2Offset)/PI*180.0f),
  (int)(q2Offset/PI*180.0f));
  long armAngle[MOTOR::MOTOR_MAX] = {
    (long)(q1/PI*180.0f*m_armRatio[MOTOR_BASE]),
    (long)((PI-q2-q2Offset)/PI*180.0f*m_armRatio[MOTOR_ARM1]),
    (long)(upAngle/PI*180.0f*m_armRatio[MOTOR_ARM2]),
    0};
  resetMoveSequene();
  appendMoveSequence(armAngle,MOTOR::MOTOR_MAX);
  setState(ROBOT_EXECUTE_SEQUENCE);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}
void Robot::executeGoToPosition() {
  if(!m_moveSequence[m_currentMoveID].moveInit) {
      for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
          long currentPos = m_app->currentPosition(motor);
          m_app->printf("goto motor[%d] %d/%d\r\n",
          motor,(int)currentPos,(int)m_moveSequence[m_currentMoveID].armAngle[motor]);
          m_app->setTargetPos(motor,m_moveSequence[m_currentMoveID].armAngle[motor]);
          m_app->setMaxSpeed(motor, fabs(m_moveSequence[m_currentMoveID].armSpeed[motor])*(m_app->currentPosition(motor) < m_moveSequence[m_currentMoveID].armAngle[motor] ? 10.0f:-10.0f));
          m_app->setSpeed(motor, fabs(m_moveSequence[m_currentMoveID].armSpeed[motor])*(m_app->currentPosition(motor) < m_moveSequence[m_currentMoveID].armAngle[motor] ? 1.0f:-1.0f));
          Serial.println(m_app->speed(motor));
          Serial.println(m_moveSequence[m_currentMoveID].armAngle[motor]);
      }
      m_moveSequence[m_currentMoveID].moveInit = true;
  }
  bool allMotorsAtTarget = true;
  for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
      if(!m_app->isMoveDone(motor)){
          allMotorsAtTarget = false;
          m_app->runSpeed(motor);
      }else{
        // m_app->printf("MOTOR[%d] pos move done\r\n",motor);
      }
  }
  if(allMotorsAtTarget) {
    m_app->msleep(100);
    m_currentMoveID++;
  }

  if(m_currentMoveID >= m_numberMove) {
    for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
      m_app->printf("goto done motor[%d] %d/%d\r\n",
          motor,(int)(m_moveSequence[m_currentMoveID].armAngle[motor]),(int)(m_app->currentPosition(motor)));
    }
    m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
  }
}

void Robot::executeRotateAngle() {
  if(m_app->buttonState((BUTTON_ID)m_motorID) != BUTTON_STATE::BUTTON_NOMAL && m_app->speed(m_motorID) < 0) {
    m_app->printf("MOTOR[%d] at limit inverse speed\r\n", m_motorID);
    m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
  } else {
    if(!m_app->isMoveDone(m_motorID)) {
      m_app->runSpeed(m_motorID);
    } else {
      m_app->printf("MOTOR[%d] reached[%ld]\r\n", m_motorID, m_app->currentPosition(m_motorID));
      m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
    }
  }
}

void Robot::calculateRobotArm(float x, float y, float a1, float a2, float* p1, float* p2) {
  *p2 = acos((x*x+y*y-a1*a1-a2*a2)/(2*a1*a2));
  *p1 = atan(y/x) - atan((a2*sin(*p2))/(a1+a2*cos(*p2)));
  *p1 =  *p1 < 0?*p1+PI:*p1;
  m_app->printf("x[%d] y[%d] a1[%d] a2[%d] q1[%d] q2[%d]\r\n", 
                (int)x,(int)y,(int)a1,(int)a2,(int)(*p1/M_PI*180.0f),(int)(*p2/M_PI*180.0f));
}