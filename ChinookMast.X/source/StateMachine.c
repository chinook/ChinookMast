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

#include "..\headers\StateMachine.h"
#include "..\headers\DRV8711_Para.h"

INT8 Calib_done = 1;

// Mast general value
volatile INT32 Mast_now = 10;                  // Actual position of Mast
volatile INT32 Mast_consigne = 10000;            // New value of Mast
extern volatile BOOL oCapture1, oCapture3;

INT8  memoMast_Up = 0;
INT8  memoMast_Down = 0;

//==============================================================================
//	STATES OF STATE MACHINE
//==============================================================================

//===============================================================
// Name     : StateScheduler_Mast
// Purpose  : Decides which state is next depending on current
//            state and flags. Used as a function
//===============================================================
void StateScheduler(void)
{
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if (pStateMast == &StateInit)
  {
    if (INIT_2_CALIB)
    {
      pStateMast = &StateCalib;   // calib state
    }
    else if (INIT_2_STOP)
    {
      pStateMast = &StateStop;    // stop state
    }
    else if (INIT_2_DOWN)
    {
      pStateMast = &StateDown;    // Down state
    }
    else if (INIT_2_UP)
    {
      pStateMast = &StateUp;      // Up state
    }
    else
    {
      pStateMast = &StateInit;    // Go to Error state by default
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateCalib
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateCalib)
  {
    if (CALIB_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (CALIB_2_STOP)
    {
      pStateMast = &StateStop;     // Stop state
    }
    else if (CALIB_2_DOWN)
    {
      pStateMast = &StateDown;     // Down state
    }
    else if (CALIB_2_UP)
    {
      pStateMast = &StateUp;     // Up state
    }
    else
    {
      pStateMast = &StateCalib;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateStop
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateStop)
  {
    if (STOP_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (STOP_2_CALIB)
    {
      pStateMast = &StateCalib;     // Calib state
    }
    else if (STOP_2_DOWN)
    {
      pStateMast = &StateDown;     // Down state
    }
    else if (STOP_2_UP)
    {
      pStateMast = &StateUp;     // Up state
    }
    else
    {
      pStateMast = &StateStop;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDown
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateDown)
  {
    if (DOWN_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (DOWN_2_CALIB)
    {
      pStateMast = &StateCalib;     // Calib state
    }
    else if (DOWN_2_STOP)
    {
      pStateMast = &StateStop;     // Stop state
    }
    else if (DOWN_2_UP)
    {
      pStateMast = &StateUp;     // UP state
    }
    else
    {
      pStateMast = &StateDown;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateUp
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateUp)
  {
    if (UP_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (UP_2_CALIB)
    {
      pStateMast = &StateCalib;     // Calib state
    }
    else if (UP_2_STOP)
    {
      pStateMast = &StateStop;     // Stop state
    }
    else if (UP_2_DOWN)
    {
      pStateMast = &StateDown;     // Down state
    }
    else
    {
      pStateMast = &StateUp;   // Go to Error state by default
    }
  }

//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//   Current state = undetermined
//  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    pStateMast = &StateInit;   // Go to Error state by default
  }
  /*
   * DEVELOPPER CODE HERE
   */

}

//===============================================================
// Name     : StateInit
// Purpose  : Initialization of the system.
//===============================================================
void StateInit(void)
{
  INTDisableInterrupts();   // Disable all interrupts of the system.

  INIT_PORTS;
//  INIT_WDT;
  INIT_TIMER;
  INIT_INPUT_CAPTURE;
  INIT_PWM;
  INIT_UART;
//  INIT_CAN;
//  INIT_SKADI;
  INIT_SPI;
//  INIT_I2C;
  START_INTERRUPTS;
    
  Init_reg_Mast();
  DRVB_SC = 0;
  Timer.DelayMs(1);
  DRVA_RESET = 0;
  Timer.DelayMs(1);
  DRVA_SLEEP = 1;
  Timer.DelayMs(1);
//  WriteDrive(DRVA, CONTROL_Mastw);
  WriteDrive(DRVA, 0x0001);
//  WriteDrive(DRVA, TORQUE_Mastw);
//  WriteDrive(DRVA, 0x10FF);
//  WriteDrive(DRVA, 0x2180);
//  WriteDrive(DRVA, BLANK_Mastw);
//  WriteDrive(DRVA, DECAY_Mastw);
//  WriteDrive(DRVA, STATUS_Mastw);
//  WriteDrive(DRVA, 0x7000);
}


//===============================================================
// Name     : StateStop
// Purpose  : Stop stepper Mast when good Mast or problem
//===============================================================
void StateStop(void)
{
//  DRVA_STEP = 1;
//  DRVA_SLEEP = 0;
  
  Pwm.SetDutyCycle(PWM_4, 500);
  Pwm.SetDutyCycle(PWM_5, 500);
  WriteDrive(DRVA, STATUS_Mastw);
}


//===============================================================
// Name     : StateCalib
// Purpose  : calib mode will search max and min Mast of transmission
//===============================================================
void StateCalib(void)
{
  Calib_done = 0;
}


//===============================================================
// Name     : StateDown
// Purpose  : Error state of the system. Used to assess and
//            correct errors in the system.
//===============================================================
void StateDown(void)
{
//  DRVA_DIR = MAST_DIR_DOWN;
  DRVA_SLEEP = 1;

//  DRVA_BIN1 ^= 1;
//  DRVA_BIN2 ^= 1;
//  DRVA_BIN1 = 0;
//  DRVA_BIN2 = 0;
  Pwm.SetDutyCycle(PWM_4, 750);
  Pwm.SetDutyCycle(PWM_5, 250);
  Mast_now--;
  WriteDrive(DRVA, STATUS_Mastw);
}


//===============================================================
// Name     : StateUp
// Purpose  : Error state of the system. Used to assess and
//            correct errors in the system.
//===============================================================
void StateUp(void)
{
//  DRVA_DIR = MAST_DIR_UP;
  DRVA_SLEEP = 1;

//  DRVA_BIN1 ^= 1;
//  DRVA_BIN2 ^= 1;
//  DRVA_BIN1 = 0;
//  DRVA_BIN2 = 0;
  Pwm.SetDutyCycle(PWM_4, 250);
  Pwm.SetDutyCycle(PWM_5, 750);
  Mast_now++;
  WriteDrive(DRVA, STATUS_Mastw);
}


//===============================================================
// Name     : StateAcquisition
// Purpose  : Get data from peripherals
//===============================================================
void StateAcquisition(void)
{
  if (!SW2 && !memoMast_Down) 
  {
    Mast_consigne += 1000;
    memoMast_Down = 1;
  }
  else if (SW2) 
  {
    memoMast_Down = 0;
  }
  
  if (!SW3 && !memoMast_Up) 
  {
    Mast_consigne -= 1000;
    memoMast_Up = 1;
  }
  else if (SW3) 
  {
    memoMast_Up = 0;
  }
  
  INT64 rx1, rx3;
  sUartLineBuffer_t buffer = 
  { 
    .buffer = {0} 
   ,.length = 0 
  };

  if (oCapture1)
  {
    oCapture1 = 0;
    rx1 = InputCapture.GetTimeBetweenCaptures(IC1, SCALE_US);
    if (rx1 < 0)
    {
      buffer.length = sprintf(buffer.buffer, "error\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
    else
    {
      buffer.length = sprintf(buffer.buffer, "IC1 = %d\r\n", rx1);
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
  }
  
  if (oCapture3)
  {
    oCapture3 = 0;
    rx3 = InputCapture.GetTimeBetweenCaptures(IC3, SCALE_US);
    if (rx3 < 0)
    {
      buffer.length = sprintf(buffer.buffer, "error\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
    else
    {
      buffer.length = sprintf(buffer.buffer, "IC3 = %d\r\n", rx3);
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
  }
}
