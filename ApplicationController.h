#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

class Button;
class Robot;
class CommandReader;
enum MACHINE_STATE {
    MACHINE_INIT,
    MACHINE_WAIT_COMMAND,
    MACHINE_EXECUTE_COMMAND,
};

enum BUTTON_ID {
    BTN_BASE,
    BTN_ARM1,
    BTN_ARM2,
    BTN_MAX,
};

enum MOTOR{
    MOTOR_BASE,
    MOTOR_ARM1,
    MOTOR_ARM2,
    MOTOR_MAX,
} ;

class ApplicationController
{
public:
    explicit ApplicationController();
    virtual ~ApplicationController();
    
    void loop();
    void checkAllButtonState();
    int buttonState(BUTTON_ID buttonID);
    MACHINE_STATE stateMachine();
    void executeCommand(char* command);
    void setMachineState(MACHINE_STATE machineState);
    virtual int printf(const char *fmt, ...) = 0;
    virtual void msleep(int millis) = 0;
    virtual long getSystemTimeInMillis() = 0;
    virtual int readSerial(char* output, int length) = 0;
    virtual void setMaxSpeed(MOTOR motor, float speed) = 0;
    virtual void setSpeed(MOTOR motor, float speed) = 0;
    virtual void setAcceleration(MOTOR motor, float acceleration) = 0;
    virtual void setTargetPos(MOTOR motor, long target) = 0;
    virtual long distanceToGo(MOTOR motor) = 0;
    virtual void run(MOTOR motor) = 0;
    virtual void runSpeed(MOTOR motor) = 0;
    virtual void setCurrentPosition(MOTOR motor, long position) = 0;
    virtual long currentPosition(MOTOR motor) = 0;
    virtual float speed(MOTOR motor) = 0;

protected:
    MACHINE_STATE m_machineState;
    Button* m_listButton[BUTTON_ID::BTN_MAX];
    Robot* m_robot;
    CommandReader* m_commandReader;
};

#endif // APPLICATIONCONTROLLER_H
