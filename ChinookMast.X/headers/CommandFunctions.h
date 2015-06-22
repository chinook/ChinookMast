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


#ifndef __COMMAND_FUNCTIONS_H__
#define	__COMMAND_FUNCTIONS_H__

#include "Setup.h"


/*
 * _____________     ___  error  ____    ___      ________inPi  _____outPi   ___     ________
 * | wind angle |-> /+  \ ----->| K |-> /+  \ -> | 1 / s |---->| ki |-----> /+  \ ->| Motor |
 * |____________|   \_-_/       |___|   \_-_/    |_______| |   |____|       \_+_/   |_______|
 *                    ^                                    |    _____         ^
 *                    |                                    |   | kp |         |
 *                    |                                     -->|____|---------
 *                    |______
 *
 */


//==============================================================================
// Macro definitions
//==============================================================================
#define MAST_DIR_LEFT     -1
#define MAST_DIR_RIGHT     1
#define ERROR_THRESHOLD    0.1

typedef struct sCmdValue
{
  float  previousValue
        ,currentValue
        ;
} sCmdValue_t;

//==============================================================================
// Variable declarations
//==============================================================================


//==============================================================================
// Mast regulation public functions prototypes
//==============================================================================
void TustinZ    (sCmdValue_t *input, sCmdValue_t *output);
void SetPwm     (float cmd);
void Regulator  (void);

#define ABS(x)  (x > 0)?  x : -x
#define SIGN(x) (x > 0)?  1 : -1

#endif	/* __COMMAND_FUNCTIONS_H__ */

