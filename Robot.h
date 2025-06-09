#ifndef ROBOT_H
#define ROBOT_H

#include "ApplicationController.h"

#define MOTOR_MAX_SEQUENCE 20

enum ROBOT_STATE {
    ROBOT_INIT,
    ROBOT_GO_HOME,
    ROBOT_EXECUTE_SEQUENCE,
    ROBOT_EXECUTE_ROTATE_COMMAND,
};

typedef struct {
    bool moveInit;
    long armAngle[MOTOR::MOTOR_MAX];
    float armSpeed[MOTOR::MOTOR_MAX];
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
    void ablsoluteAngle(long angleBase, long angleArm1, long angleArm2, long angleServo);
    void rotateAngle(MOTOR motorID, long angle, float speed);
    void executeGohome();
    void executeGoToPosition();
    void executeRotateAngle();
    void calculateRobotArm(float x, float y, float z, float L, float M, float N, float* q1, float* q2, float* q3);
private:
    ApplicationController* m_app;
    int m_numberMove = 0;
    int m_currentMoveID = 0;
    float m_distanceToChessBoard;
    float m_chessBoardSquareLength;
    float m_crawlHeight;
    float m_chessHeight;
    float m_crawlLength;
    float m_armLength[MOTOR::MOTOR_MAX];
    float m_armHomeSpeed[MOTOR::MOTOR_MAX];
    float m_armRatio[MOTOR::MOTOR_MAX];
    long m_armMaxPosition[MOTOR::MOTOR_MAX];
    MoveLocation m_moveSequence[MOTOR_MAX_SEQUENCE];
    MOTOR m_motorID;
    ROBOT_STATE m_state;

    int m_servoAngle;
    long m_servoStartTime;
};

#endif // ROBOT_H
