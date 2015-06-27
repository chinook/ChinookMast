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


//==============================================================================
// Variable declarations
//==============================================================================
//const UINT16 EEPROM_POS_REGISTER = 0x0200;

I2cEepromInternalRegister_t eepromFirstRegister =
{
  .index.pageIndex = 0b00010000
 ,.index.byteIndex = 0b000000
};

volatile sButtonStates_t buttons =
{
  .chng   .byte = 0
 ,.buttons.byte = 0
          
 ,.buttons.bits.boardSw1 = 1
 ,.buttons.bits.boardSw2 = 1
 ,.buttons.bits.boardSw3 = 1
};

extern volatile float  mastCurrentSpeed      // Actual speed of Mast
                      ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oEnableMastStopProcedure
                    ,oTimerReg
                    ,oTimerChngMode
                    ,oManualMode
                    ,oCountTimeToChngMode
                    ,oManualFlagChng
                    ,oManualMastRight
                    ,oManualMastLeft
                    ;

extern volatile sCmdValue_t mastAngle;

//==============================================================================
// State Machine private functions prototypes
//==============================================================================


//==============================================================================
// EEPROM functions
//==============================================================================
void WriteMastPos2Eeprom (void)
{
  UINT8 dataBuffer[7];
  dataBuffer[0] = I2c.Var.eepromAddress.byte;
  dataBuffer[1] = eepromFirstRegister.address.highByte;
  dataBuffer[2] = eepromFirstRegister.address.lowByte;
//  dataBuffer[1] = EEPROM_POS_REGISTER >> 8;
//  dataBuffer[2] = EEPROM_POS_REGISTER;

  memcpy(&dataBuffer[3], (void *) &mastAngle.currentValue, 4);

  while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end
  while(I2c.Var.oI2cWriteIsRunning[I2C4]); // Wait for any I2C4 write sequence to end

  I2c.AddDataToFifoWriteQueue(I2C4, &dataBuffer[0], 7, TRUE);
}


void ReadMastPosFromEeprom (void)
{
  UINT8 mastPos[4];
  UINT8 slaveAddPlusRegBuf[3];

  slaveAddPlusRegBuf[0] = I2c.Var.eepromAddress.byte;
  slaveAddPlusRegBuf[1] = eepromFirstRegister.address.highByte;
  slaveAddPlusRegBuf[2] = eepromFirstRegister.address.lowByte;
//  slaveAddPlusRegBuf[1] = EEPROM_POS_REGISTER >> 8;
//  slaveAddPlusRegBuf[2] = EEPROM_POS_REGISTER;

  while(I2c.Var.oI2cWriteIsRunning[I2C4]);  // Wait for any I2C4 write sequence to end
  while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end

  I2c.AddDataToFifoReadQueue(I2C4, &slaveAddPlusRegBuf[0], 3, 4);

  while(I2c.Var.oI2cReadIsRunning[I2C4]); // Wait for the read sequence to end

  I2c.ReadRxFifo(I2C4, &mastPos[0], 4);

  memcpy((void *) &mastAngle.currentValue, &mastPos[0], 4);

  mastAngle.previousValue = mastAngle.currentValue;
}


//==============================================================================
// Mast manual functions
//==============================================================================
void MastManualLeft (void)
{
  DRVB_SLEEP = 1;

  Pwm.SetDutyCycle(PWM_2, 750);
  Pwm.SetDutyCycle(PWM_3, 250);
//  Pwm.SetDutyCycle(PWM_2, 600);
//  Pwm.SetDutyCycle(PWM_3, 400);

  WriteDrive(DRVB, STATUS_Mastw);
}


void MastManualRight (void)
{
  DRVB_SLEEP = 1;

  Pwm.SetDutyCycle(PWM_2, 250);
  Pwm.SetDutyCycle(PWM_3, 750);
//  Pwm.SetDutyCycle(PWM_2, 400);
//  Pwm.SetDutyCycle(PWM_3, 600);

  WriteDrive(DRVB, STATUS_Mastw);
}


void MastManualStop (void)
{
  oEnableMastStopProcedure = 1;
  LED_STATUS_TOGGLE;

  WriteDrive(DRVB, STATUS_Mastw);
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
  if (buttons.chng.byte)
  {
    // <editor-fold defaultstate="collapsed" desc="SW1 on board">
    if (buttons.chng.bits.boardSw1)
    {
      buttons.chng.bits.boardSw1 = 0;

      if (!buttons.buttons.bits.boardSw1)     // If SW1 is pressed
      {
        mastAngle.currentValue = 0;
        mastAngle.previousValue = 0;
        
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
        else if (oManualMode)
        {
          oManualMastLeft = 1;
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
        else if (oManualMode)
        {
          oManualMastRight = 1;
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

    // <editor-fold defaultstate="collapsed" desc="SW1 on steering wheel">
    if (buttons.chng.bits.steerWheelSw1)
    {
      buttons.chng.bits.steerWheelSw1 = 0;

      if (buttons.buttons.bits.steerWheelSw1)       // If left switch on steering wheel is pressed
      {
        if (buttons.buttons.bits.steerWheelSw10)    // And right switch on steering wheel is pressed
        {
          oCountTimeToChngMode = 1;                 // Start procedure to change manual mode
          Timer.EnableInterrupt(TIMER_5);
          Timer.Reset(TIMER_5);
          oTimerChngMode = 0;

          oManualMastLeft  = 0;                     // Stop moving
          oManualMastRight = 0;

          if (oManualMode)
          {
            oManualFlagChng = 1;
          }
        }
        else if (oManualMode)
        {
          oManualMastLeft = 1;
          oManualFlagChng = 1;
        }
      }
      else                          // If left switch on steering wheel is not pressed
      {
        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
        {
          oCountTimeToChngMode = 0;
          Timer.DisableInterrupt(TIMER_5);

          if (oTimerChngMode)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
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

    // <editor-fold defaultstate="collapsed" desc="SW3 on steering wheel">
    if (buttons.chng.bits.steerWheelSw3)
    {
      buttons.chng.bits.steerWheelSw3 = 0;

      if (buttons.buttons.bits.steerWheelSw3)     // If SW1 is pressed
      {
        mastAngle.currentValue = 0;
        mastAngle.previousValue = 0;

        WriteMastPos2Eeprom (); // Write zero to EEPROM

        SEND_CALIB_DONE;  // Confirm that the calib is done
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
          Timer.EnableInterrupt(TIMER_5);
          Timer.Reset(TIMER_5);
          oTimerChngMode = 0;

          oManualMastLeft  = 0;                     // Stop moving
          oManualMastRight = 0;

          if (oManualMode)
          {
            oManualFlagChng = 1;
          }
        }
        else if (oManualMode)
        {
          oManualMastRight = 1;
          oManualFlagChng = 1;
        }
      }
      else                          // If right switch on steering wheel is not pressed
      {
        if (oCountTimeToChngMode)   // And the procedure ot change mode was occuring
        {
          oCountTimeToChngMode = 0;
          Timer.DisableInterrupt(TIMER_5);

          if (oTimerChngMode)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
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
}