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
    m_armLength[MOTOR::MOTOR_BASE] = (130.0f);
    m_armLength[MOTOR::MOTOR_ARM1] = (139.0f);
    m_armHomeSpeed[MOTOR::MOTOR_BASE] = -1000.0f;
    m_armHomeSpeed[MOTOR::MOTOR_ARM1] = -100.0f;
    m_armHomeSpeed[MOTOR::MOTOR_ARM2] = 15.0f;
    m_armHomeSpeed[MOTOR::MOTOR_ARM3] = 1.0f;
    m_armRatio[MOTOR::MOTOR_BASE] = 14.0f*4.50f;
    m_armRatio[MOTOR::MOTOR_ARM1] = 2.0f;
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

void Robot::ablsoluteAngle(long angleBase, long angleArm1, long angleArm2, long angleServo) {
  float moveTime = 0;
  long armAngle[MOTOR::MOTOR_MAX] = {angleBase,angleArm1,angleArm2,angleServo};
  for(int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
    float angleToMove = fabs((float)(armAngle[motor] - m_app->currentPosition(motor)));
    float maxSpeed = fabs((float)(m_armHomeSpeed[motor]));
    if(angleToMove/maxSpeed > moveTime) moveTime = angleToMove/maxSpeed;
    Serial.print("Motor[");
    Serial.print(motor);
    Serial.print("]");
    Serial.print(angleToMove);
    Serial.print(" ");
    Serial.print(maxSpeed);
    Serial.print(" ");
    Serial.println(moveTime);
  }
  moveTime = pow(2, (int)(log(moveTime)));
  m_moveSequence[0].moveInit = false;
  for(int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
    float angleToMove = fabs((float)(armAngle[motor] - m_app->currentPosition(motor)));
    m_moveSequence[0].armAngle[motor] = armAngle[motor];
    m_moveSequence[0].armSpeed[motor] = angleToMove/moveTime;
  }
  m_numberMove = 1;
  m_currentMoveID = 0;

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
    // for(unsigned int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
    //   m_app->printf("MOTOR[%d] home at[%ld]\r\n",
    //     motor,m_app->currentPosition(motor));
    // }
    for(unsigned int motor = 0; motor < MOTOR::MOTOR_MAX; motor ++) {
      m_app->setHomePosition(motor);
    }
    m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
  }
}
    
void Robot::goToPosition(int startCol, int startRow, int stopCol, int stopRow, bool attack, bool castle, char promote) {
  m_app->printf("Go to Pos [%d,%d] to [%d,%d] attack[%s] castle[%s] promote[%c]\r\n",
                  startCol, startRow, stopCol, stopRow, attack?"yes":"no", castle?"yes":"no", promote);

  float xCurPos = ((float)startCol-4.0f+0.5f) * m_chessBoardSquareLength;
  float yCurPos = ((float)startRow+0.5f) * m_chessBoardSquareLength + m_distanceToChessBoard;
  
  float curAngleArm1;
  float curAngleArm2;
  float curAngleArm3;
  // calculateRobotArm(float x, float y, float z, float L, float M, float N, float* q1, float* q2, float* q3);
  calculateRobotArm(xCurPos, yCurPos, m_crawlHeight + m_chessHeight - m_armLength[MOTOR::MOTOR_BASE],
          m_armLength[MOTOR::MOTOR_ARM1],m_armLength[MOTOR::MOTOR_ARM2],m_crawlLength,
          &curAngleArm1,&curAngleArm2,&curAngleArm3);

  float xTgtPos = ((float)stopCol-4.0f+0.5f) * m_chessBoardSquareLength;
  float yTgtPos = ((float)stopRow+0.5f) * m_chessBoardSquareLength + m_distanceToChessBoard;
  float tgtAngleArm1;
  float tgtAngleArm2;
  float tgtAngleArm3;
  // calculateRobotArm(float x, float y, float z, float L, float M, float N, float* q1, float* q2, float* q3);
  calculateRobotArm(xTgtPos, yTgtPos, m_crawlHeight + m_chessHeight - m_armLength[MOTOR::MOTOR_BASE],
          m_armLength[MOTOR::MOTOR_ARM1],m_armLength[MOTOR::MOTOR_ARM2],m_crawlLength,
          &tgtAngleArm1,&tgtAngleArm2,&tgtAngleArm3);
  m_app->printf("ARMAngle Base[%d] arm1[%d] arm2[%d]\r\n",
      (int)(curAngleArm1*180.0f/M_PI),
      (int)(curAngleArm2*180.0f/M_PI),
      (int)(curAngleArm3*180.0f/M_PI));
  return;
  m_numberMove = 8;
  m_currentMoveID = 0;

  for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
    m_app->setMaxSpeed(motor, 2000.0);
    m_app->setAcceleration(motor, 500.0);
  }

  setState(ROBOT_EXECUTE_SEQUENCE);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}
void Robot::executeGoToPosition() {
  if(!m_moveSequence[m_currentMoveID].moveInit) {
      for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
          long currentPos = m_app->currentPosition(motor);
          m_app->printf("goto motor[%d] %d/%ld\r\n",
          motor,currentPos,m_moveSequence[m_currentMoveID].armAngle[motor]);
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
      }
  }
  if(allMotorsAtTarget) {
    m_app->msleep(100);
    m_currentMoveID++;
  }

  if(m_currentMoveID >= m_numberMove) {
    for(unsigned int motor=0; motor < (unsigned int)MOTOR::MOTOR_MAX; motor++) {
      m_app->printf("goto done motor[%d] %d/%d\r\n",
          motor,(int)m_moveSequence[m_currentMoveID].armAngle[motor],(int)m_app->currentPosition(motor));
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

void Robot::calculateRobotArm(float x, float y, float z, float L, float M, float N, float* q1, float* q2, float* q3) {
  float R = sqrt(x*x+y*y);
  float s = R - N;
  float Q = sqrt(s*s + z*z);
  float f = atan2(z, s);
  float g = acosf((L*L + Q*Q - M*M) / (2*L*Q));
  float a = f + g;
  float b = acos(( M*M + L*L - Q*Q) / (2*L*M));
  float d = atan2 (x, y);
  m_app->printf("x[%d] y[%d] z[%d] L[%d] M[%d] N[%d] R[%d] Q[%d] f[%d] g[%d]\r\n", 
                (int)x, (int)y, (int)z, (int)L, (int)M, (int)N, 
                (int)R,int(Q),(int)(f*M_PI/180.0f),(int)(g*M_PI/180.0f));
  *q1 = d;
  *q2 = a;
  *q3 = b;
}