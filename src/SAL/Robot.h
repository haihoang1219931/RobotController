#ifndef ROBOT_H
#define ROBOT_H

#include "StdTypes.h"

typedef enum {
  MOTOR_CAPTURE,
  MOTOR_ARM1,
  MOTOR_ARM2,
  MOTOR_ARM3,
  MOTOR_ARM4,
  MOTOR_ARM5,
} MOTOR;

typedef struct {
    int steps;
    int stepTime;
    int active;
} Joint;

typedef struct {
    Joint jointSteps[MAX_MOTOR];
} Move;

class ApplicationController;
class Motor;
class Robot
{
public:
    Robot(ApplicationController* app);
    void setMotorParam(int motorID, JointParam param);
    float armLength(int motorID);
    int currentStep(int motorID);
    void loop();
    void setState(ROBOT_STATE newState);
    void requestGoHome(int motorID = MAX_MOTOR);
    void executeGohome();
    void requestGoPosition(int motorID, int targetStep, int stepTime, bool isRelativeMove);
    void executeGoPosition();
    void resetMoveSequene();
    void appendMove(int* jointSteps);
    void moveSequence(int motorID = MAX_MOTOR);
    void executeMoveSequence();
    void initMove();
    void gotoTarget();
    void capture();
    long elapsedTime();
    void initDirection(int motorID, int direction);
    void moveStep(int motorID, int currentStep, int nextStep);
    bool isLimitReached(int motorID,
                        MOTOR_LIMIT_TYPE limitType);
    int homeStep(int motorID);
    int motorCurrentStep(int motorID);
    void setCurrentStep(int motorID, int step);
    void setDir(int motorID, int dir);
    int dir(int motorID);
    float angleToStep(int motorID, float angle);
    void currentStep(int* listCurrentStep, int* numMotor);
    void currentAngle(float* listCurrentStep, int* numMotor);
    void armLength(float* listArmLength, int* numMotor);

private:
    ApplicationController* m_app;
    Motor* m_motorList[MAX_MOTOR];
    ROBOT_STATE m_state;
    ROBOT_SEQUENCE_STATE m_sequenceState;
    Move m_moveSequence[MAX_MOVE_SEQUENCE];
    int m_curMove;
    int m_numMove;
    int m_numMotor;
    int m_requestMotorID;
    long m_startTime;
    long m_elapsedTime;
};

#endif // ROBOT_H
