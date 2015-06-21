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


//==============================================================================
// Variable declarations
//==============================================================================
const UINT16 EEPROM_POS_REGISTER = 0x0100;

volatile sButtonStates_t buttons =
{
  .chng   .byte = 0
 ,.buttons.byte = 0
          
 ,.buttons.bits.boardSw1 = 1
 ,.buttons.bits.boardSw2 = 1
 ,.buttons.bits.boardSw3 = 1
};


extern volatile INT8  mastCurrentPos        // Actual position of Mast
                     ,Mast_consigne         // New value of Mast
                     ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oTimer1
                    ,oTimer5
                    ,oManualMode
                    ,oCountTimeToChngMode
                    ,oManualFlagChng
                    ,oManualMastRight
                    ,oManualMastLeft
                    ;

extern volatile UINT32 nTurns;

//==============================================================================
// State Machine private functions prototypes
//==============================================================================


//==============================================================================
// EEPROM functions
//==============================================================================
void WriteMastPos2Eeprom (UINT8 pos)
{
   UINT8 dataBuffer[4];
   dataBuffer[0] = I2c.Var.eepromAddress.byte;
//   dataBuffer[1] = EEPROM_POS_REGISTER >> 8;
//   dataBuffer[2] = EEPROM_POS_REGISTER;
   dataBuffer[1] = 0x01;
   dataBuffer[2] = 0x00;
   dataBuffer[3] = pos;

   while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end
   while(I2c.Var.oI2cWriteIsRunning[I2C4]); // Wait for any I2C4 write sequence to end

   I2c.AddDataToFifoWriteQueue(I2C4, &dataBuffer[0], 4, TRUE);
}


UINT8 ReadMastPosFromEeprom (void)
{
  UINT8 mastPos;
  UINT8 slaveAddPlusRegBuf[3];

  slaveAddPlusRegBuf[0] = I2c.Var.eepromAddress.byte;
//  slaveAddPlusRegBuf[1] = EEPROM_POS_REGISTER >> 8;
//  slaveAddPlusRegBuf[2] = EEPROM_POS_REGISTER;
  slaveAddPlusRegBuf[1] = 0x01;
  slaveAddPlusRegBuf[2] = 0x00;

  while(I2c.Var.oI2cWriteIsRunning[I2C4]);  // Wait for any I2C4 write sequence to end
   while(I2c.Var.oI2cReadIsRunning[I2C4]);  // Wait for any I2C4 read sequence to end

  I2c.AddDataToFifoReadQueue(I2C4, &slaveAddPlusRegBuf[0], 3, 1);

  while(I2c.Var.oI2cReadIsRunning[I2C4]); // Wait for the read sequence to end

  I2c.ReadRxFifo(I2C4, &mastPos, 1);
  
  return mastPos;
}


//==============================================================================
// Mast manual functions
//==============================================================================
void MastManualLeft (void)
{
  DRVB_SLEEP = 1;

  Pwm.SetDutyCycle(PWM_2, 750);
  Pwm.SetDutyCycle(PWM_3, 250);

//  mastCurrentPos--;

  WriteDrive(DRVB, STATUS_Mastw);
}


void MastManualRight (void)
{
  DRVB_SLEEP = 1;

  Pwm.SetDutyCycle(PWM_2, 250);
  Pwm.SetDutyCycle(PWM_3, 750);

//  mastCurrentPos++;

  WriteDrive(DRVB, STATUS_Mastw);
}


void MastManualStop (void)
{
  DRVB_SLEEP = 0;

  Pwm.SetDutyCycle(PWM_2, 500);
  Pwm.SetDutyCycle(PWM_3, 500);

  WriteMastPos2Eeprom(mastCurrentPos);
}


//==============================================================================
// Buttons functions
//==============================================================================
void AssessButtons (void)
{
  if (buttons.chng.byte)
  {

    if (buttons.chng.bits.boardSw1)
    {
      buttons.chng.bits.boardSw1 = 0;

      if (!buttons.buttons.bits.boardSw1)     // If SW1 is pressed
      {
        mastCurrentPos = 0;
        WriteMastPos2Eeprom (mastCurrentPos); // Write zero to EEPROM
      }
    }


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
          oTimer5 = 0;

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

          if (oTimer5)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
          }
        }
        else if (oManualMode)
        {
          oManualMastLeft = 0;
          oManualFlagChng = 1;
        }
      }
    }


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
          oTimer5 = 0;

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

          if (oTimer5)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
          }
        }
        else if (oManualMode)
        {
          oManualMastRight = 0;
          oManualFlagChng = 1;
        }
      }
    }


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
          oTimer5 = 0;

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

          if (oTimer5)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
          }
        }
        else if (oManualMode)
        {
          oManualMastLeft = 0;
          oManualFlagChng = 1;
        }
      }
    }


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
          oTimer5 = 0;

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

          if (oTimer5)              // If at least one second has passed
          {
            oManualMode ^= 1;       // Change mode
          }
        }
        else if (oManualMode)
        {
          oManualMastRight = 0;
          oManualFlagChng = 1;
        }
      }
    }
  }
}