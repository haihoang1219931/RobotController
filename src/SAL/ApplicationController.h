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
    void storeButtonState(int btnID, bool pressed);
    void updateInputState();
    MACHINE_STATE stateMachine();
    void getCurrentPosition(float* listCurrentStep, int* numMotor);
    void getCurrentArmLength(float* listArmLength, int* numArm);
    void getChessBoardParams(float* listParam, int* numParam);
    void executeCommand(char* command);
    void executeSingleMotor(int motorID,
                         int targetStep,
                         int direction,
                         int stepTime);
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
    virtual void initRobot() = 0;
    virtual void specificPlatformGohome(int motorID = MAX_MOTOR) = 0;
    virtual void harwareStop(int motorID = MAX_MOTOR) = 0;
    virtual void checkInput() = 0;
    virtual int printf(const char *fmt, ...) = 0;
    virtual void msleep(int millis) = 0;
    virtual long getSystemTime() = 0;
    virtual void enableEngine(bool enable) = 0;
    virtual bool isLimitReached(int motorID,
                        MOTOR_LIMIT_TYPE limitType) = 0;
    virtual int readSerial(char* output, int length) = 0;
    virtual void initDirection(int motorID, int direction) = 0;
    virtual void moveStep(int motorID, int currentStep, int nextStep) = 0;

public:
    MACHINE_STATE m_machineState;
    Button* m_buttonList[MAX_BUTTON];
    Robot* m_robot;
    ChessBoard* m_chessBoard;
    CommandReader* m_commandReader;
    int m_comCommandID = 0;
    int m_appTimer;
};

#endif // APPLICATIONCONTROLLER_H
