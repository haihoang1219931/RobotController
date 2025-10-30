#ifndef ROBOT_H
#define ROBOT_H

#include "StdTypes.h"

typedef struct {
    int jointSteps[MAX_MOTOR];
    int captureStep;
} Move;

class ApplicationController;
class Motor;
class Robot
{
public:
    Robot(ApplicationController* app,int numMotor = 0);
    void loop();
    void setState(ROBOT_STATE newState);
    void goHome();
    void executeGohome();
    int getNumMotor();
    void goToPosition(int* stepList, int numMotor, MOTION_SPACES motionSpace = MOTION_JOINT_SPACE);
    void executeGoToPosition();
    void resetMoveSequene();
    void appendMove(int* jointSteps, int captureStep);
    void moveSequence(int numMotor);
    void executeMoveSequence();
    void initMove();
    void gotoTarget();
    void capture();
    long elapsedTime();
    void initDirection(int motorID, int direction);
    void moveStep(int motorID, int currentStep, int nextStep);
    bool isLimitReached(int motorID,
                        MOTOR_LIMIT_TYPE limitType);
    void getCurrentPosition(int* listCurrentStep, int* numMotor, float* captureStep);
    Motor* getMotor(int motorID);
private:
    ApplicationController* m_app;
    Motor* m_motorList[MAX_MOTOR];
    Motor* m_capture;
    ROBOT_STATE m_state;
    ROBOT_SEQUENCE_STATE m_sequenceState;
    Move m_moveSequence[MAX_MOVE_SEQUENCE];
    int m_curMove;
    int m_numMove;
    int m_numMotor;
    int m_executeNumMotor;
    long m_startTime;
    long m_elapsedTime;
};

#endif // ROBOT_H
