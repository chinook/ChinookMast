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
extern volatile sCmdValue_t mastAngle;
extern volatile float mastCurrentSpeed;
extern volatile UINT32 rxWindAngle;
extern volatile BOOL oManualMode;

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
    buffer.length = sprintf(buffer.buffer, "Cette led n'existe pas!\r\n");
    Uart.PutTxFifoBuffer(UART6, &buffer);
  }
}


/**************************************************************
 * Function name  : SetMode
 * Purpose        : Set the mast mode of operation
 * Arguments      : 1 : Manual mode
 *                  0 : Auto mode
 * Returns        : None.
 *************************************************************/
void SetMode(sSkadi_t *skadi, sSkadiArgs_t args)
{
  sUartLineBuffer_t buffer;

  UINT8 mode = atoi(args.elements[0]);   // Convert argument to int

  if (mode == 0)    // Auto mode
  {
    oManualMode = 0;
    SEND_MODE_TO_STEERING_WHEEL;
  }
  else if (mode == 1)    // Manual mode
  {
    oManualMode = 1;
    SEND_MODE_TO_STEERING_WHEEL;
  }
  else
  {
    buffer.length = sprintf(buffer.buffer, "Mauvais argument!\r\n");
    Uart.PutTxFifoBuffer(UART6, &buffer);
  }
}


/**************************************************************
 * Function name  : SetPos
 * Purpose        : Set the mast angle
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void SetPos(sSkadi_t *skadi, sSkadiArgs_t args)
{
  sUartLineBuffer_t buffer;

  float mast = atoi(args.elements[0]);   // Convert argument to int

  if ((mast >= -179) && (mast <= 179))
  {
    mastAngle.currentValue  = mast;
    mastAngle.previousValue = mast;
    mastCurrentSpeed = 0;
    if (mast == 0)
    {
      SEND_CALIB_DONE;
    }
  }
  else
  {
    buffer.length = sprintf(buffer.buffer, "Mauvais argument!\r\n");
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

  float wind = atoi(args.elements[0]);   // Convert argument to int

  if ((wind >= -179) && (wind <= 179))
  {
    memcpy((void *) &rxWindAngle, (void *) &wind, 4);
  }
  else
  {
    buffer.length = sprintf(buffer.buffer, "Mauvais argument!\r\n");
    Uart.PutTxFifoBuffer(UART6, &buffer);
  }
}


/**************************************************************
 * Function name  : LedError
 * Purpose        : Toggle LedError
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void LedError(sSkadi_t *skadi, sSkadiArgs_t args)
{
  LED_ERROR_TOGGLE;
}


/**************************************************************
 * Function name  : LedStatus
 * Purpose        : Toggle LedStatus
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void LedStatus(sSkadi_t *skadi, sSkadiArgs_t args)
{
  LED_STATUS_TOGGLE;
}


/**************************************************************
 * Function name  : LedCan
 * Purpose        : Toggle LedCan
 * Arguments      : Received from Skadi functions
 * Returns        : None.
 *************************************************************/
void LedCan(sSkadi_t *skadi, sSkadiArgs_t args)
{
  LED_CAN_TOGGLE;
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
  pStateMast = &StateInit;
}