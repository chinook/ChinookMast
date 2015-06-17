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


//==============================================================================
// State Machine private functions prototypes
//==============================================================================


//==============================================================================
// State Machine functions
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
