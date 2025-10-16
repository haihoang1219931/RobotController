#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include "StdTypes.h"

class Button;
class Robot;
class ChessBoard;
class CommandReader;

class ApplicationController
{
public:
    explicit ApplicationController();
    virtual ~ApplicationController();
    
    void loop();
    void checkAllButtonState();
//    int buttonState(BUTTON_ID buttonID);
    MACHINE_STATE stateMachine();
    void getCurrentPosition(float* listCurrentStep, int* numMotor, float* captureStep);
    void getCurrentArmLength(float* listCurrentArmLength, int* numArm);
    void getChessBoardParams(float* listParam, int* numParam);
    void executeCommand(char* command);
    void setMachineState(MACHINE_STATE machineState);
    bool inverseKinematic(float x, float y,
                           float a1, float a2,float* p1, float* p2);
    void forwardKinematic(float a1, float a2, float p1, float p2, float* x, float* y);
    void executeSequence(MOVE_TYPE moveType,
                         int startCol, int startRow,
                         int stopCol, int stopRow,
                         char promotePiece = 0);
    void calculateSequenceMoveNormal(int startCol, int startRow,
                         int stopCol, int stopRow);
    void calculateSequenceAttack(int startCol, int startRow,
                         int stopCol, int stopRow);
    void calculateSequencePastPawn(int startCol, int startRow,
                         int stopCol, int stopRow);
    void calculateSequencePromotePiece(int startCol, int startRow,
                         int stopCol, int stopRow, char promotePiece);
    void calculateSequenceCastle(int kingCol, int kingRow,
                                 int rookCol, int rookRow);
    void calculateJoints(float xPos, float yPos, float upAngleInDegree, int* jointSteps);
    void clearSequenceMove();
    void appendSequenceMove(Point start, Point stop, bool straightMove = false);
    void appendStandByMove();
    void initSequenceMove(int numberOfJoints);
    virtual int printf(const char *fmt, ...) = 0;
    virtual void msleep(int millis) = 0;
    virtual long getSystemTime() = 0;
    virtual bool isLimitReached(int motorID,
                        MOTOR_LIMIT_TYPE limitType) = 0;
    virtual int readSerial(char* output, int length) = 0;
//    virtual void setMaxSpeed(MOTOR motor, float speed) = 0;
//    virtual void setSpeed(MOTOR motor, float speed) = 0;
//    virtual void setAcceleration(MOTOR motor, float acceleration) = 0;
//    virtual void setTargetPos(MOTOR motor, long target) = 0;
//    virtual bool isMoveDone(MOTOR motor) = 0;
//    virtual void run(MOTOR motor) = 0;
//    virtual void runSpeed(MOTOR motor) = 0;
//    virtual void setCurrentPosition(MOTOR motor, long position) = 0;
//    virtual long currentPosition(MOTOR motor) = 0;
//    virtual bool isMotorHomed(MOTOR motor) = 0;
//    virtual void setHomePosition(MOTOR motor) = 0;
//    virtual void goHome(MOTOR motor) = 0;
//    virtual float speed(MOTOR motor) = 0;
//    virtual float maxSpeed(MOTOR motor) = 0;
    virtual void enableEngine(bool enable) = 0;    
//    virtual void stop(MOTOR motor) = 0;

public:
    MACHINE_STATE m_machineState;
    Button* m_buttonList[MAX_BUTTON];
    Robot* m_robot;
    ChessBoard* m_chessBoard;
    CommandReader* m_commandReader;
    int m_comCommandID = 0;
    int m_appTimer;
    float m_motorScale[MAX_MOTOR];
    float m_armLength[MAX_MOTOR];
    float m_scale;
};

#endif // APPLICATIONCONTROLLER_H
