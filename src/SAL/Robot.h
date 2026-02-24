#ifndef ROBOT_H
#define ROBOT_H

#include "StdTypes.h"

typedef struct {
    Joint jointSteps[MAX_MOTOR];
} Move;

class ApplicationController;
class Robot
{
public:
    Robot(ApplicationController* app);
    void setMotorParam(int motorID, JointParam param);
 
    void loop();
    void setState(ROBOT_STATE newState);
    void requestGoHome(int motorID = MAX_MOTOR);
    void executeGohome();
    void requestGoPosition(int motorID, int targetStep, int stepTime, bool isRelativeMove);
    void resetMoveSequene();
    void appendMove(int* jointSteps);
    void moveSequence(int motorID = MAX_MOTOR);
    long elapsedTime();
    int angleToStep(int motorID, float angle);
    float stepToAngle(int motorID, int step);
    void currentStep(int* listCurrentStep, int* numMotor);
    void currentAngle(float* listCurrentStep, int* numMotor);
    void armLength(float* listArmLength, int* numMotor);
    
    float armLength(int motorID);
    int currentStep(int motorID);
    int minStep(int motorID);
    int maxStep(int motorID);
    float homeAngle(int motorID);

private:
    ApplicationController* m_app;
    // Motor* m_motorList[MAX_MOTOR];
    JointParam m_motorParamList[MAX_MOTOR];
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
