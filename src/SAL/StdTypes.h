#ifndef STDTYPES_H
#define STDTYPES_H

#include <stdio.h>

#define MAX_MOVE_SEQUENCE 20
#define MAX_BUTTON 12
#define MAX_MOTOR 12
#define MAX_COMMAND_LENGTH 256
#define MAX_PARAMS_CHESSBOARD 4

#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef enum {
    MOTOR_INIT,
    MOTOR_EXECUTE_INCREASE_SPEED,
    MOTOR_EXECUTE_CRUISE_SPEED,
    MOTOR_EXECUTE_DECREASE_SPEED,
    MOTOR_EXECUTE_HOME,
    MOTOR_DONE
}MOTOR_CONTROL_STATE;

typedef enum {
    MOTOR_LIMIT_MIN,
    MOTOR_LIMIT_MAX,
}MOTOR_LIMIT_TYPE;

typedef enum {
    MOTOR_ENABLED,
    MOTOR_DISABLED
}MOTOR_ACTIVE_STATE;

typedef enum{
    ROBOT_INIT,
    ROBOT_EXECUTE_GO_HOME,
    ROBOT_EXECUTE_POSITION,
    ROBOT_EXECUTE_SEQUENCE,
    ROBOT_EXECUTE_DONE,
}ROBOT_STATE;

typedef enum{
    ROBOT_MOVE_INIT,
    ROBOT_MOVE_EXECUTE,
    ROBOT_MOVE_CAPTURE,
    ROBOT_MOVE_DONE,
}ROBOT_SEQUENCE_STATE;

typedef enum {
    MACHINE_INIT,
    MACHINE_WAIT_COMMAND,
    MACHINE_EXECUTE_COMMAND,
    MACHINE_EXECUTE_COMMAND_DONE,
}MACHINE_STATE;

typedef enum {
    MOTION_JOINT_SPACE,
    MOTION_CARTESION_SPACE,
}MOTION_SPACES;

typedef enum {
    MOVE_NORMAL,
    MOVE_ATTACK,
    MOVE_PASTPAWN,
    MOVE_CASTLE,
    MOVE_PROMOTE,
}MOVE_TYPE;

typedef enum {
    ZONE_MACHINE,
    ZONE_GUEST
}ZONE_TYPE;

typedef struct{
    float x;
    float y;
}Point;

typedef struct{
    bool active;
    float scale;
    float length;
    int initStep;
    int homeStep;
    int homeStepTime;
}JointParam;
#endif // STDTYPES_H
