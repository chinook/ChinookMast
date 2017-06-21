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


//==============================================================================
// Macro definitions
//==============================================================================
#define MAST_DIR_LEFT               -1.0f
#define MAST_DIR_RIGHT               1.0f



/*------------------------------ DC MOTOR ASSEMBLY ----------------------------
/*
/* |OUTPUT | <- |GEAR |  <- |MOTOR |  <- |ACTUAL | -> |MOTOR |  -> | ENCODER |
/* |SHAFT  | <- |BOX  |  <- |SHAFT |  <- |MOTOR  | -> |SHAFT |
/*
/*------------------------------ DC MOTOR ASSEMBLY ----------------------------*/
/* The following #define names are quite misleading, make sure to read the comments
   before each and every terms */

#define __NEW_MOTOR__

#ifndef __NEW_MOTOR__
/* The following specifications are for the IG42E-49K DC motor.
   It's also identified as the RB-Cyt-194 on robotshop.com */

#define PULSES_PER_ROTATION         5.0f
#define MOTOR_ENCODER_RATIO         49.0f
#define MAST_MOTOR_RATIO            50.0f

#define MOTOR_DEG_PER_PULSE         360.0f/(PULSES_PER_ROTATION*MOTOR_ENCODER_RATIO)

#else
/* The following specifications are for the newer 638324 DC motor.
   It's also identified as the RB-Sct-1012 on robotshop.com */

#define PULSES_PER_ROTATION         12.0f

// MOTOR_ENCODER_RATIO is the ratio between the motor's assembly output
// shaft and the motor's shaft alone. In other words, it's the gearbox's ratio...
#define MOTOR_ENCODER_RATIO         71.0f

// MAST_MOTOR_RATIO is the number of turns the motor's assembly output must
// perform before the mast gets a full rotation. It's also the amount of teeth
// present on the cog circling the mast.
#define MAST_MOTOR_RATIO            60.0f

// MOTOR_DEG_PER_PULSE is obtained by dividing 360degrees by the product
// of the amount of encoder pulses per motor shaft rotation and the gearbox's
// ratio. In other words, it's the rotational degrees done by the output shaft
// per encoder pulse.
// Ex: 360/( (encoder pulses per motor shaft turns)*(gearbox ratio)
#define MOTOR_DEG_PER_PULSE         360.0f/245 //(PULSES_PER_ROTATION*MOTOR_ENCODER_RATIO)
#endif

#define N_DATA_TO_ACQ               300   // Used when debugging with SKADI



/*
 * Structure used for the Tustin discrete integrators
 */
typedef struct sCmdValue
{
  float  previousValue
        ,currentValue
        ;
} sCmdValue_t;


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

// Various MATH functions
#define ABS(x)  ( (x >= 0)?  x : -x )
#define SIGN(x) ( (x >= 0)?  1 : -1 )


// Used for debugging with SKADI
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

