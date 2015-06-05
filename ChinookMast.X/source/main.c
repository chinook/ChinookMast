//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : main.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the main C file of the template project used by all
//           developpers of Chinook. It uses ChinookLib, which is another
//           repository on Github.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : For ChinookLib to be useable, it must be cloned on your hard
//           drive so the path
//               "..\..\..\ChinookLib\ChinookLib.X\headers\ChinookLib.h"
//           references an existing file.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#include "..\headers\Setup.h"
#include "..\headers\Interrupts.h"
#include "..\headers\StateMachine_Mast.h"
#include "..\headers\HardwareProfile.h"
#include "..\headers\DRV8711_Para.h"
#include "..\headers\SkadiFunctions.h"
#include "..\headers\StateFunctions.h"


//==============================================================================
// FUNCTION PROTOTYPES
//==============================================================================
void sendUART();

//==============================================================================
// VARIABLE DECLARATIONS
//==============================================================================
volatile extern INT32 Flag_Main_While;       // From Interrupt.c
extern volatile BOOL  oI2cMustWrite
                     ,oI2cMustRead
                     ,oI2c4DataRead
                     ;

extern volatile UINT8 iCurrentState;

UINT8 data = 0;

// Mast general value
volatile INT32  Mast_now = 10;                  // Actual position of Mast
volatile INT32  Mast_consigne = 10000;            // New value of Mast

INT8  memoMast_Up = 0;
INT8  memoMast_Down = 0;

volatile UINT8 i2cData[5] = {0};

