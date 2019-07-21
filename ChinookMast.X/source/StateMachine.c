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

#include "StateMachine.h"
#include "DRV8711_Para.h"
#include "Interrupts.h"
#include "StateFunctions.h"
#include "CommandFunctions.h"
#include "Potentiometer.h"

extern volatile sButtonStates_t buttons;

extern volatile UINT32 rxWindAngle;
extern volatile float  rxTurbineRpm;

extern volatile float T;

extern volatile BOOL oEnableMastStopProcedure;

extern sUartLineBuffer_t buffer;

// Used for the average of the wind angle
//========================================
extern volatile UINT32 nWindAngleSamples;
extern volatile float  meanWindAngle;
//========================================

// Used for the average of the turbine rpm
//========================================
extern volatile UINT32 nTurbineRpmSamples;
extern volatile float  meanTurbineRpm;
//========================================

extern volatile sCmdValue_t windAngle
                           ,mastAngle
                           ,mastSpeed
                           ;

// Mast general value
volatile float mastCurrentSpeed   = 0   // Actual speed of Mast
              ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oNewWindAngle
                    ,oNewTurbineRpm
                    ,oTimerReg
                    ,oTimerSendData
                    ,oTimerChngMode
                    ,oAdcReady
                    ,oMastMinBlock
                    ,oMastMaxBlock
                    ;

volatile BOOL  oManualMode            = 1
              ,oCountTimeToChngMode   = 0
              ,oManualFlagChng        = 0
              ,oManualMastRight       = 0
              ,oManualMastLeft        = 0
              ;

sPotValues_t potValues = 
{
  .nSamples         = 0
 ,.potSamples       = 
  {
    .bufEmpty       = 1
   ,.bufFull        = 0
   ,.inIdx          = 0
   ,.lineBuffer     = {0}
   ,.maxBufSize     = 256
   ,.outIdx         = 0
  }
 ,.lastAverage      = 0
 ,.angle            = NULL
 ,.speed            = NULL
  
 ,.potStepSamples   = 
  {
    .bufEmpty       = 1
   ,.bufFull        = 0
   ,.inIdx          = 0
   ,.lineBuffer     = {0}
   ,.maxBufSize     = 256
   ,.outIdx         = 0
  }
 ,.potStepValue     = 0
 ,.lastBits         = 0
    
 ,.zeroInBits       = 0
    
 ,.deadZoneDetect   = 500
    
 ,.stepZero         = 0
};


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
    else if (ACQ_2_GET_MAST_DATA)
    {
      pStateMast = &StateGetMastData;
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
  // Current state = StateGetMastData
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateGetMastData)
  {
    if (GET_MAST_DATA_2_ACQ)
    {
      pStateMast = &StateAcq;
    }
    else
    {
      pStateMast = &StateGetMastData;    // Stay in current state
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
  // Current state = StateInit
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else if (pStateMast == &StateInit)
  {
    if (INIT_2_ACQ)
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
#ifdef USE_POTENTIOMETER
  INIT_ADC;
#else
  INIT_INPUT_CAPTURE;
#endif
  INIT_UART;
  INIT_SPI;
  INIT_PWM;
  INIT_I2C;
  INIT_CAN;
  START_INTERRUPTS;

  // Send ID to backplane by CAN protocol
  SEND_ID_TO_BACKPLANE;

  Timer.DelayMs(10);
  
  // Send the mode of operation to the steering wheel
  SEND_MODE_TO_STEERING_WHEEL;

  // Get last known position of the mast
  if (AbsFloat(mastAngle.currentValue) > 360)  // Error
  {
    mastAngle.previousValue = 0;
    mastAngle.currentValue  = 0;
  }
#ifdef USE_POTENTIOMETER
  if (potValues.lastAverage > ADC_TOTAL_BITS)
  {
    potValues.lastAverage = ADC_TOTAL_BITS << 1;
  }
  if (potValues.zeroInBits > ADC_TOTAL_BITS)
  {
    potValues.zeroInBits = ADC_TOTAL_BITS << 1;
  }
  if (potValues.potStepValue > POT_TO_MOTOR_RATIO)
  {
    potValues.potStepValue = POT_TO_MOTOR_RATIO << 1;
  }
  if (potValues.lastBits > (ADC_BITS_PER_REVOLUTION - 1))
  {
    potValues.lastBits = 0;
  }
  //PotAddFirstSample(&potValues);
//  potValues.potSamples.lineBuffer.buffer[potValues.potSamples.maxBufSize] = potValues.lastAverage;
//  potValues.potStepSamples.lineBuffer.buffer[potValues.potStepSamples.maxBufSize] = potValues.potStepValue;
#endif

  // Init registers for the drive
  InitDriver();
  
#ifdef USE_POTENTIOMETER
  potValues.angle = &mastAngle;
  potValues.speed = &mastSpeed;
#endif
}


//===============================================================
// Name     : StateManual
// Purpose  : Assess manual flags and adjust the mast in consequence
//===============================================================
void StateManual(void)
{
  sUartLineBuffer_t buffer;
  oManualFlagChng = 0;
  //MastManualLeft();

  if (oManualMastLeft)
  {
      MastManualLeft();
  }
  else if (oManualMastRight)
  {
      MastManualRight();
  }
  else
  {
    MastStop();
  }
}


//===============================================================
// Name     : StateGetMastData
// Purpose  : Get position of mast if in manual mode
//===============================================================
void StateGetMastData(void)
{
  oTimerReg = 0;
  
  // Update wind direction
  windAngle.previousValue = windAngle.currentValue;

  float tempWind;
  memcpy ((void *) &tempWind, (void *) &rxWindAngle, 4);

  if (nWindAngleSamples != 0)
  {
    tempWind = meanWindAngle / nWindAngleSamples;
    meanWindAngle = 0;
    nWindAngleSamples = 0;
  }
  else  // If no new sample was received
  {
    tempWind = windAngle.previousValue;   // Keep previous value as current value
  }

#ifdef USE_POTENTIOMETER
  //MastGetSpeed(&potValues, T);
  mastCurrentSpeed = potValues.speed->currentValue;
#endif

  /*
   * Some kind of modulo
   */
  if (mastAngle.currentValue > 180)
  {
    mastAngle.currentValue -= 360;
  }
  else if (mastAngle.currentValue < -180)
  {
    mastAngle.currentValue += 360;
  }

  
  // TODO
  // Re-evaluate if code is necessary
  
  /*
#ifdef USE_POTENTIOMETER
   *
   * Check mast limits
   *
//  if (AbsFloat(mastSpeed.currentValue) >= BITS_TO_DEG_TIMES_20)
  if (AbsFloat(mastSpeed.currentValue) >= 3.5f)
  {
    if ( (SignFloat(mastSpeed.currentValue) == MAST_DIR_LEFT) && (!MAST_MIN_OK) )        // Mast too far left
    {
      if(!oEnableMastStopProcedure)
      {
          LED_DEBUG4_TOGGLE;
          MastManualStop();
      }
    }
    
    if ( (SignFloat(mastSpeed.currentValue) == MAST_DIR_RIGHT) && (!MAST_MAX_OK) )  // Mast too far right
    {
      if(!oEnableMastStopProcedure)
      {
        buffer.length = sprintf(buffer.buffer, "GetMastData stop \r\n\n");
        Uart.PutTxFifoBuffer(UART6, &buffer);
        LED_DEBUG3_TOGGLE;
        MastManualStop();
      }
    }
  }
#endif
  */
}


//===============================================================
// Name     : StateReg
// Purpose  : Regulate the mast
//===============================================================
void StateReg(void)
{
  // DO NOT TEST REGULATOR ALONE. SERIOUSLY, SCARY SHIT.
  //oTimerReg = 0;
  //Regulator();
   
  RelativeWAngleRegulator();
}


//===============================================================
// Name     : StateDisconnect
// Purpose  : Send a disconnect message to the backplane
//===============================================================
void StateDisconnect(void)
{
  if (mastCurrentSpeed != 0)
  {
    MastManualStop();
  }
  
  SEND_DISCONNECT_TO_BACKPLANE;
}


//===============================================================
// Name     : StateClose
// Purpose  : Close all peripherals and put device in sleep mode
//===============================================================
void StateClose(void)
{

  INTDisableInterrupts();   // Disable all interrupts of the system.
  
  LED_ALL_OFF();

  I2c.Close(I2C4);

  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    Pwm.Close(PWM_2);
    Pwm.Close(PWM_3);

    InputCapture.Close(IC2);
    InputCapture.Close(IC4);
  }
  //==========================================================

  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    Pwm.Close(PWM_4);
    Pwm.Close(PWM_5);

    InputCapture.Close(IC1);
    InputCapture.Close(IC3);
  }
  //==========================================================

  Spi.Close(SPI4);

//  Can.Close(CAN1);

  Uart.Close(UART6);

  Timer.Close(TIMER_1);
  Timer.Close(TIMER_2);
  Timer.Close(TIMER_3);
  Timer.Close(TIMER_5);

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
  oTimerSendData = 0;

  static UINT8 iCounterToTwoSec = 0;
  
  //SEND_MAST_DIRECTION;  // Via CAN bus

  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
//    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors
  }
  //==========================================================

  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
