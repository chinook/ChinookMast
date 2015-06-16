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
volatile INT32 mastCurrentPos = 10;                  // Actual position of Mast
volatile INT32 Mast_consigne = 1000;            // New value of Mast
extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ;

volatile BOOL  oFirstTimeInStateUp    = 1
              ,oFirstTimeInStateDown  = 1
              ,oFirstTimeInStateStop  = 1
              ,oButtonLeft            = 0
              ,oButtonRight           = 0
              ;

BOOL  oCapture1Acquired = 0
     ,oCapture2Acquired = 0
     ,oCapture3Acquired = 0
     ,oCapture4Acquired = 0
     ;

INT8  memoMast_Right = 0;
INT8  memoMast_Left = 0;

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
      pStateMast = &StateManualLeft;    // Down state
    }
    else if (INIT_2_UP)
    {
      pStateMast = &StateManualRight;      // Up state
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
      pStateMast = &StateManualLeft;     // Down state
    }
    else if (CALIB_2_UP)
    {
      pStateMast = &StateManualRight;     // Up state
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
      pStateMast = &StateManualLeft;     // Down state
    }
    else if (STOP_2_UP)
    {
      pStateMast = &StateManualRight;     // Up state
    }
    else
    {
      pStateMast = &StateStop;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateManualLeft
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateManualLeft)
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
      pStateMast = &StateManualRight;     // UP state
    }
    else
    {
      pStateMast = &StateManualLeft;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateManualRight
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateManualRight)
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
      pStateMast = &StateManualLeft;     // Down state
    }
    else
    {
      pStateMast = &StateManualRight;   // Go to Error state by default
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
  INIT_CAN;
//  INIT_SKADI;
  INIT_SPI;
//  INIT_I2C;
  START_INTERRUPTS;

  // Send ID to backplane by CAN protocol
  SEND_ID_TO_BACKPLANE;

  // Init registers for the drive
  Init_reg_Mast();
//  DRVB_SC = 0;
  DRVA_SC = 0;
  Timer.DelayMs(1);
//  DRVA_RESET = 0;
  DRVB_RESET = 0;
  Timer.DelayMs(1);
//  DRVA_SLEEP = 1;
  DRVB_SLEEP = 1;
  Timer.DelayMs(1);
  
//  WriteDrive(DRVA, CONTROL_Mastw);
//  WriteDrive(DRVA, TORQUE_Mastw);
  WriteDrive(DRVB, CONTROL_Mastw);
  WriteDrive(DRVB, TORQUE_Mastw);
//  WriteDrive(DRVA, OFF_Mastw);
//  WriteDrive(DRVA, 0x2180);
  WriteDrive(DRVB, 0x2180);
//  WriteDrive(DRVA, BLANK_Mastw);
//  WriteDrive(DRVA, DECAY_Mastw);
//  WriteDrive(DRVA, STALL_Mastw);
//  WriteDrive(DRVA, DRIVE_Mastw);
//  WriteDrive(DRVA, STATUS_Mastw);
  WriteDrive(DRVB, STATUS_Mastw);
}


