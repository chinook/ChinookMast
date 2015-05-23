//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook V
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateMachine_Mast.c
// Author  : Amaury LAINE
// Date    : 2015-03-13
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : Used to controle stepper for Chinook V
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : NA
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\StateMachine_Mast.h"
#include "..\headers\DRV8711_Para.h"

INT8 Calib_done = 1;

//==============================================================================
//	STATES OF STATE MACHINE
//==============================================================================

//===============================================================
// Name     : StateScheduler_Mast
// Purpose  : Decides which state is next depending on current
//            state and flags. Used as a function
//===============================================================
void StateSchedulerMast(void)
{
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if (pStateMast == &StateInitMast)
  {
    if (Mast_INIT_2_CALIB)
    {
      pStateMast = &StateCalibMast;       // calib state
    }
    else if (Mast_INIT_2_STOP)
    {
      pStateMast = &StateStopMast;     // stop state
    }
    else if (Mast_INIT_2_DOWN)
    {
      pStateMast = &StateDownMast;     // Down state
    }
    else if (Mast_INIT_2_UP)
    {
      pStateMast = &StateUpMast;     // Up state
    }
    else
    {
      pStateMast = &StateInitMast;   // Go to Error state by default
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateCalib
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateCalibMast)
  {
    if (Mast_CALIB_2_INIT)
    {
      pStateMast = &StateInitMast;       // Init state
    }
    else if (Mast_CALIB_2_STOP)
    {
      pStateMast = &StateStopMast;     // Stop state
    }
    else if (Mast_CALIB_2_DOWN)
    {
      pStateMast = &StateDownMast;     // Down state
    }
    else if (Mast_CALIB_2_UP)
    {
      pStateMast = &StateUpMast;     // Up state
    }
    else
    {
      pStateMast = &StateCalibMast;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateStop
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateStopMast)
  {
    if (Mast_STOP_2_INIT)
    {
      pStateMast = &StateInitMast;       // Init state
    }
    else if (Mast_STOP_2_CALIB)
    {
      pStateMast = &StateCalibMast;     // Calib state
    }
    else if (Mast_STOP_2_DOWN)
    {
      pStateMast = &StateDownMast;     // Down state
    }
    else if (Mast_STOP_2_UP)
    {
      pStateMast = &StateUpMast;     // Up state
    }
    else
    {
      pStateMast = &StateStopMast;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDown
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateDownMast)
  {
    if (Mast_DOWN_2_INIT)
    {
      pStateMast = &StateInitMast;       // Init state
    }
    else if (Mast_DOWN_2_CALIB)
    {
      pStateMast = &StateCalibMast;     // Calib state
    }
    else if (Mast_DOWN_2_STOP)
    {
      pStateMast = &StateStopMast;     // Stop state
    }
    else if (Mast_DOWN_2_UP)
    {
      pStateMast = &StateUpMast;     // UP state
    }
    else
    {
      pStateMast = &StateDownMast;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateUp
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateUpMast)
  {
    if (Mast_UP_2_INIT)
    {
      pStateMast = &StateInitMast;       // Init state
    }
    else if (Mast_UP_2_CALIB)
    {
      pStateMast = &StateCalibMast;     // Calib state
    }
    else if (Mast_UP_2_STOP)
    {
      pStateMast = &StateStopMast;     // Stop state
    }
    else if (Mast_UP_2_DOWN)
    {
      pStateMast = &StateDownMast;     // Down state
    }
    else
    {
      pStateMast = &StateUpMast;   // Go to Error state by default
    }
  }

//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//   Current state = undetermined
//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    pStateMast = &StateInitMast;   // Go to Error state by default
  }
  /*
   * DEVELOPPER CODE HERE
   */

}

//===============================================================
// Name     : StateInitMast
// Purpose  : Initialization of the system.
//===============================================================
void StateInitMast(void)
{

  INTDisableInterrupts();   // Disable all interrupts of the system.

  INIT_PORTS;
//  INIT_WDT;
//  INIT_TIMER;
//  INIT_INPUT_CAPTURE;
//  INIT_PWM;
//  INIT_ADC;
  INIT_UART;
//  INIT_CAN;
//  INIT_SKADI;
//  INIT_SPI;
//  INIT_I2C;
  START_INTERRUPTS;

//  DRVA_RESET = 0;
//  DRVA_SLEEP = 1;
//  DRV_mgs(DRVA, CONTROL_Mastw);
//  DRV_mgs(DRVA, TORQUE_Mastw);
//  DRV_mgs(DRVA, BLANK_Mastw);
//  DRV_mgs(DRVA, DECAY_Mastw);
//  DRV_mgs(DRVA, STATUS_Mastw);


}

//===============================================================
// Name     : StateStopMast
// Purpose  : Stop stepper Mast when good Mast or problem
//===============================================================
void StateStopMast(void)
{

  DRVA_STEP = 1;
  DRVA_SLEEP = 0;

}

//===============================================================
// Name     : StateCalibMast
// Purpose  : calib mode will search max and min Mast of transmission
//===============================================================
void StateCalibMast(void)
{
  Calib_done = 0;

}

//===============================================================
// Name     : StateDownMast
// Purpose  : Error state of the system. Used to assess and
//            correct errors in the system.
//===============================================================
void StateDownMast(void)
{
  DRVA_DIR = Mast_DIR_DOWN;
  DRVA_SLEEP = 1;

  DRVA_BIN1 ^= 1;
  DRVA_BIN2 ^= 1;
  Mast_now--;
}

//===============================================================
// Name     : StateUpMast
// Purpose  : Error state of the system. Used to assess and
//            correct errors in the system.
//===============================================================
void StateUpMast(void)
{
  DRVA_DIR = Mast_DIR_UP;
  DRVA_SLEEP = 1;

  DRVA_BIN1 ^= 1;
  DRVA_BIN2 ^= 1;
  Mast_now++;
}
