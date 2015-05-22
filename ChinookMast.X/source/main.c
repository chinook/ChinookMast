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

// Mast general value
volatile INT32  Mast_now = 10;                  // Actual position of Mast
volatile INT32  Mast_consigne = 10000;            // New value of Mast

INT8  memoMast_Up = 0;
INT8  memoMast_Down = 0;

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
  memcpy( &Chinook.Skadi , &I2c   , sizeof ( struct sChinookSkadi ) );
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
  StateInitMast();

  Init_reg_Mast();
  

  UINT32 i = 0;
  DRVA_STEP = 0;
  DRVA_BIN1 = 1;
  DRVA_BIN2 = 0;

  Uart.SendDataBuffer(UART6, "==================================================\n\r", 52);

  pBuffStateMast  = pStateMast;
  sendUART();

	while(1)  //infinite loop
	{
    // flag of 500us for stepper Mast.
    if(Flag_Main_While)
    {
      Flag_Main_While = 0;
     
      LED_STATUS = SW1;

      if(!SW2 && !memoMast_Down) {Mast_consigne += 1000;memoMast_Down=1;}
      else if(SW2) memoMast_Down=0;
      if(!SW3 && !memoMast_Up) {Mast_consigne -= 1000;memoMast_Up=1;}
      else if(SW3) memoMast_Up=0;

//==============================================================================
// Mast State machine with Drive A
//==============================================================================
		(*pStateMast)();          // jump to next state
     StateSchedulerMast();   // Decides which state will be next

    if(pBuffStateMast != pStateMast){
      pBuffStateMast  = pStateMast;
      sendUART();
    }

    }
	}
} //END MAIN CODE

void sendUART(){
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