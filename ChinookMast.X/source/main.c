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


#include "Setup.h"
#include "Potentiometer.h"
#include "Interrupts.h"
#include "StateMachine.h"
#include "HardwareProfile.h"
#include "DRV8711_Para.h"
#include "SkadiFunctions.h"
#include "CommandFunctions.h"
#include "StateFunctions.h"

volatile BOOL  oManualMode;

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
    
    SEND_MODE_TO_STEERING_WHEEL;
  

	while(1)  //infinite loop
	{

    //======================================
    // Mast State machine with Drive A
    //======================================
    (*pStateMast)();    // jump to next state
    StateScheduler();   // Decides which state will be next

	}  // end while
} // end main