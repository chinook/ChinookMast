//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Project : ChinookMast
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : CommandFunctions.h
// Author  : Frederic Chasse
// Date    : 2015-06-21
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the header file for the functions of the Mast regulation
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifndef __COMMAND_FUNCTIONS__
#define	__COMMAND_FUNCTIONS__

#include "Setup.h"
#include "StateMachine.h"


/*
 *  ____________     ___  error  ___     ___      _______ inPi  ____ outPi   ___     ________   __________      _______
 * | wind angle |-> /+  \ ----->| K |-> /+  \ -> | 1 / s |---->| ki |-----> /+  \ ->| Motor |->| Encodeur |--->| 1 / s |__
 * |____________|   \_-_/       |___|   \_-_/    |_______| |   |____|       \_+_/   |_______|  |__________| |  |_______|  |
 *                    ^                   ^                |    _____         ^                             |             |
 *                    |                   |                |   | kp |         |                             |             |
 *                    |                   |                 -->|____|---------            mastCurrentSpeed  |             |
 *                    |                   |_________________________________________________________________|             |
 *                    |___________________________________________________________________________________________________|
 *                                                                                        mastCurrentPos
 */


//==============================================================================
// Macro definitions
//==============================================================================
#define MAST_DIR_LEFT               -1
#define MAST_DIR_RIGHT               1

//#define ERROR_THRESHOLD              5.0f
#define ERROR_THRESHOLD              0.1f

#define KP                           0.015f
//#define KI                           0.075f
#define KI                           0.100f
//#define K                            1.500f
#define K                            0.400f
#define T                            0.100f
#define PWM_MAX_DUTY_CYCLE           0.980f
#define PWM_MIN_DUTY_CYCLE           0.030f


//const float  KP = 0.015f
////            ,KI = 0.075f
//            ,KI = 0.100f
//            ,K  = 0.5f
//            ,T  = 0.05f
//            ,pwmMaxDutyCycle  = 0.98f
//            ,pwmMinDutyCycle  = 0.2f
//            ;

#define MOTOR_ENCODER_RATIO         49.0f
#define MAST_MOTOR_RATIO            50.0f

#define INP_CAP_EVENTS_FOR_AVERAGE  20

//#define MOTOR_DEG_PER_PULSE         360.0f/980.0f
#define MOTOR_DEG_PER_PULSE         360.0f/245.0f
//#define MOTOR_DEG_PER_PULSE         360.0f/250.0f
//#define MOTOR_DEG_PER_PULSE         360.0f/252.0f

#define N_DATA_TO_ACQ               200

#define PRINT_DATA                  0


typedef struct sCmdValue
{
  float  previousValue
        ,currentValue
        ;
} sCmdValue_t;


//typedef struct sInpCapValues
//{
//  UINT32 inpCapTime2  [INP_CAP_EVENTS_FOR_AVERAGE + 10];
//  UINT32 inpCapTime4  [INP_CAP_EVENTS_FOR_AVERAGE + 10];
//  INT8   dir          [INP_CAP_EVENTS_FOR_AVERAGE + 10];
//  UINT16 n;
//} sInpCapValues_t;

//==============================================================================
// Variable declarations
//==============================================================================


//==============================================================================
// Mast regulation public functions prototypes
//==============================================================================
void TustinZ    (sCmdValue_t *input, sCmdValue_t *output);
void SetPwm     (float cmd);
void Regulator  (void);

// Input capture functions
// =======================================
void AssessMastValues (void);
// =======================================

#define ABS(x)  ( (x >= 0)?  x : -x )
#define SIGN(x) ( (x >= 0)?  1 : -1 )

typedef struct
{
  float windPrevious  [N_DATA_TO_ACQ];
  float windCurrent   [N_DATA_TO_ACQ];
  float posPrevious   [N_DATA_TO_ACQ];
  float posCurrent    [N_DATA_TO_ACQ];
  float speedPrevious [N_DATA_TO_ACQ];
  float speedCurrent  [N_DATA_TO_ACQ];
  float error         [N_DATA_TO_ACQ];
  float inPiPrevious  [N_DATA_TO_ACQ];
  float inPiCurrent   [N_DATA_TO_ACQ];
  float outPiPrevious [N_DATA_TO_ACQ];
  float outPiCurrent  [N_DATA_TO_ACQ];
  float cmd           [N_DATA_TO_ACQ];
  UINT16 length;
} sCmdData_t;

#endif	/* __COMMAND_FUNCTIONS__ */

