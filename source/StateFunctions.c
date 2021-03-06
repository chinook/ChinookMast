//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : StateFunctions.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the C file for the functions of the state machine of
//           the system.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\StateFunctions.h"
#include "..\headers\CommandFunctions.h"
#include "..\headers\Potentiometer.h"


void PRINTF(const char * message)
{
	sUartLineBuffer_t buffer;
	buffer.length = sprintf(buffer.buffer, message);	
	Uart.PutTxFifoBuffer(UART6, &buffer);
}

// Private functions prototypes
void SetZeroFromSteeringWheel (void);


//==============================================================================
// Variable declarations
//==============================================================================

// EEPROM register containing mast last orientation
I2cEepromInternalRegister_t eepromFirstRegister =
{
  .index.pageIndex = 0b00010000
 ,.index.byteIndex = 0b000000
};

// All the buttons used. 3 on the steering wheel, 3 on the board
volatile sButtonStates_t buttons =
{
  .chng   .byte = 0
 ,.buttons.byte = 0

 // switches on board are at 1 when not pressed
 ,.buttons.bits.boardSw1 = 1
 ,.buttons.bits.boardSw2 = 1
 ,.buttons.bits.boardSw3 = 1
};

extern volatile float  mastCurrentSpeed       // Actual speed of Mast
                      ;

extern sPotValues_t potValues;

extern volatile sCmdValue_t mastAngle;        // Discrete position of mast

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oEnableMastStopProcedure // Stop procedure using TIMER 2
                    ,oTimerReg                // From TIMER 1
                    ,oTimerChngMode           // Flag used when changing mode
                    ,oManualMode
                    ,oCountTimeToChngMode     // Flag used when changing mode
                    ,oManualFlagChng          // In manual mode, indicates that a change has occured on the buttons
                    ,oManualMastRight
                    ,oManualMastLeft
                    ,oTimerSetZero
                    ,oSetZeroCounterOccured
                    ;

extern volatile UINT16 setZeroCounter;


//==============================================================================
// EEPROM functions
//==============================================================================

/*
 * Write last recorded position of mast to EEPROM
 */
void WriteMastPos2Eeprom (void)
{
#ifdef USE_POTENTIOMETER
  UINT8 dataBuffer[19];
  dataBuffer[0] = I2c.Var.eepromAddress.byte;
  dataBuffer[1] = eepromFirstRegister.address.highByte;
  dataBuffer[2] = eepromFirstRegister.address.lowByte;

  memcpy(&dataBuffer[3 ], (void *) &mastAngle.currentValue    , 4);
  memcpy(&dataBuffer[7 ], (void *) &potValues.lastAverage     , 4);
  memcpy(&dataBuffer[11], (void *) &potValues.lastBits        , 2);
  memcpy(&dataBuffer[13], (void *) &potValues.zeroInBits      , 4);
  memcpy(&dataBuffer[17], (void *) &potValues.potStepValue    , 2);

  while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end
  while(I2c.Var.oI2cWriteIsRunning[I2C4]); // Wait for any I2C4 write sequence to end

  I2c.AddDataToFifoWriteQueue(I2C4, &dataBuffer[0], 19, TRUE);
#else
  UINT8 dataBuffer[7];
  dataBuffer[0] = I2c.Var.eepromAddress.byte;
  dataBuffer[1] = eepromFirstRegister.address.highByte;
  dataBuffer[2] = eepromFirstRegister.address.lowByte;

  memcpy(&dataBuffer[3], (void *) &mastAngle.currentValue, 4);

  while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end
  while(I2c.Var.oI2cWriteIsRunning[I2C4]); // Wait for any I2C4 write sequence to end

  I2c.AddDataToFifoWriteQueue(I2C4, &dataBuffer[0], 7, TRUE);
#endif
}

/*
 * Read last recorded position of mast. Used at init
 */
