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
void StateInitMast(void);           // Initialization state of the system
void StateCalibMast(void);    // Stop pitch if pitch is on good position
void StateStopMast(void);           // Stop motor if problem or if Mast = consigne
void StateDownMast(void);           // Down Mast with stepper motor and a gage feedback.
void StateUpMast(void);             // Up Mast with stepper motor and a gage feedback.
void StateSchedulerMast(void); // State Scheduler. Decides which state is next

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

volatile extern INT32 Mast_now;                    // From main.c
volatile extern INT32 Mast_consigne;               // From main.c

/*********** PITCH STATE MACHINE PARAMETERS **/
#define Mast_ADC_LVL_DETECT   500           // Min value to detect changement Mast in bits (max 1023 for this PIC 10bits)
#define Mast_ADC_VAL_EXTREM   100           // Extrem value of gage in bits (max 1023 for this PIC 10bits)
#define Mast_ADC_HYST         100            // hysteresis to don't have an infinity of shift
#define GAGE_ADC              Gage_Value    // Value of gage, dependant of which ADC port it's connected
#define Mast_ADC_CALIB        300           // Maximum to detect max Mast
#define Mast_MAX              20000
#define Mast_MIN              0

/*********** BASIC CONDITION *****************/
#define MAST_DOWN             (Mast_now > Mast_consigne)
#define MAST_UP               (Mast_now < Mast_consigne)
#define MAST_OK               (Mast_now == Mast_consigne)
#define MAST_MAX_OK           (Mast_now == Mast_MAX)
#define MAST_MIN_OK           (Mast_now == Mast_MIN)
#define Mast_DIR_DOWN         SW1
#define Mast_DIR_UP           !Mast_DIR_DOWN
#define EXTREMUM_MAX          (GAGE_ADC < Mast_ADC_VAL_EXTREM) && (DRVA_DIR == Mast_DIR_UP)
#define EXTREMUM_MIN          (GAGE_ADC < Mast_ADC_VAL_EXTREM) && (DRVA_DIR == Mast_DIR_DOWN)
#define MAST_CALIB_MODE       !Calib_done
#define MODE_MANUEL           1


/******* TRANSITION CONDITION INIT **********/
#define Mast_INIT_2_CALIB         MAST_CALIB_MODE
#define Mast_INIT_2_STOP          !MAST_CALIB_MODE && MODE_MANUEL && MAST_OK
#define Mast_INIT_2_DOWN          !MAST_CALIB_MODE && MODE_MANUEL && MAST_DOWN &&!MAST_MIN_OK
#define Mast_INIT_2_UP            !MAST_CALIB_MODE && MODE_MANUEL && MAST_UP &&!MAST_MAX_OK

/******* TRANSITION CONDITION CALIB ********/
#define Mast_CALIB_2_INIT         0
#define Mast_CALIB_2_STOP         !MAST_CALIB_MODE && MODE_MANUEL && MAST_OK
#define Mast_CALIB_2_DOWN         !MAST_CALIB_MODE && MODE_MANUEL && MAST_DOWN &&!MAST_MIN_OK
#define Mast_CALIB_2_UP           !MAST_CALIB_MODE && MODE_MANUEL && MAST_UP  &&!MAST_MAX_OK

/******* TRANSITION CONDITION STOP *********/
#define Mast_STOP_2_INIT          0
#define Mast_STOP_2_CALIB         MAST_CALIB_MODE
#define Mast_STOP_2_DOWN          !MAST_CALIB_MODE && MODE_MANUEL && MAST_DOWN &&!MAST_MIN_OK
#define Mast_STOP_2_UP            !MAST_CALIB_MODE && MODE_MANUEL && MAST_UP  &&!MAST_MAX_OK

/******* TRANSITION CONDITION UP **********/
#define Mast_UP_2_INIT            0
#define Mast_UP_2_CALIB           MAST_CALIB_MODE
#define Mast_UP_2_STOP            !MAST_CALIB_MODE && MODE_MANUEL && (MAST_OK || MAST_MAX_OK)
#define Mast_UP_2_DOWN            !MAST_CALIB_MODE && MODE_MANUEL && MAST_DOWN &&!MAST_MIN_OK

/******* TRANSITION CONDITION DOWN *******/
#define Mast_DOWN_2_INIT          0
#define Mast_DOWN_2_CALIB         MAST_CALIB_MODE
#define Mast_DOWN_2_STOP          !MAST_CALIB_MODE && MODE_MANUEL && (MAST_OK || MAST_MIN_OK)
#define Mast_DOWN_2_UP            !MAST_CALIB_MODE && MODE_MANUEL && MAST_UP  && !MAST_MAX_OK

//==============================================================================
// Variable declarations
//==============================================================================
void (*pStateMast)(void);       // State pointer, used to navigate between states
void (*pBuffStateMast)(void);       // State pointer, used to navigate between states
volatile  INT8  breakFlag   // Flag indicating if the emergency break has been pressed
               ,errorFlag   // Flag indicating an error
               ;

#endif	/* STATE_MACHINE_MAST_H */

