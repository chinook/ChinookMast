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

BOOL  oCapture1Acquired = 0
     ,oCapture2Acquired = 0
     ,oCapture3Acquired = 0
     ,oCapture4Acquired = 0
     ;

extern volatile sButtonStates_t buttons;

extern volatile UINT32 rxWindAngle;

extern volatile sCmdValue_t windAngle
                           ,mastAngle
                           ,mastSpeed
                           ;

// Mast general value
volatile float mastCurrentPos   = 0   // Actual position of Mast
              ,mastCurrentSpeed = 0   // Actual speed of Mast
              ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oTimer1
                    ,oTimer4
                    ,oTimer5
                    ;

volatile BOOL  oManualMode            = 1
              ,oCalibMode             = 0
              ,oCountTimeToChngMode   = 0
              ,oManualFlagChng        = 0
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
  // Current state = StateAcq
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  if (pStateMast == &StateAcq)
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
    else if (ACQ_2_SEND_DATA)
    {
      pStateMast = &StateSendData;
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
  // Current state = StateManual
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateManual)
  {
    if (MANUAL_2_ACQ)
    {
      pStateMast = &StateAcq;
    }
    else
    {
      pStateMast = &StateManual;    // Stay in current state
    }
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // Current state = StateSendData
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateSendData)
  {
    if (SEND_DATA_2_ACQ)
    {
      pStateMast = &StateAcq;
    }
    else
    {
      pStateMast = &StateSendData;    // Stay in current state
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
  // Current state = StateInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateInit)
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
  // Current state = StateIdle
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

  // Get last known position of the mast
  ReadMastPosFromEeprom();

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
  oCalibMode = 0;
}


//===============================================================
// Name     : StateReg
// Purpose  : Regulate the mast
//===============================================================
void StateReg(void)
{
  oTimer1 = 0;

  Regulator();

  mastCurrentPos = mastAngle.currentValue;

  WriteMastPos2Eeprom();
}


//===============================================================
// Name     : StateDisconnect
// Purpose  : Send a disconnect message to the backplane
//===============================================================
void StateDisconnect(void)
{
  MastManualStop();
  
  SEND_DISCONNECT_TO_BACKPLANE;
}


//===============================================================
// Name     : StateClose
// Purpose  : Close all peripherals and put device in sleep mode
//===============================================================
void StateClose(void)
{

  INTDisableInterrupts();   // Disable all interrupts of the system.

//  Wdt.Disable();

  I2c.Close(I2C4);

  Pwm.Close(PWM_2);
  Pwm.Close(PWM_3);
//  Pwm.Close(PWM_4);
//  Pwm.Close(PWM_5);

  Spi.Close(SPI4);

//  Can.Close(CAN2);

  Uart.Close(UART6);

//  InputCapture.Close(IC1);
  InputCapture.Close(IC2);
//  InputCapture.Close(IC3);
  InputCapture.Close(IC4);

  Timer.Close(TIMER_1);
  Timer.Close(TIMER_2);
  Timer.Close(TIMER_3);
  Timer.Close(TIMER_5);

//  OSCCONSET = 0x10;         // Sleep mode

}


//===============================================================
// Name     : StateIdle
// Purpose  : Wait for power-off
//===============================================================
void StateIdle(void)
{
  return;
}


//===============================================================
// Name     : StateSendData
// Purpose  : Send useful data to other devices
//===============================================================
void StateSendData(void)
{
  oTimer4 = 0;
  
  BYTE txMastAngle[4];

  memcpy(&txMastAngle[0], (void *) &mastCurrentPos, 4);   // mastCurrentPos is a float

  Can.SendByteArray(CAN1, 0x55, &txMastAngle[0], 4);
//  
//  WriteMastPos2Eeprom();
//
//  sUartLineBuffer_t buffer =
//  {
//    .buffer = {0}
//   ,.length =  0
//  };
//
//  buffer.length = sprintf(buffer.buffer, "PWM = %\r\n");
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
  }
  else
  {
    LED_DEBUG0_OFF;
  }
  
  if (buttons.buttons.bits.boardSw1 != SW1)
  {
    buttons.buttons.bits.boardSw1    = SW1;
    buttons.chng.bits.boardSw1       =   1;
  }

  if (buttons.buttons.bits.boardSw2 != SW2)
  {
    buttons.buttons.bits.boardSw2    = SW2;
    buttons.chng.bits.boardSw2       =   1;
  }

  if (buttons.buttons.bits.boardSw3 != SW3)
  {
    buttons.buttons.bits.boardSw3    = SW3;
    buttons.chng.bits.boardSw3       =   1;
  }

  AssessButtons();
  
//  INT64 rx2, rx4;
//
//  if (oCapture2 && oCapture4)
//  {
//    oCapture2 = 0;
//    oCapture4 = 0;
//
//    rx2 = InputCapture.GetTimeBetweenCaptures(IC2, SCALE_US);
//
//    rx4 = InputCapture.GetTimeBetweenCaptures(IC4, SCALE_US);
//
//    if (ABS(100 - rx2*100/rx4) < 10)
//    {
//      oCapture2Acquired = 1;
//      oCapture4Acquired = 1;
//    }
//    else
//    {
//      oCapture2Acquired = 0;
//      oCapture4Acquired = 0;
//    }
//  }
//
//  INT8 firstIc;
//
//  if (oCapture2Acquired && oCapture4Acquired)
//  {
//    oCapture2Acquired = 0;
//    oCapture4Acquired = 0;
//
//    firstIc = InputCapture.GetDirection(IC2, IC4, rx4, SCALE_US);
//
//    if (firstIc == IC2)
//    {
////      buffer.length = sprintf(buffer.buffer, "DROITE\r\n");
////      Uart.PutTxFifoBuffer(UART6, &buffer);
////      mastCurrentSpeed = (rx2 + rx4) / (2*49);
//      mastCurrentSpeed = (rx2 + rx4) / 98;
//    }
//    else if (firstIc == IC4)
//    {
////      buffer.length = sprintf(buffer.buffer, "GAUCHE\r\n");
////      Uart.PutTxFifoBuffer(UART6, &buffer);
////      mastCurrentSpeed = - (rx2 + rx4) / (2*49);
//      mastCurrentSpeed = - (rx2 + rx4) / 98;
//    }
////    else
////    {
////      buffer.length = sprintf(buffer.buffer, "ERREUR\r\n");
////      Uart.PutTxFifoBuffer(UART6, &buffer);
////    }
//  }
//
//  oCapture2Acquired = 0;
//  oCapture4Acquired = 0;

//  Skadi.GetCmdMsg();
}