void ReadMastPosFromEeprom (void)
{
#ifdef USE_POTENTIOMETER
  UINT8 buffer[16];
  UINT8 slaveAddPlusRegBuf[3];

  slaveAddPlusRegBuf[0] = I2c.Var.eepromAddress.byte;
  slaveAddPlusRegBuf[1] = eepromFirstRegister.address.highByte;
  slaveAddPlusRegBuf[2] = eepromFirstRegister.address.lowByte;

  while(I2c.Var.oI2cWriteIsRunning[I2C4]);  // Wait for any I2C4 write sequence to end
  while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end

  I2c.AddDataToFifoReadQueue(I2C4, &slaveAddPlusRegBuf[0], 3, 16);

  while(I2c.Var.oI2cReadIsRunning[I2C4]); // Wait for the read sequence to end

  I2c.ReadRxFifo(I2C4, &buffer[0], 16);

  memcpy((void *) &mastAngle.currentValue     , &buffer[0] , 4);
  memcpy((void *) &potValues.lastAverage      , &buffer[4] , 4);
  memcpy((void *) &potValues.lastBits         , &buffer[8] , 2);
  memcpy((void *) &potValues.zeroInBits       , &buffer[10], 4);
  memcpy((void *) &potValues.potStepValue     , &buffer[14], 2);

  mastAngle.previousValue = mastAngle.currentValue;
#else
  UINT8 mastPos[4];
  UINT8 slaveAddPlusRegBuf[3];

  slaveAddPlusRegBuf[0] = I2c.Var.eepromAddress.byte;
  slaveAddPlusRegBuf[1] = eepromFirstRegister.address.highByte;
  slaveAddPlusRegBuf[2] = eepromFirstRegister.address.lowByte;

  while(I2c.Var.oI2cWriteIsRunning[I2C4]);  // Wait for any I2C4 write sequence to end
  while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end

  I2c.AddDataToFifoReadQueue(I2C4, &slaveAddPlusRegBuf[0], 3, 4);

  while(I2c.Var.oI2cReadIsRunning[I2C4]); // Wait for the read sequence to end

  I2c.ReadRxFifo(I2C4, &mastPos[0], 4);

  memcpy((void *) &mastAngle.currentValue, &mastPos[0], 4);

  mastAngle.previousValue = mastAngle.currentValue;
#endif
}


//==============================================================================
// Mast manual functions
//==============================================================================
void MastManualLeft (void)
{
  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    DRVB_SLEEP = 1;

//    Pwm.SetDutyCycle(PWM_2, 600);
//    Pwm.SetDutyCycle(PWM_3, 400);
    
    Pwm.SetDutyCycle(PWM_2, 750);
    Pwm.SetDutyCycle(PWM_3, 250);

    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    DRVA_SLEEP = 1;

//    Pwm.SetDutyCycle(PWM_4, 600);
//    Pwm.SetDutyCycle(PWM_5, 400);
//    
    Pwm.SetDutyCycle(PWM_4, 750);
    Pwm.SetDutyCycle(PWM_5, 250);

    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}


void MastManualRight (void)
{
  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    DRVB_SLEEP = 1;
    
//    Pwm.SetDutyCycle(PWM_2, 400);
//    Pwm.SetDutyCycle(PWM_3, 600);
//    
    Pwm.SetDutyCycle(PWM_2, 250);
    Pwm.SetDutyCycle(PWM_3, 750);

    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    DRVA_SLEEP = 1;

//    Pwm.SetDutyCycle(PWM_4, 400);
//    Pwm.SetDutyCycle(PWM_5, 600);
    
    Pwm.SetDutyCycle(PWM_4, 250);
    Pwm.SetDutyCycle(PWM_5, 750);

    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}


void MastManualStop (void)
{
  PRINTF("MastManualStop\r\n");
  //LED_STATUS_ON;
  //oEnableMastStopProcedure = 1;     // Start stop procedure using TIMER 2
  oEnableMastStopProcedure = 0;     //Debug mast pulse problem by removing the slow stop and just setting PWM' back to zero
  //LED_STATUS_TOGGLE;

  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
    Pwm.SetDutyCycle(PWM_2, 500);
    Pwm.SetDutyCycle(PWM_3, 500);
    DRVB_SLEEP = 0;
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
    Pwm.SetDutyCycle(PWM_4, 500);
    Pwm.SetDutyCycle(PWM_5, 500);
    DRVA_SLEEP = 0;
  }
  //==========================================================
}


