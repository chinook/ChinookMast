//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateFunctions.h
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the header file for the functions of the state machine
//           of the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifndef __COMMAND_FUNCTIONS_H__
#define	__COMMAND_FUNCTIONS_H__

#include "Setup.h"


/*
 * _____________                ________     _____               ________
 * | wind angle |---> (+- ) --->| 1 / s |--->| ki |---> +    --->| Motor |
 * |____________|       ^       |_______|    |____|              |_______|
 *                      |______
 *
 */


//==============================================================================
// Macro definitions
//==============================================================================
#define MAST_DIR_LEFT  -1
#define MAST_DIR_RIGHT  1

typedef struct sCmdValue
{
  double  previousValue
         ,currentValue
         ;
} sCmdValue_t;


//==============================================================================
// Variable declarations
//==============================================================================
const float  KP
            ,KI
            ,K
            ,T
            ;

const UINT8  pwmMaxDutyCycle
            ,pwmMinDutyCycle
            ;

sCmdValue_t windAngle
           ,mastAngle
           ;



//==============================================================================
// State Machine public functions prototypes
//==============================================================================
void TustinZ (sCmdValue_t *input, sCmdValue_t *output);

#endif	/* __COMMAND_FUNCTIONS_H__ */

