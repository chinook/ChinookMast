//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook V
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateMachine_Mast.h
// Author  : Amaury LAINE
// Date    : 2015-03-13
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : Header with all condition/transition between all states
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : NA
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifndef STATE_MACHINE_MAST_H
#define	STATE_MACHINE_MAST_H

#include "Setup.h"
//#include "StateFunctions.h"


//==============================================================================
// State Machine public function prototypes
//==============================================================================
void StateInit        (void);     // Initialization state of the system
void StateCalib       (void);     // Stop pitch if pitch is on good position
void StateManual      (void);     // Assess manual flags and adjust the mast in consequence
void StateAcq         (void);     // Get data from peripherals
void StateDisconnect  (void);     // Send disconnect msg to backplane
void StateClose       (void);     // Close all peripheral and enter sleep mode
void StateSendData    (void);     // Send various data to other devices
void StateReg         (void);     // Regulate the mast
void StateIdle        (void);     // Wait for power-down
void StateGetMastData (void);     // Get position of mast if in manual mode
void StateScheduler   (void);     // State Scheduler. Decides which state is next



//==============================================================================
// Variable declarations
//==============================================================================
void (*pStateMast)(void);       // State pointer, used to navigate between states
void (*pBuffStateMast)(void);       // State pointer, used to navigate between states
volatile  INT8  breakFlag   // Flag indicating if the emergency break has been pressed
               ,errorFlag   // Flag indicating an error
               ;

//==============================================================================
// Macro definitions
//==============================================================================

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// State scheduler flags
// The state scheduler is at the end of each state and
// decides which state is next. Following flags
// are used in this decision. The names used can and
// should be renamed to improve readability. Also, the
// conditions tested in the defines should be changed
// to proper tests
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*********** LIMITS **/
#define MAST_MAX               90
#define MAST_MIN              -90

/*********** BASIC CONDITION *****************/
#define MAST_LEFT             (oManualMastLeft)
#define MAST_RIGHT            (oManualMastRight)
#define MAST_OK               (!oManualMastLeft && !oManualMastRight)
#define MAST_MAX_OK           (mastAngle.currentValue <= MAST_MAX)
#define MAST_MIN_OK           (mastAngle.currentValue >= MAST_MIN)
#define MAST_DIR_DOWN         SW1
#define MAST_DIR_UP           !MAST_DIR_DOWN
#define MAST_CALIB_MODE       oCalibMode
#define MANUAL_MODE           oManualMode
#define MANUAL_FLAG_CHANGE    oManualFlagChng
#define DISCONNECT_OK         0
#define REG_TIMER_OK          oTimerReg
#define SEND_DATA_TIMER_OK    oTimerSendData

#define PULSE_PER_DEGREE      30

//==============================================================================
// STATE TRANSITIONS
//==============================================================================

/******* TRANSITION CONDITION INIT **********/
#define INIT_2_ACQ            !MAST_CALIB_MODE
#define INIT_2_CALIB           MAST_CALIB_MODE


/******* TRANSITION CONDITION CALIB **********/
#define CALIB_2_ACQ           1


/******* TRANSITION CONDITION ACQ **********/
#define ACQ_2_MANUAL          !MAST_CALIB_MODE &&  MANUAL_MODE && MANUAL_FLAG_CHANGE
#define ACQ_2_DISCONNECT       DISCONNECT_OK
#define ACQ_2_REG             !MAST_CALIB_MODE && !MANUAL_MODE && REG_TIMER_OK
#define ACQ_2_GET_MAST_DATA   !MAST_CALIB_MODE &&  MANUAL_MODE && REG_TIMER_OK
#define ACQ_2_SEND_DATA        SEND_DATA_TIMER_OK


/******* TRANSITION CONDITION GET MAST DATA **********/
#define GET_MAST_DATA_2_ACQ       1


/******* TRANSITION CONDITION SEND DATA **********/
#define SEND_DATA_2_ACQ       1


/******* TRANSITION CONDITION MANUAL **********/
#define MANUAL_2_ACQ          1


/******* TRANSITION CONDITION REG **********/
#define REG_2_ACQ             1


/******* TRANSITION CONDITION DISCONNECT **********/
#define DISCONNECT_2_CLOSE    1


/******* TRANSITION CONDITION CLOSE **********/
#define CLOSE_2_IDLE          1


/******* TRANSITION CONDITION IDLE **********/
#define IDLE_2_INIT           0



/******* TRANSITION CONDITION INIT **********/
//#define INIT_2_CALIB          MAST_CALIB_MODE
//#define INIT_2_MANUAL         MANUAL_MODE && MANUAL_FLAG_CHANGE
//#define INIT_2_STOP           !MAST_CALIB_MODE && MANUAL_MODE && MAST_OK || !MANUAL_MODE
//#define INIT_2_LEFT           !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
//#define INIT_2_RIGHT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT &&!MAST_MAX_OK

/******* TRANSITION CONDITION CALIB ********/
//#define CALIB_2_INIT          0
//#define INIT_2_MANUAL         MANUAL_MODE && MANUAL_FLAG_CHANGE
//#define CALIB_2_STOP          !MAST_CALIB_MODE && MANUAL_MODE && MAST_OK || !MANUAL_MODE
//#define CALIB_2_LEFT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
//#define CALIB_2_RIGHT         !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  &&!MAST_MAX_OK

/******* TRANSITION CONDITION CALIB ********/
//#define MANUAL_2_CALIB        0
//#define MANUAL_2_INIT         0
//#define CALIB_2_STOP          !MAST_CALIB_MODE && MANUAL_MODE && MAST_OK || !MANUAL_MODE
//#define CALIB_2_LEFT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
//#define CALIB_2_RIGHT         !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  &&!MAST_MAX_OK

/******* TRANSITION CONDITION STOP *********/
//#define STOP_2_INIT           0
//#define STOP_2_CALIB          MAST_CALIB_MODE
//#define STOP_2_LEFT           !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
//#define STOP_2_RIGHT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  &&!MAST_MAX_OK

/******* TRANSITION CONDITION RIGHT **********/
//#define RIGHT_2_INIT          0
//#define RIGHT_2_CALIB         MAST_CALIB_MODE
//#define RIGHT_2_STOP          !MAST_CALIB_MODE && MANUAL_MODE && (MAST_OK || MAST_MAX_OK) || !MANUAL_MODE
//#define RIGHT_2_LEFT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK

/******* TRANSITION CONDITION LEFT *******/
//#define LEFT_2_INIT           0
//#define LEFT_2_CALIB          MAST_CALIB_MODE
//#define LEFT_2_STOP           !MAST_CALIB_MODE && MANUAL_MODE && (MAST_OK || MAST_MIN_OK) || !MANUAL_MODE
//#define LEFT_2_RIGHT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  && !MAST_MAX_OK

#endif	/* STATE_MACHINE_MAST_H */