//==============================================================================
// Buttons functions
//==============================================================================
void AssessButtons (void)
{
  // <editor-fold defaultstate="collapsed" desc="Check changes on board">
  // <editor-fold defaultstate="collapsed" desc="Change on SW1 on board">
  if (buttons.buttons.bits.boardSw1 != SW1)
  {
    buttons.buttons.bits.boardSw1    = SW1;
    buttons.chng.bits.boardSw1       =   1;

  }
  // </editor-fold>

  // <editor-fold defaultstate="collapsed" desc="Change on SW2 on board">
  if (buttons.buttons.bits.boardSw2 != SW2)
  {
    buttons.buttons.bits.boardSw2    = SW2;
    buttons.chng.bits.boardSw2       =   1;
  }
  // </editor-fold>

  // <editor-fold defaultstate="collapsed" desc="Change on SW3 on board">
  if (buttons.buttons.bits.boardSw3 != SW3)
  {
    buttons.buttons.bits.boardSw3    = SW3;
    buttons.chng.bits.boardSw3       =   1;
  }
  // </editor-fold>
  // </editor-fold>

  // <editor-fold defaultstate="collapsed" desc="Assess changes">
  if (buttons.chng.byte)  // If any change has occured on any button
  {
    // <editor-fold defaultstate="collapsed" desc="SW1 on board">
    if (buttons.chng.bits.boardSw1)
    {
      buttons.chng.bits.boardSw1 = 0;

      if (!buttons.buttons.bits.boardSw1)     // If SW1 is pressed
      {
        mastAngle.currentValue = 0;
        mastAngle.previousValue = 0;
#ifdef USE_POTENTIOMETER
        potValues.zeroInBits = potValues.lastAverage;
        potValues.potStepValue = POT_TO_MOTOR_RATIO >> 1;
#endif
        WriteMastPos2Eeprom (); // Write zero to EEPROM

        SEND_CALIB_DONE;  // Confirm that the calib is done
      }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="SW2 on board">
    if (buttons.chng.bits.boardSw2)
    {
      buttons.chng.bits.boardSw2 = 0;

      if (!buttons.buttons.bits.boardSw2)     // If SW2 is pressed
      {
        if (!buttons.buttons.bits.boardSw3)   // And SW3 is pressed
        {
          oCountTimeToChngMode = 1;           // Start procedure to change manual mode
          Timer.EnableInterrupt(TIMER_5);
          Timer.Reset(TIMER_5);
          oTimerChngMode = 0;

          oManualMastLeft  = 0;               // Stop moving
          oManualMastRight = 0;

          if (oManualMode)
          {
            oManualFlagChng = 1;
          }
        }
        else if (oManualMode && !oEnableMastStopProcedure)
        {
          oManualMastLeft = 1;
          LED_DEBUG0_ON;
          oManualFlagChng = 1;
        }
      }
      else                          // If SW2 is not pressed
      {
        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
        {
          oCountTimeToChngMode = 0;
          Timer.DisableInterrupt(TIMER_5);

          if (oTimerChngMode)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
            if (mastCurrentSpeed != 0)
            {
              MastManualStop();
            }
            SEND_MODE_TO_STEERING_WHEEL;  // Send change of mode to the steering wheel
          }
        }
        else if (oManualMode)
        {
          if (oManualMastLeft)
          {
            oManualMastLeft = 0;
            LED_DEBUG0_OFF;
            oManualFlagChng = 1;
          }
        }
      }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="SW3 on board">
    if (buttons.chng.bits.boardSw3)
    {
      buttons.chng.bits.boardSw3 = 0;

      if (!buttons.buttons.bits.boardSw3)     // If SW3 is pressed
      {
        if (!buttons.buttons.bits.boardSw2)   // And SW2 is pressed
        {
          oCountTimeToChngMode = 1;           // Start procedure to change manual mode
          Timer.EnableInterrupt(TIMER_5);
          Timer.Reset(TIMER_5);
          oTimerChngMode = 0;

          oManualMastLeft  = 0;               // Stop moving
          oManualMastRight = 0;

          if (oManualMode)
          {
            oManualFlagChng = 1;
          }
        }
        else if (oManualMode && !oEnableMastStopProcedure)
        {
          oManualMastRight = 1;
          LED_DEBUG1_ON;
          oManualFlagChng = 1;
        }
      }
      else                          // If SW3 is not pressed
      {
        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
        {
          oCountTimeToChngMode = 0;
          Timer.DisableInterrupt(TIMER_5);

          if (oTimerChngMode)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
            if (mastCurrentSpeed != 0)
            {
              MastManualStop();
            }
            SEND_MODE_TO_STEERING_WHEEL;  // Send change of mode to the steering wheel
          }
        }
        else if (oManualMode)
        {
          if (oManualMastRight)
          {
            oManualMastRight = 0;
            LED_DEBUG1_OFF;
            oManualFlagChng = 1;
          }
        }
      }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="SW1 on steering wheel">
    if (buttons.chng.bits.steerWheelSw1)
    {
      buttons.chng.bits.steerWheelSw1 = 0;

      if (buttons.buttons.bits.steerWheelSw1)       // If left switch on steering wheel is pressed
      {
        if (buttons.buttons.bits.steerWheelSw10)    // And right switch on steering wheel is pressed
        {
          oCountTimeToChngMode = 1;                 // Start procedure to change manual mode
          Timer.Reset(TIMER_5);
          Timer.EnableInterrupt(TIMER_5);
          oTimerChngMode = 0;

          oManualMastLeft  = 0;                     // Stop moving
          oManualMastRight = 0;

          if (oManualMode)
          {
            oManualFlagChng = 1;
          }
        }
        else if (oManualMode && !oEnableMastStopProcedure)
        {
          oManualMastLeft = 1;
          oManualFlagChng = 1;
        }
      }
      else                          // If left switch on steering wheel is not pressed
      {
        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
        {
          Timer.DisableInterrupt(TIMER_5);
          oCountTimeToChngMode = 0;

          if (oTimerChngMode)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
            if (mastCurrentSpeed != 0)
            {
              MastManualStop();
            }
            SEND_MODE_TO_STEERING_WHEEL;  // Send change of mode to the steering wheel
          }
        }
        else if (oManualMode)
        {
          if (oManualMastLeft)
          {
            oManualMastLeft = 0;
            oManualFlagChng = 1;
          }
        }
      }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="SW4 on steering wheel">
    if (buttons.chng.bits.steerWheelSw4)
    {
      buttons.chng.bits.steerWheelSw4 = 0;

      if (buttons.buttons.bits.steerWheelSw4)     // If SW1 is pressed
      {
//        mastAngle.currentValue = 0;
//        mastAngle.previousValue = 0;
//        
//#ifdef USE_POTENTIOMETER
//        potValues.zeroInBits = potValues.lastAverage;
//        potValues.potStepValue = POT_TO_MOTOR_RATIO >> 1;
//#endif
//
//        WriteMastPos2Eeprom (); // Write zero to EEPROM
//
//        SEND_CALIB_DONE;  // Confirm that the calib is done
        
//        SetZeroFromSteeringWheel();
        
        oTimerSetZero = 1;
        
      }
      else
      {
        oTimerSetZero = 0;
        setZeroCounter = 0;
        oSetZeroCounterOccured = 0;
      }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="SW10 on steering wheel">
    if (buttons.chng.bits.steerWheelSw10)
    {
      buttons.chng.bits.steerWheelSw10 = 0;

      if (buttons.buttons.bits.steerWheelSw10)      // If right switch on steering wheel is pressed
      {
        if (buttons.buttons.bits.steerWheelSw1)     // And left switch on steering wheel is pressed
        {
          oCountTimeToChngMode = 1;                 // Start procedure to change manual mode
          Timer.Reset(TIMER_5);
          Timer.EnableInterrupt(TIMER_5);
          oTimerChngMode = 0;

          oManualMastLeft  = 0;                     // Stop moving
          oManualMastRight = 0;

          if (oManualMode)
          {
            oManualFlagChng = 1;
          }
        }
        else if (oManualMode && !oEnableMastStopProcedure)
        {
          oManualMastRight = 1;
          oManualFlagChng = 1;
        }
      }
      else                          // If right switch on steering wheel is not pressed
      {
        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
        {
          Timer.DisableInterrupt(TIMER_5);
          oCountTimeToChngMode = 0;

          if (oTimerChngMode)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
            if (mastCurrentSpeed != 0)
            {
              MastManualStop();
            }
            SEND_MODE_TO_STEERING_WHEEL;  // Send change of mode to the steering wheel
          }
        }
        else if (oManualMode)
        {
          if (oManualMastRight)
          {
            oManualMastRight = 0;
            oManualFlagChng = 1;
          }
        }
      }
    }
    // </editor-fold>
  }
  // </editor-fold>
  
  if (oTimerSetZero && oSetZeroCounterOccured)
  {
    oTimerSetZero = 0;
    setZeroCounter = 0;
    oSetZeroCounterOccured = 0;
    SetZeroFromSteeringWheel();
  }
}

void SetZeroFromSteeringWheel (void)
{
  mastAngle.currentValue = 0;
  mastAngle.previousValue = 0;
        
#ifdef USE_POTENTIOMETER
  potValues.zeroInBits = potValues.lastAverage;
  potValues.potStepValue = POT_TO_MOTOR_RATIO >> 1;
#endif

  WriteMastPos2Eeprom (); // Write zero to EEPROM

  SEND_CALIB_DONE;  // Confirm that the calib is done
}


//==============================================================================
// Math functions
//==============================================================================

INT8 SignFloat (float value)
{
  return (value >= 0) ? 1 : -1;
}

INT8 SignInt (INT32 value)
{
  return (value >= 0) ? 1 : -1;
}

float AbsFloat (float value)
{
  return (value >= 0) ? value : -value;
}

INT32 AbsInt (INT32 value)
{
  return (value >= 0) ? value : -value;
}

float MaxFloat (float v1, float v2)
{
  return (v1 >= v2) ? v1 : v2;
}

INT32 MaxInt (INT32 v1, INT32 v2)
{
  return (v1 >= v2) ? v1 : v2;
}

float MinFloat (float v1, float v2)
{
  return (v1 <= v2) ? v1 : v2;
}

INT32 MinInt (INT32 v1, INT32 v2)
{
  return (v1 <= v2) ? v1 : v2;
}