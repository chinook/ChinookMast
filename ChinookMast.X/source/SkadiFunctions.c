//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : SourceTemplate.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This C file gathers the functions used for Skadi.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : Function names can and should be renamed by the user to improve the
//           readability of the code. User must set the functions here and adjust
//           the structure skadiCommandTable located in Setup.c.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\SkadiFunctions.h"
#include "..\headers\CommandFunctions.h"


//==============================================================================
// Private functions prototypes
//==============================================================================


//==============================================================================
// Variable definitions
//==============================================================================
extern volatile sCmdValue_t windAngle;
extern volatile UINT32 rxWindAngle;

//==============================================================================
// Functions
//==============================================================================

/**************************************************************
 * Function name  : LedDebug
 * Purpose        : Toggle LedDebug depending on the arguments
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void LedDebug(sSkadi_t *skadi, sSkadiArgs_t args)
{
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, "Cette led n'existe pas!\r\n");
//  UINT8 errorMsg[] = {"Cette led n'existe pas!\r\n"};

  int led = atoi(args.elements[0]);   // Convert argument to int

  if (led == 0)
  {
    LED_DEBUG0_TOGGLE;
  }
  else if (led == 1)
  {
    LED_DEBUG1_TOGGLE;
  }
  else if (led == 2)
  {
    LED_DEBUG2_TOGGLE;
  }
  else if (led == 3)
  {
    LED_DEBUG3_TOGGLE;
  }
  else if (led == 4)
  {
    LED_DEBUG4_TOGGLE;
  }
  else
  {
//    Uart.SendDataBuffer(UART6, errorMsg, sizeof(errorMsg));
    Uart.PutTxFifoBuffer(UART6, &buffer);
  }
}


/**************************************************************
 * Function name  : SetWind
 * Purpose        : Set the wind angle
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void SetWind(sSkadi_t *skadi, sSkadiArgs_t args)
{
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, "Mauvais argument!\r\n");

  float wind = atoi(args.elements[0]);   // Convert argument to int

  if ((wind >= -179) && (wind <= 179))
  {
    memcpy((void *) &rxWindAngle, (void *) &wind, 4);
  }
  else
  {
//    Uart.SendDataBuffer(UART6, errorMsg, sizeof(errorMsg));
    Uart.PutTxFifoBuffer(UART6, &buffer);
  }
}


/**************************************************************
 * Function name  : LedCan
 * Purpose        : Toggle LedCan depending on the arguments
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void LedCan(sSkadi_t *skadi, sSkadiArgs_t args)
{
//  UINT8 errorMsg[] = {"Cette led n'existe pas!\r\n"};
  sUartLineBuffer_t buffer;
  buffer.length = sprintf(buffer.buffer, "Cette led n'existe pas!\r\n");

  int led = atoi(args.elements[0]);   // Convert argument to int

  if (led == 2)
  {
    LED_CAN_TOGGLE;
  }
  else
  {
//    Uart.SendDataBuffer(UART6, errorMsg, sizeof(errorMsg));
    Uart.PutTxFifoBuffer(UART6, &buffer);
  }
}


/**************************************************************
 * Function name  : ReInitSystem
 * Purpose        : Redo StateInit()
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void ReInitSystem(sSkadi_t *skadi, sSkadiArgs_t args)
{
  StateInit();
}