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
void StateInit(void);           // Initialization state of the system
void StateCalib(void);          // Stop pitch if pitch is on good position
void StateManualStop(void);           // Stop motor if problem or if Mast = consigne
void StateManualLeft(void);           // Down Mast with stepper motor and a gage feedback.
void StateManualRight(void);             // Up Mast with stepper motor and a gage feedback.
void StateAcquisition(void);    // Get data from peripherals
void StateScheduler(void);      // State Scheduler. Decides which state is next



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
#define MAST_MAX_OK           (mastCurrentPos >= MAST_MAX)
#define MAST_MIN_OK           (mastCurrentPos <= MAST_MIN)
#define MAST_DIR_DOWN         SW1
#define MAST_DIR_UP           !MAST_DIR_DOWN
#define MAST_CALIB_MODE       !oCalibDone
#define MANUAL_MODE           oManualMode


/******* TRANSITION CONDITION INIT **********/
#define INIT_2_CALIB         MAST_CALIB_MODE
#define INIT_2_STOP          !MAST_CALIB_MODE && MANUAL_MODE && MAST_OK || !MANUAL_MODE
#define INIT_2_LEFT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
#define INIT_2_RIGHT            !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT &&!MAST_MAX_OK

/******* TRANSITION CONDITION CALIB ********/
#define CALIB_2_INIT         0
#define CALIB_2_STOP         !MAST_CALIB_MODE && MANUAL_MODE && MAST_OK || !MANUAL_MODE
#define CALIB_2_LEFT         !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
#define CALIB_2_RIGHT           !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  &&!MAST_MAX_OK

/******* TRANSITION CONDITION STOP *********/
#define STOP_2_INIT          0
#define STOP_2_CALIB         MAST_CALIB_MODE
#define STOP_2_LEFT          !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK
#define STOP_2_RIGHT            !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  &&!MAST_MAX_OK

/******* TRANSITION CONDITION UP **********/
#define RIGHT_2_INIT            0
#define RIGHT_2_CALIB           MAST_CALIB_MODE
#define RIGHT_2_STOP            !MAST_CALIB_MODE && MANUAL_MODE && (MAST_OK || MAST_MAX_OK) || !MANUAL_MODE
#define RIGHT_2_LEFT            !MAST_CALIB_MODE && MANUAL_MODE && MAST_LEFT &&!MAST_MIN_OK

/******* TRANSITION CONDITION DOWN *******/
#define LEFT_2_INIT          0
#define LEFT_2_CALIB         MAST_CALIB_MODE
#define LEFT_2_STOP          !MAST_CALIB_MODE && MANUAL_MODE && (MAST_OK || MAST_MIN_OK) || !MANUAL_MODE
#define LEFT_2_RIGHT            !MAST_CALIB_MODE && MANUAL_MODE && MAST_RIGHT  && !MAST_MAX_OK

#define PULSE_PER_DEGREE    30

#endif	/* STATE_MACHINE_MAST_H */

