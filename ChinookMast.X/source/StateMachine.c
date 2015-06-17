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
#include "..\headers\StateFunctions.h"
#include "..\headers\CommandFunctions.h"

INT8 Calib_done = 1;

extern volatile UINT32 nTurns;

// Mast general value
volatile INT8  mastCurrentPos = 0        // Actual position of Mast
              ,Mast_consigne  = 0        // New value of Mast
              ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ;

volatile BOOL  oFirstTimeInStateRight    = 1
              ,oFirstTimeInStateLeft  = 1
              ,oFirstTimeInStateStop  = 1
              ,oButtonLeft            = 0
              ,oButtonRight           = 0
              ;

BOOL  oCapture1Acquired = 0
     ,oCapture2Acquired = 0
     ,oCapture3Acquired = 0
     ,oCapture4Acquired = 0
     ,oManualMastRight  = 0
     ,oManualMastLeft   = 0
     ;


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
      pStateMast = &StateManualStop;    // stop state
    }
    else if (INIT_2_LEFT)
    {
      pStateMast = &StateManualLeft;    // Down state
    }
    else if (INIT_2_RIGHT)
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
      pStateMast = &StateManualStop;     // Stop state
    }
    else if (CALIB_2_LEFT)
    {
      pStateMast = &StateManualLeft;     // Down state
    }
    else if (CALIB_2_RIGHT)
    {
      pStateMast = &StateManualRight;     // Up state
    }
    else
    {
      pStateMast = &StateCalib;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateManualStop
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateManualStop)
  {
    if (STOP_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (STOP_2_CALIB)
    {
      pStateMast = &StateCalib;     // Calib state
    }
    else if (STOP_2_LEFT)
    {
      pStateMast = &StateManualLeft;     // Down state
    }
    else if (STOP_2_RIGHT)
    {
      pStateMast = &StateManualRight;     // Up state
    }
    else
    {
      pStateMast = &StateManualStop;   // Go to Error state by default
    }
  }
  
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateManualLeft
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateManualLeft)
  {
    if (LEFT_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (LEFT_2_CALIB)
    {
      pStateMast = &StateCalib;     // Calib state
    }
    else if (LEFT_2_STOP)
    {
      pStateMast = &StateManualStop;     // Stop state
    }
    else if (LEFT_2_RIGHT)
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
    if (RIGHT_2_INIT)
    {
      pStateMast = &StateInit;       // Init state
    }
    else if (RIGHT_2_CALIB)
    {
      pStateMast = &StateCalib;     // Calib state
    }
    else if (RIGHT_2_STOP)
    {
      pStateMast = &StateManualStop;     // Stop state
    }
    else if (RIGHT_2_LEFT)
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
  INIT_UART;
  INIT_CAN;
//  INIT_SKADI;
  INIT_SPI;
  INIT_PWM;
  INIT_I2C;
  START_INTERRUPTS;

  // Send ID to backplane by CAN protocol
  SEND_ID_TO_BACKPLANE;

  mastCurrentPos = ReadMastPosFromEeprom();
  
//  I2c.EepromSendByte(I2C4, 0x0100, -45);
//  I2c.EepromReadByte(I2C4, 0x0100, &angle);mastCurrentPos

  // Init registers for the drive
  InitDriver();
}


//===============================================================
// Name     : StateManualStop
// Purpose  : Stop stepper Mast when good Mast or problem
//===============================================================
void StateManualStop(void)
{
//  DRVA_SLEEP = 0;
  DRVB_SLEEP = 0;
  
  if (oFirstTimeInStateStop)
  {
    oFirstTimeInStateRight = 1;
    oFirstTimeInStateLeft = 1;
    oFirstTimeInStateStop = 0;

    Pwm.SetDutyCycle(PWM_2, 500);
    Pwm.SetDutyCycle(PWM_3, 500);
    WriteMastPos2Eeprom(mastCurrentPos);
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
  
  if (oFirstTimeInStateLeft)
  {
    oFirstTimeInStateRight = 1;
    oFirstTimeInStateStop = 1;
    oFirstTimeInStateLeft = 0;

//    Pwm.SetDutyCycle(PWM_2, 800);
//    Pwm.SetDutyCycle(PWM_3, 200);
    Pwm.SetDutyCycle(PWM_2, 250);
    Pwm.SetDutyCycle(PWM_3, 750);
  }
//  mastCurrentPos--;
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

  if (oFirstTimeInStateRight)
  {
    oFirstTimeInStateStop = 1;
    oFirstTimeInStateLeft = 1;
    oFirstTimeInStateRight = 0;
//    Pwm.SetDutyCycle(PWM_2, 200);
//    Pwm.SetDutyCycle(PWM_3, 800);
    Pwm.SetDutyCycle(PWM_2, 750);
    Pwm.SetDutyCycle(PWM_3, 250);
  }
//  mastCurrentPos++;
//  WriteDrive(DRVA, STATUS_Mastw);
  WriteDrive(DRVB, STATUS_Mastw);
}


//===============================================================
// Name     : StateAcquisition
// Purpose  : Get data from peripherals
//===============================================================
void StateAcquisition(void)
{

  if ( (!SW3 || oButtonRight) && (!SW2 || oButtonLeft) )
  {
    oManualMastLeft  = 0;
    oManualMastRight = 0;
  }
  else
  {
    if ( (!SW2 && !oManualMastLeft) || (oButtonLeft && !oManualMastLeft) )
    {
  //    Mast_consigne -= 1000;
      oManualMastLeft = 1;
  //    oButtonLeft = 0;
    }
    else if (SW2 && !oButtonLeft)
    {
      oManualMastLeft = 0;
    }

    if ( (!SW3 && !oManualMastRight) || (oButtonRight && !oManualMastRight) )
    {
  //    Mast_consigne += 1000;
      oManualMastRight = 1;
  //    oButtonRight = 0;
    }
    else if (SW3 && !oButtonRight)
    {
      oManualMastRight = 0;
    }
  }

  if (!SW1)
  {
    WriteMastPos2Eeprom(0);
  }

  if (oCapture2)
  {
    if (nTurns >= 7)
    {
      if (oManualMastRight)
      {
        mastCurrentPos++;
      }
      else if (oManualMastLeft)
      {
        mastCurrentPos--;
      }
      nTurns = 0;
    }
  }
  
//  INT64 rx1, rx2, rx3, rx4;
//
//  sUartLineBuffer_t buffer =
//  {
//    .buffer = {0}
//   ,.length =  0
//  };
//
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

//  if (oCapture2 && oCapture4)
//  {
//    oCapture2 = 0;
//    oCapture4 = 0;
//    rx2 = InputCapture.GetTimeBetweenCaptures(IC2, SCALE_US);
//    if (rx2 < 0)
//    {
//      buffer.length = sprintf(buffer.buffer, "error\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//    else
//    {
////      buffer.length = sprintf(buffer.buffer, "IC2 speed = %d,    capture = %d\r\n", rx2, InputCapture.Var.currentCaptureCountValue[IC2]);
////      Uart.PutTxFifoBuffer(UART6, &buffer);
//      oCapture2Acquired = 1;
//    }
//
//    rx4 = InputCapture.GetTimeBetweenCaptures(IC4, SCALE_US);
//    if (rx4 < 0)
//    {
//      buffer.length = sprintf(buffer.buffer, "error\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//    else
//    {
////      buffer.length = sprintf(buffer.buffer, "IC4 speed = %d,    capture = %d\r\n", rx4, InputCapture.Var.currentCaptureCountValue[IC4]);
////      Uart.PutTxFifoBuffer(UART6, &buffer);
//      oCapture4Acquired = 1;
//    }
//  }
  
//  INT8 firstIc;
//
//  if (oCapture2Acquired && oCapture4Acquired)
//  {
//    oCapture2Acquired = 0;
//    oCapture4Acquired = 0;
//
//    firstIc = InputCapture.GetDirection(IC2, IC4, rx4, SCALE_US);
//    if (firstIc == IC2)
//    {
//      buffer.length = sprintf(buffer.buffer, "DROITE\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//    else if (firstIc == IC4)
//    {
//      buffer.length = sprintf(buffer.buffer, "GAUCHE\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//    else
//    {
//      buffer.length = sprintf(buffer.buffer, "ERREUR\r\n");
//      Uart.PutTxFifoBuffer(UART6, &buffer);
//    }
//  }
//
//  oCapture2Acquired = 0;
//  oCapture4Acquired = 0;

//  Skadi.GetCmdMsg();
}