//===============================================================
// Name     : StateStop
// Purpose  : Stop stepper Mast when good Mast or problem
//===============================================================
void StateStop(void)
{
//  DRVA_SLEEP = 0;
  DRVB_SLEEP = 0;
  
  if (oFirstTimeInStateStop)
  {
    oFirstTimeInStateUp = 1;
    oFirstTimeInStateDown = 1;
    oFirstTimeInStateStop = 0;

    Pwm.SetDutyCycle(PWM_2, 500);
    Pwm.SetDutyCycle(PWM_3, 500);
  }
//  WriteDrive(DRVA, STATUS_Mastw);
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
// Name     : StateManualLeft
// Purpose  : Error state of the system. Used to assess and
//            correct errors in the system.
//===============================================================
void StateManualLeft(void)
{
//  DRVA_SLEEP = 1;
  DRVB_SLEEP = 1;
  
  if (oFirstTimeInStateDown)
  {
    oFirstTimeInStateUp = 1;
    oFirstTimeInStateStop = 1;
    oFirstTimeInStateDown = 0;

//    Pwm.SetDutyCycle(PWM_2, 800);
//    Pwm.SetDutyCycle(PWM_3, 200);
    Pwm.SetDutyCycle(PWM_2, 250);
    Pwm.SetDutyCycle(PWM_3, 750);
  }
  mastCurrentPos--;
//  WriteDrive(DRVA, STATUS_Mastw);
  WriteDrive(DRVB, STATUS_Mastw);
}


//===============================================================
// Name     : StateManualRight
// Purpose  : Error state of the system. Used to assess and
//            correct errors in the system.
//===============================================================
void StateManualRight(void)
{
//  DRVA_SLEEP = 1;
  DRVB_SLEEP = 1;

  if (oFirstTimeInStateUp)
  {
    oFirstTimeInStateStop = 1;
    oFirstTimeInStateDown = 1;
    oFirstTimeInStateUp = 0;
//    Pwm.SetDutyCycle(PWM_2, 200);
//    Pwm.SetDutyCycle(PWM_3, 800);
    Pwm.SetDutyCycle(PWM_2, 750);
    Pwm.SetDutyCycle(PWM_3, 250);
  }
  mastCurrentPos++;
//  WriteDrive(DRVA, STATUS_Mastw);
  WriteDrive(DRVB, STATUS_Mastw);
}


//===============================================================
// Name     : StateAcquisition
// Purpose  : Get data from peripherals
//===============================================================
void StateAcquisition(void)
{
  if ( (!SW2 && !memoMast_Left) || (oButtonLeft && !memoMast_Left) )
  {
    Mast_consigne -= 1000;
    memoMast_Left = 1;
    oButtonLeft = 0;
  }
  else if (SW2) 
  {
    memoMast_Left = 0;
  }
  
  if ( (!SW3 && !memoMast_Right) || (oButtonRight && !memoMast_Right) )
  {
    Mast_consigne += 1000;
    memoMast_Right = 1;
    oButtonRight = 0;
  }
  else if (SW3) 
  {
    memoMast_Right = 0;
  }
  
  INT64 rx1, rx2, rx3, rx4;

  sUartLineBuffer_t buffer = 
  { 
    .buffer = {0} 
   ,.length =  0
  };

//  if (oCapture1)
//  {
//    oCapture1 = 0;
//    rx1 = InputCapture.GetTimeBetweenCaptures(IC1, SCALE_US);
//    if (rx1 < 0)
//    {
//      buffer.length = sprintf(buffer.buffer, "error\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//    else
//    {
//      buffer.length = sprintf(buffer.buffer, "IC1 = %d\r\n", rx1);
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//  }
//
//  if (oCapture3)
//  {
//    oCapture3 = 0;
//    rx3 = InputCapture.GetTimeBetweenCaptures(IC3, SCALE_US);
//    if (rx3 < 0)
//    {
//      buffer.length = sprintf(buffer.buffer, "error\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//    else
//    {
//      buffer.length = sprintf(buffer.buffer, "IC3 = %d\r\n", rx3);
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//  }

  if (oCapture2 && oCapture4)
  {
    oCapture2 = 0;
    oCapture4 = 0;
    rx2 = InputCapture.GetTimeBetweenCaptures(IC2, SCALE_US);
    if (rx2 < 0)
    {
      buffer.length = sprintf(buffer.buffer, "error\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
    else
    {
//      buffer.length = sprintf(buffer.buffer, "IC2 speed = %d,    capture = %d\r\n", rx2, InputCapture.Var.currentCaptureCountValue[IC2]);
//      Uart.PutTxFifoBuffer(UART6, &buffer);
      oCapture2Acquired = 1;
    }
    
    rx4 = InputCapture.GetTimeBetweenCaptures(IC4, SCALE_US);
    if (rx4 < 0)
    {
      buffer.length = sprintf(buffer.buffer, "error\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
    else
    {
//      buffer.length = sprintf(buffer.buffer, "IC4 speed = %d,    capture = %d\r\n", rx4, InputCapture.Var.currentCaptureCountValue[IC4]);
//      Uart.PutTxFifoBuffer(UART6, &buffer);
      oCapture4Acquired = 1;
    }
  }
  
  INT8 firstIc;

  if (oCapture2Acquired && oCapture4Acquired)
  {
    oCapture2Acquired = 0;
    oCapture4Acquired = 0;

    firstIc = InputCapture.GetDirection(IC2, IC4, rx4, SCALE_US);
    if (firstIc == IC2)
    {
      buffer.length = sprintf(buffer.buffer, "DROITE\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
    else if (firstIc == IC4)
    {
      buffer.length = sprintf(buffer.buffer, "GAUCHE\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
    else
    {
      buffer.length = sprintf(buffer.buffer, "ERREUR\r\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
    }
  }
  
  oCapture2Acquired = 0;
  oCapture4Acquired = 0;
}
