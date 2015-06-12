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
#include "..\headers\StateMachine.h"
#include "..\headers\HardwareProfile.h"
#include "..\headers\DRV8711_Para.h"
#include "..\headers\SkadiFunctions.h"
//#include "..\headers\StateFunctions.h"


//==============================================================================
// FUNCTION PROTOTYPES
//==============================================================================
void sendUART();

//==============================================================================
// VARIABLE DECLARATIONS
//==============================================================================
extern volatile BOOL oFlagMainWhile;       // From Interrupt.c

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

//  StateInit();

  pBuffStateMast  = pStateMast;
  
  oFlagMainWhile = 1;
  
  
	while(1)  //infinite loop
	{
    
    // flag of 500us for stepper Mast.
    if(oFlagMainWhile)
    {
      oFlagMainWhile = 0;

      //======================================
      // Mast State machine with Drive A
      //======================================
      (*pStateMast)();          // jump to next state
//      WriteDrive(DRVA, STATUS_Mastw);
      StateAcquisition();
      StateScheduler();   // Decides which state will be next

//      if(pBuffStateMast != pStateMast)
//      {
//        pBuffStateMast = pStateMast;
////        sendUART();
//      }
    } // end if oFlagMainWhile
	}  // end while
}   //END MAIN CODE

void sendUART()
{
  char  Text[100]={0};
  int i=0;
  if(pStateMast==&StateInit) Uart.SendDataBuffer(UART6, "State Mast : Init\t\t", 19);
  else if(pStateMast==&StateCalib) Uart.SendDataBuffer(UART6, "State Mast : Calib\t\t", 20);
  else if(pStateMast==&StateStop) Uart.SendDataBuffer(UART6, "State Mast : Stop\t\t", 19);
  else if(pStateMast==&StateDown) Uart.SendDataBuffer(UART6, "State Mast : Down\t\t", 19);
  else if(pStateMast==&StateUp) Uart.SendDataBuffer(UART6, "State Mast : Up  \t\t", 19);

//  sprintf(Text, "Mast Now = %d\t\tMast Consigne = %d\n\r", Mast_now, Mast_consigne);
  Uart.SendDataBuffer(UART6, Text, sizeof(Text));
  for(i=0; i<100; i++) Text[i]=0;
}