//==============================================================================
// MAIN CODE
//==============================================================================
void main(void)
{

//==============================================================================
// Following memcopies are used to be able to use the form 
// Chinook.LibraryX.FunctionX in case the developper wants to see the available
// functions.
//==============================================================================
  memcpy( &Chinook.Port  , &Port  , sizeof ( struct sChinookPort  ) );
  memcpy( &Chinook.Uart  , &Uart  , sizeof ( struct sChinookUart  ) );
  memcpy( &Chinook.Pwm   , &Pwm   , sizeof ( struct sChinookPwm   ) );
  memcpy( &Chinook.Timer , &Timer , sizeof ( struct sChinookTimer ) );
  memcpy( &Chinook.Spi   , &Port  , sizeof ( struct sChinookSpi   ) );
  memcpy( &Chinook.Wdt   , &Wdt   , sizeof ( struct sChinookWdt   ) );
  memcpy( &Chinook.Adc   , &Adc   , sizeof ( struct sChinookAdc   ) );
  memcpy( &Chinook.Can   , &Can   , sizeof ( struct sChinookCan   ) );
  memcpy( &Chinook.I2c   , &I2c   , sizeof ( struct sChinookI2c   ) );
  memcpy( &Chinook.Skadi , &Skadi , sizeof ( struct sChinookSkadi ) );
  memcpy( &Chinook.InputCapture , &InputCapture , sizeof ( struct sChinookInputCapture ) );
//==============================================================================


//==============================================================================
// The next line disables the JTAG for the PIC. If the JTAG is enabled, pins
// RB10-13 can't be used as I/Os. If you want to use the JTAG, comment or
// remove this line.
//==============================================================================
  DDPCONbits.JTAGEN = 0;
//==============================================================================


//==============================================================================
// Configure the device for maximum performance but do not change the PBDIV
// Given the options, this function will change the flash wait states, RAM
// wait state and enable prefetch cache but will not change the PBDIV.
// The PBDIV value is already set via the pragma FPBDIV option in HardwareProfile.h.
//==============================================================================
  SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
//==============================================================================

// State machine init
//============================
	pStateMast = &StateInitMast;
//============================
  
  extern volatile BOOL oI2c4DataSent;

  I2c.Var.eepromAddress.rw = I2C_WRITE;
  i2cData[0] = I2c.Var.eepromAddress.byte;  // Slave address
  i2cData[1] = 0x05;      // HIGH byte of EEPROM internal memory
  i2cData[2] = 0x55;           // LOW byte of EEPROM internal memory
  i2cData[3] = 0xFF;                        // Data to write
  i2cData[4] = I2c.Var.eepromAddress.byte;  // Slave address

  StateInitMast();

  Init_reg_Mast();
  

  UINT16 i = 0;
  DRVA_STEP = 0;
  DRVA_BIN1 = 1;
  DRVA_BIN2 = 0;

//  Uart.SendDataBuffer(UART6, "==================================================\n\r", 52);

//  pBuffStateMast  = pStateMast;
//  sendUART();

  sUartLineBuffer_t buffer =
  {
     .buffer = {0}
    ,.length =  0
  };
  
  UINT16 character;
  UINT8 alloString[] = "\n\rBRAVO CA MARCHE\n\r\0";
  INT8 err;

  LED_STATUS_OFF;
  LED_ERROR_OFF;
  LED_CAN_OFF;
  LED_DEBUG4_OFF;
  LED_DEBUG3_OFF;
  LED_DEBUG2_OFF;
  LED_DEBUG1_OFF;
  LED_DEBUG0_OFF;
  
	while(1)  //infinite loop
	{
    // I2C TEST

    i = 0;
    if (!SW1)
    {
//      oI2cMustWrite = 1;
//      INTSetFlag(INT_I2C4M);
//      while(!oI2c4DataSent);
      
//      I2c.Var.eepromAddress.rw = I2C_READ;
//      i2cData[3] = I2c.Var.eepromAddress.byte;
//      i2cData[4] = 0;
//      i2cData[5] = 0;
      
//      iCurrentState = 0;
//      oI2cMustRead = 1;
//      INTSetFlag(INT_I2C4M);
//      while(!oI2c4DataRead);
//      LED_STATUS_ON;
      
//      I2c.EepromSendByte(I2C4, 0x0555, 0xFF);
//      err = I2c.EepromReadByte(I2C4, 0x0555, (UINT8 *) &data);
      
      UINT8 buffer[7];
      I2c.Var.eepromAddress.rw = I2C_WRITE;
      buffer[0] = I2c.Var.eepromAddress.byte;
      buffer[1] = 0x05;
      buffer[2] = 0x40;
      buffer[3] = 0xAA;
      
      err = I2c.AddDataToFifoWriteQueue(I2C4, &buffer[0], 4, TRUE);
      if (err < 0)
      {
        LED_STATUS_ON;
      }
      
//      Timer.DelayMs(100);
      
      while(I2c.Var.oI2cWriteIsRunning[I2C4]);
      
      I2c.Var.eepromAddress.rw = I2C_WRITE;
      buffer[0] = I2c.Var.eepromAddress.byte;
      buffer[1] = 0x05;
      buffer[2] = 0x40;
      I2c.Var.eepromAddress.rw = I2C_READ;
      buffer[3] = I2c.Var.eepromAddress.byte;
      
      err = I2c.AddDataToFifoReadQueue(I2C4, &buffer[0], 1);
      if (err < 0)
      {
        LED_DEBUG1_ON;
      }
//      while(I2c.Var.oI2cWriteIsRunning[I2C4]);
      
//      
//      Timer.DelayMs(10);
      
      while(I2c.Var.oI2cReadIsRunning[I2C4]);
      
//      while(!I2c.Var.oRxDataAvailable[I2C4]);
      
      err = I2c.ReadRxFifo(I2C4, &data, 1);
      if (err < 0)
      {
        LED_DEBUG3_ON;
      }
      
      if (data == 0xAA)
      {
        LED_CAN_ON;
      }
      else
      {
        LED_ERROR_ON;
      }
      while(1);
    }
//    if (!SW1)
//    {
//      LED_CAN_ON;
//      LED_DEBUG0_ON;
//      LED_ERROR_ON;
//      while(1);
//    }



    // UART test
//    if (Uart.Var.oIsRxDataAvailable[UART6])
//    {
//      Uart.GetRxFifoBuffer(UART6, &buffer, TRUE);
//      buffer.length = 0;
//      if (buffer.buffer[0] == 'a')
//      {
//      }
//    }
    
//    // SPI TEST
//
//    i = 0;
//    if (!SW1)
//    {
//      LED_CAN_OFF;/*1000 0001 0001 0000*/
//      WriteDrive(DRVA, 0x0003);
//      ReadDrive(DRVA, 0x8000);
//      WriteDrive(DRVA, 0x1155);
//      ReadDrive(DRVA, 0x9000);
//      WriteDrive(DRVA, 0x0003);
//      while(1);
//    }
//    else
//    {
//      LED_CAN_ON;
//    }
    
    
    // CODE À AMAURY
    
//    // flag of 500us for stepper Mast.
//    if(Flag_Main_While)
//    {
//      Flag_Main_While = 0;
//     
//      LED_STATUS = SW1;
//
//      if (!SW2 && !memoMast_Down) 
//      {
//        Mast_consigne += 1000;
//        memoMast_Down = 1;
//      }
//      else if (SW2) 
//      {
//        memoMast_Down = 0;
//      }
//      if (!SW3 && !memoMast_Up) 
//      {
//        Mast_consigne -= 1000;
//        memoMast_Up = 1;
//      }
//      else if (SW3) 
//      {
//        memoMast_Up = 0;
//      }

//==============================================================================
// Mast State machine with Drive A
//==============================================================================
//      (*pStateMast)();          // jump to next state
//       StateSchedulerMast();   // Decides which state will be next
//
//      if(pBuffStateMast != pStateMast)
//      {
//        pBuffStateMast = pStateMast;
//        sendUART();
//      }

//    }
	}
} //END MAIN CODE

void sendUART()
{
  char  Text[100]={0};
  int i=0;
  if(pStateMast==&StateInitMast) Uart.SendDataBuffer(UART6, "State Mast : Init\t\t", 19);
  else if(pStateMast==&StateCalibMast) Uart.SendDataBuffer(UART6, "State Mast : Calib\t\t", 20);
  else if(pStateMast==&StateStopMast) Uart.SendDataBuffer(UART6, "State Mast : Stop\t\t", 19);
  else if(pStateMast==&StateDownMast) Uart.SendDataBuffer(UART6, "State Mast : Down\t\t", 19);
  else if(pStateMast==&StateUpMast) Uart.SendDataBuffer(UART6, "State Mast : Up  \t\t", 19);

  sprintf(Text, "Mast Now = %d\t\tMast Consigne = %d\n\r", Mast_now, Mast_consigne);
  Uart.SendDataBuffer(UART6, Text, sizeof(Text));
  for(i=0; i<100; i++) Text[i]=0;
}