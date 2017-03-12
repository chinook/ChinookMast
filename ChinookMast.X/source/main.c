//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Mast
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : main.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the main C file of the mast project.
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
#include "..\headers\Potentiometer.h"
#include "..\headers\Interrupts.h"
#include "..\headers\StateMachine.h"
#include "..\headers\HardwareProfile.h"
#include "..\headers\DRV8711_Para.h"
#include "..\headers\SkadiFunctions.h"
#include "..\headers\CommandFunctions.h"
#include "..\headers\StateFunctions.h"

void MastStartPwm (UINT16 pwm1, UINT16 pwm2);


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
	pStateMast = &StateInit;
//============================
  
  StateInit();
  
  sUartLineBuffer_t  rxBuf = {0}
                    ,txBuf = {0}
                    ;
  
  float timeStamp = 0;
  
  extern volatile BOOL oTimerReg;
  extern volatile sCmdValue_t mastSpeed;
  extern volatile float mastCurrentSpeed;
  
#define TIMESTAMP_VALUE 0.1f
  
  LED_ALL_OFF();
  
	while(1)  //infinite loop
	{
    while(rxBuf.buffer[0] == 0)
    {
      while(rxBuf.length == 0)
      {
        Uart.GetRxFifoBuffer(UART6, &rxBuf, FALSE);
        AssessMastValues();
        if (oTimerReg)
        {
          oTimerReg = 0;
          // Update mast speed
          mastSpeed.previousValue = mastSpeed.currentValue;
          mastSpeed.currentValue  = mastCurrentSpeed;
        }
      }
      if (rxBuf.buffer[0] != 'Y')
      {
        rxBuf.buffer[0] = 0;
        rxBuf.length = 0;
      }
    }
    rxBuf.buffer[0] = 0;
    rxBuf.length = 0;
    LED_DEBUG2_ON;
    
    MastStartPwm(590, 410);   // PWM to adjust in this function.
    
    while(rxBuf.buffer[0] == 0)
    {
      while(rxBuf.length == 0)
      {
        Uart.GetRxFifoBuffer(UART6, &rxBuf, FALSE);
        AssessMastValues();
        if (oTimerReg)
        {
          oTimerReg = 0;
          // Update mast speed
          mastSpeed.previousValue = mastSpeed.currentValue;
          mastSpeed.currentValue  = mastCurrentSpeed;
          
          timeStamp += TIMESTAMP_VALUE;
          memcpy(&txBuf.buffer[0], &timeStamp                       , 4);
          memcpy(&txBuf.buffer[4], (void *) &mastSpeed.currentValue , 4);
          txBuf.length = 8;
          Uart.PutTxFifoBuffer(UART6, &txBuf);
        }
      }
      if (rxBuf.buffer[0] != 'N')
      {
        rxBuf.buffer[0] = 0;
        rxBuf.length = 0;
      }
    }
    
    rxBuf.buffer[0] = 0;
    rxBuf.length = 0;
    timeStamp = 0;
    MastManualStop();
    LED_DEBUG2_OFF;

    //======================================
    // Mast State machine with Drive A
    //======================================
//    (*pStateMast)();    // jump to next state
//    StateScheduler();   // Decides which state will be next

	}  // end while
} // end main


void MastStartPwm (UINT16 pwm1, UINT16 pwm2)
{
  
  if ((pwm1 + pwm2) != 1000)
  {
    return;
  }
  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    DRVB_SLEEP = 1;

    Pwm.SetDutyCycle(PWM_2, pwm1);
    Pwm.SetDutyCycle(PWM_3, pwm2);

    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    DRVA_SLEEP = 1;

    Pwm.SetDutyCycle(PWM_4, pwm1);
    Pwm.SetDutyCycle(PWM_5, pwm2);

    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}