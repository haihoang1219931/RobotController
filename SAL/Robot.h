#ifndef ROBOT_H
#define ROBOT_H

#include "StdTypes.h"

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
    void goToPosition(int* stepList, int numMotor);
    void executeGoToPosition();
    long elapsedTime();
    void moveStep(int motorID);
    bool isLimitReached(int motorID,
                        MOTOR_LIMIT_TYPE limitType);
    void getCurrentPosition(int* listCurrentStep, int* numMotor);
    Motor* getMotor(int motorID);
private:
    ApplicationController* m_app;
    Motor* m_motorList[MAX_MOTOR];
    ROBOT_STATE m_state;
    int m_numMotor;
    int m_executeNumMotor;
    long m_startTime;
    long m_elapsedTime;
};

#endif // ROBOT_H
