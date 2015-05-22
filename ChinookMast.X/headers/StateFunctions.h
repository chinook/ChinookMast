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


#ifndef __STATE_FUNCTIONS_H__
#define	__STATE_FUNCTIONS_H__

#include "Setup.h"


//==============================================================================
// State Machine private functions prototypes
//==============================================================================


//==============================================================================
// Macro definitions
//==============================================================================

/* Setup IO LED */
#define LED_DEBUG0_ON       Port.B.ClearBits(BIT_8)
#define LED_DEBUG1_ON       Port.B.ClearBits(BIT_9)
#define LED_DEBUG2_ON       Port.B.ClearBits(BIT_10)
#define LED_DEBUG3_ON       Port.B.ClearBits(BIT_11)
#define LED_DEBUG4_ON       Port.B.ClearBits(BIT_12)
#define LED_ERROR_ON        Port.B.ClearBits(BIT_13)
#define LED_CAN_ON          Port.B.ClearBits(BIT_15)
#define LED_STATUS_ON       Port.F.ClearBits(BIT_3)

#define LED_DEBUG0_OFF      Port.B.SetBits(BIT_8)
#define LED_DEBUG1_OFF      Port.B.SetBits(BIT_9)
#define LED_DEBUG2_OFF      Port.B.SetBits(BIT_10)
#define LED_DEBUG3_OFF      Port.B.SetBits(BIT_11)
#define LED_DEBUG4_OFF      Port.B.SetBits(BIT_12)
#define LED_ERROR_OFF       Port.B.SetBits(BIT_13)
#define LED_CAN_OFF         Port.B.SetBits(BIT_15)
#define LED_STATUS_OFF      Port.F.SetBits(BIT_3)

#define LED_DEBUG0_TOGGLE   Port.B.ToggleBits(BIT_8)
#define LED_DEBUG1_TOGGLE   Port.B.ToggleBits(BIT_9)
#define LED_DEBUG2_TOGGLE   Port.B.ToggleBits(BIT_10)
#define LED_DEBUG3_TOGGLE   Port.B.ToggleBits(BIT_11)
#define LED_DEBUG4_TOGGLE   Port.B.ToggleBits(BIT_12)
#define LED_ERROR_TOGGLE    Port.B.ToggleBits(BIT_13)
#define LED_CAN_TOGGLE      Port.B.ToggleBits(BIT_15)
#define LED_STATUS_TOGGLE   Port.F.ToggleBits(BIT_3)


/* Setup  IO switch */
#define READ_SW1    Port.E.ReadBits(BIT_5) >> 5
#define READ_SW2    Port.E.ReadBits(BIT_6) >> 6
#define READ_SW3    Port.E.ReadBits(BIT_7) >> 7

//==============================================================================
// Variable declarations
//==============================================================================


#endif	/* __STATE_FUNCTIONS_H__ */