//    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors
  }
  //==========================================================

  if (iCounterToTwoSec < 10)
//  if (iCounterToTwoSec < 1)
  {
    iCounterToTwoSec++;
  }
}


//===============================================================
// Name     : StateAcq
// Purpose  : Get data from peripherals
//===============================================================
void StateAcq(void)
{
  float tempWindAngle   = 0;
  float tempTurbineRpm  = 0;
  UINT16 tempAdcValue   = 0;
  static BOOL oModeMem  = 0;
  INT8 err = 0;

  if (oModeMem != oManualMode)
  {
    if (oManualMode)
    {
      LED_DEBUG0_ON;
    }
    else
    {
      LED_DEBUG0_OFF;
    }
    oModeMem = oManualMode;
  }

  if (oNewWindAngle)
  {
    nWindAngleSamples++;
    memcpy ((void *) &tempWindAngle, (void *) &rxWindAngle, 4);  // Copy contents of UINT32 into float
    meanWindAngle += tempWindAngle;
  }
  
  if (oNewTurbineRpm)
  {
    nTurbineRpmSamples++;
    memcpy ((void *) &tempTurbineRpm, (void *) &rxTurbineRpm, 4);  // Copy contents of UINT32 into float
    meanTurbineRpm += tempTurbineRpm;
  }

  AssessButtons();

  // TODO : re-evaluate if code is necessary ?
  /*
#ifdef USE_POTENTIOMETER
  if (oAdcReady)
  {
    oAdcReady = 0;
    tempAdcValue = Adc.Var.adcReadValues[2];
    //PotAddSample(&potValues, tempAdcValue);
    if (potValues.nSamples >= N_SAMPLES_TO_AVERAGE)
    {
      //PotAverage(&potValues);
      //MastUpdateAngle(&potValues);
    }
  }
#else
  AssessMastValues();
#endif
  */

}
