#ifndef ROBOT_H
#define ROBOT_H

#include "ApplicationController.h"

#define MOTOR_MAX_SEQUENCE 20

enum ROBOT_STATE {
    ROBOT_INIT,
    ROBOT_GO_HOME,
    ROBOT_EXECUTE_SEQUENCE,
};

typedef struct {
    bool moveInit;
    float armAngle[MOTOR::MOTOR_MAX];
    int crawlAngle;
} MoveLocation;

class ApplicationController;
class Robot
{
public:
    Robot(ApplicationController* app);
    void loop();
    void setState(ROBOT_STATE newState);
    void goHome();
    void goToPosition(int startCol, int startRow, int stopCol, int stopRow, bool attack = false, bool castle = false, char promote = 0);
    void executeGohome();
    void executeGoToPosition();
private:
    ApplicationController* m_app;
    int m_numberMove = 0;
    int m_currentMoveID = 0;
    MoveLocation m_moveSequence[MOTOR_MAX_SEQUENCE];
    ROBOT_STATE m_state;
};

#endif // ROBOT_H
