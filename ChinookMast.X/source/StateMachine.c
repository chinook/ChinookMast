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
#include "..\headers\Interrupts.h"
#include "..\headers\StateFunctions.h"
#include "..\headers\CommandFunctions.h"

INT8 oCalibDone = 1;

extern volatile UINT32 nTurns;

extern volatile sButtonStates_t buttons;

// Mast general value
volatile INT8  mastCurrentPos = 0        // Actual position of Mast
              ,Mast_consigne  = 0        // New value of Mast
              ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oTimer1
                    ,oTimer5
                    ;

volatile BOOL  oManualMode            = 1
              ,oCountTimeToChngMode   = 0
              ,oManualFlagChng        = 0
              ,oCapture1Acquired      = 0
              ,oCapture2Acquired      = 0
              ,oCapture3Acquired      = 0
              ,oCapture4Acquired      = 0
              ,oManualMastRight       = 0
              ,oManualMastLeft        = 0
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
    else if (INIT_2_ACQ)
    {
      pStateMast = &StateAcq;
    }
    else
    {
      pStateMast = &StateInit;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateCalib
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateCalib)
  {
    if (CALIB_2_ACQ)
    {
      pStateMast = &StateAcq;
    }
    else
    {
      pStateMast = &StateCalib;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateAcq
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateAcq)
  {
    if (ACQ_2_DISCONNECT)
    {
      pStateMast = &StateDisconnect;
    }
    else if (ACQ_2_MANUAL)
    {
      pStateMast = &StateManual;
    }
    else if (ACQ_2_REG)
    {
      pStateMast = &StateReg;
    }
    else
    {
      pStateMast = &StateAcq;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateReg
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateReg)
  {
    if (REG_2_ACQ)
    {
      pStateMast = &StateAcq;
    }
    else
    {
      pStateMast = &StateReg;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateDisconnect
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateDisconnect)
  {
    if (DISCONNECT_2_CLOSE)
    {
      pStateMast = &StateClose;
    }
    else
    {
      pStateMast = &StateDisconnect;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateClose
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateClose)
  {
    if (CLOSE_2_IDLE)
    {
      pStateMast = &StateIdle;
    }
    else
    {
      pStateMast = &StateClose;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateIDLE
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateIdle)
  {
    if (IDLE_2_INIT)
    {
      pStateMast = &StateInit;
    }
    else
    {
      pStateMast = &StateIdle;    // Stay in current state
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

//  sUartLineBuffer_t buffer =
//  {
//    .buffer = {0}
//   ,.length =  0
//  };
//
//    buffer.length = sprintf(buffer.buffer, "Left = %x, Right = %x, oCounting = %x, oTimer5 = %x\r\n", buttons.buttons.bits.steerWheelSw1, buttons.buttons.bits.steerWheelSw10, oCountTimeToChngMode, oTimer5);
//  Uart.PutTxFifoBuffer(UART6, &buffer);
  
//  I2c.EepromSendByte(I2C4, 0x0100, -45);
//  I2c.EepromReadByte(I2C4, 0x0100, &angle);mastCurrentPos

  // Init registers for the drive
  InitDriver();
}


//===============================================================
// Name     : StateManual
// Purpose  : Assess manual flags and adjust the mast in consequence
//===============================================================
void StateManual(void)
{
  oManualFlagChng = 0;

  if (!oManualMastLeft && !oManualMastRight)
  {
    MastManualStop();
  }
  else if (oManualMastLeft)
  {
    MastManualLeft();
  }
  else if (oManualMastRight)
  {
    MastManualRight();
  }
}


//===============================================================
// Name     : StateCalib
// Purpose  : Calibrate the mast
//===============================================================
void StateCalib(void)
{
  oCalibDone = 0;
}


//===============================================================
// Name     : StateReg
// Purpose  : Regulate the mast
//===============================================================
void StateReg(void)
{
  return;
}


//===============================================================
// Name     : StateDisconnect
// Purpose  : Send a disconnect message to the backplane
//===============================================================
void StateDisconnect(void)
{
  return;
}


//===============================================================
// Name     : StateClose
// Purpose  : Close all peripherals
//===============================================================
void StateClose(void)
{
  oCalibDone = 0;
}


//===============================================================
// Name     : StateIdle
// Purpose  : Wait for power-off
//===============================================================
void StateIdle(void)
{
  oCalibDone = 0;
}


//===============================================================
// Name     : StateAcq
// Purpose  : Get data from peripherals
//===============================================================
void StateAcq(void)
{

  sUartLineBuffer_t buffer =
  {
    .buffer = {0}
   ,.length =  0
  };

  if (oManualMode)
  {
    LED_DEBUG0_ON;
    LED_DEBUG1_OFF;
  }
  else
  {
    LED_DEBUG0_OFF;
    LED_DEBUG1_ON;
  }
  
  if (buttons.buttons.bits.boardSw1 != SW1)
  {
    buttons.buttons.bits.boardSw1  = SW1;
    buttons.chng.bits.boardSw1     = 1;
  }

  if (buttons.buttons.bits.boardSw2 != SW2)
  {
    buttons.buttons.bits.boardSw2  = SW2;
    buttons.chng.bits.boardSw2     = 1;
  }

  if (buttons.buttons.bits.boardSw3 != SW3)
  {
    buttons.buttons.bits.boardSw3  = SW3;
    buttons.chng.bits.boardSw3     = 1;
  }

  AssessButtons();

//  if (buttons.chng.byte)
//  {
//
//    if (buttons.chng.bits.boardSw1)
//    {
//      buttons.chng.bits.boardSw1 = 0;
//
//      if (!buttons.buttons.bits.boardSw1)     // If SW1 is pressed
//      {
//        mastCurrentPos = 0;
//        WriteMastPos2Eeprom (mastCurrentPos); // Write zero to EEPROM
//      }
//    }
//
//
//    if (buttons.chng.bits.boardSw2)
//    {
//      buttons.chng.bits.boardSw2 = 0;
//
//      if (!buttons.buttons.bits.boardSw2)     // If SW2 is pressed
//      {
//        if (!buttons.buttons.bits.boardSw3)   // And SW3 is pressed
//        {
//          oCountTimeToChngMode = 1;           // Start procedure to change manual mode
//          Timer.EnableInterrupt(TIMER_5);
//          Timer.Reset(TIMER_5);
//          oTimer5 = 0;
//
//          oManualMastLeft  = 0;               // Stop moving
//          oManualMastRight = 0;
//          oManualFlagChng = 1;
//        }
//        else if (oManualMode)
//        {
//          oManualMastLeft = 1;
//          oManualFlagChng = 1;
//        }
//      }
//      else                          // If SW2 is not pressed
//      {
//        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
//        {
//          oCountTimeToChngMode = 0;
//          Timer.DisableInterrupt(TIMER_5);
//
//          if (oTimer5)              // If at least one second has passed
//          {
//            oManualMode ^= 1;       // Change mode
//          }
//        }
//        else if (oManualMode)
//        {
//          oManualMastLeft = 0;
//          oManualFlagChng = 1;
//        }
//      }
//    }
//
//
//    if (buttons.chng.bits.boardSw3)
//    {
//      buttons.chng.bits.boardSw3 = 0;
//
//      if (!buttons.buttons.bits.boardSw3)     // If SW3 is pressed
//      {
//        if (!buttons.buttons.bits.boardSw2)   // And SW2 is pressed
//        {
//          oCountTimeToChngMode = 1;           // Start procedure to change manual mode
//          Timer.EnableInterrupt(TIMER_5);
//          Timer.Reset(TIMER_5);
//          oTimer5 = 0;
//
//          oManualMastLeft  = 0;               // Stop moving
//          oManualMastRight = 0;
//          oManualFlagChng = 1;
//        }
//        else if (oManualMode)
//        {
//          oManualMastRight = 1;
//          oManualFlagChng = 1;
//        }
//      }
//      else                          // If SW3 is not pressed
//      {
//        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
//        {
//          oCountTimeToChngMode = 0;
//          Timer.DisableInterrupt(TIMER_5);
//
//          if (oTimer5)              // If at least one second has passed
//          {
//            oManualMode ^= 1;       // Change mode
//          }
//        }
//        else if (oManualMode)
//        {
//          oManualMastRight = 0;
//          oManualFlagChng = 1;
//        }
//      }
//    }
//
//
//    if (buttons.chng.bits.steerWheelSw1)
//    {
//      buttons.chng.bits.steerWheelSw1 = 0;
//
//      if (buttons.buttons.bits.steerWheelSw1)       // If left switch on steering wheel is pressed
//      {
//        if (buttons.buttons.bits.steerWheelSw10)    // And right switch on steering wheel is pressed
//        {
//          oCountTimeToChngMode = 1;                 // Start procedure to change manual mode
//          Timer.EnableInterrupt(TIMER_5);
//          Timer.Reset(TIMER_5);
//          oTimer5 = 0;
//
//          oManualMastLeft  = 0;                     // Stop moving
//          oManualMastRight = 0;
//          oManualFlagChng = 1;
//        }
//        else if (oManualMode)
//        {
//          oManualMastLeft = 1;
//          oManualFlagChng = 1;
//        }
//      }
//      else                          // If left switch on steering wheel is not pressed
//      {
//        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
//        {
//          oCountTimeToChngMode = 0;
//          Timer.DisableInterrupt(TIMER_5);
//
//          if (oTimer5)              // If at least one second has passed
//          {
//            oManualMode ^= 1;       // Change mode
//          }
//        }
//        else if (oManualMode)
//        {
//          oManualMastLeft = 0;
//          oManualFlagChng = 1;
//        }
//      }
//    }
//
//
//    if (buttons.chng.bits.steerWheelSw10)
//    {
//      buttons.chng.bits.steerWheelSw10 = 0;
//
//      if (buttons.buttons.bits.steerWheelSw10)      // If right switch on steering wheel is pressed
//      {
//        if (buttons.buttons.bits.steerWheelSw1)     // And left switch on steering wheel is pressed
//        {
//          oCountTimeToChngMode = 1;                 // Start procedure to change manual mode
//          Timer.EnableInterrupt(TIMER_5);
//          Timer.Reset(TIMER_5);
//          oTimer5 = 0;
//
//          oManualMastLeft  = 0;                     // Stop moving
//          oManualMastRight = 0;
//          oManualFlagChng = 1;
//        }
//        else if (oManualMode)
//        {
//          oManualMastRight = 1;
//          oManualFlagChng = 1;
//        }
//      }
//      else                          // If right switch on steering wheel is not pressed
//      {
//        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
//        {
//          oCountTimeToChngMode = 0;
//          Timer.DisableInterrupt(TIMER_5);
//
//          if (oTimer5)              // If at least one second has passed
//          {
//            oManualMode ^= 1;       // Change mode
//          }
//        }
//        else if (oManualMode)
//        {
//          oManualMastRight = 0;
//          oManualFlagChng = 1;
//        }
//      }
//    }
//  }

//  if (oCapture2)
//  {
//    oCapture2 = 0;
//    if (oManualMastRight)
//    {
//      nTurns++;
//      if (nTurns >= PULSE_PER_DEGREE)
//      {
//        mastCurrentPos++;
//        nTurns = 0;
//      }
//    }
//    else if (oManualMastLeft)
//    {
//      nTurns--;
//      if (nTurns <= -PULSE_PER_DEGREE)
//      {
//        mastCurrentPos--;
//        nTurns = 0;
//      }
//    }
//  }
  
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
