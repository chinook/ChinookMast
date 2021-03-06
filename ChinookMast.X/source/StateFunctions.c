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

#include "StateFunctions.h"
#include "CommandFunctions.h"
#include "Potentiometer.h"

// Private functions prototypes
void SetZeroFromSteeringWheel (void);
void SetZeroFSWheelWAngleRelative (void);


//==============================================================================
// Variable declarations
//==============================================================================

// EEPROM register containing mast last orientation
I2cEepromInternalRegister_t eepromFirstRegister =
{
  .index.pageIndex = 0b00010000
 ,.index.byteIndex = 0b000000
};

// All the buttons used. 3 on the steering wheel, 3 on the board
volatile sButtonStates_t buttons =
{
  .chng   .byte = 0
 ,.buttons.byte = 0

 // switches on board are at 1 when not pressed
 ,.buttons.bits.boardSw1 = 1
 ,.buttons.bits.boardSw2 = 1
 ,.buttons.bits.boardSw3 = 1
};

extern volatile float  mastCurrentSpeed       // Actual speed of Mast
                      ;

volatile float mastDirMan;

extern sPotValues_t potValues;
sSteeringPotStates_t steeringpotentiometer;

extern volatile sCmdValue_t mastAngle;        // Discrete position of mast

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oEnableMastStopProcedure // Stop procedure using TIMER 2
                    ,oTimerReg                // From TIMER 1
                    ,oTimerChngMode           // Flag used when changing mode
                    ,oManualMode
                    ,oCountTimeToChngMode     // Flag used when changing mode
                    ,oManualFlagChng          // In manual mode, indicates that a change has occured on the buttons
                    ,oManualMastRight
                    ,oManualMastLeft
                    ,oTimerSetZero
                    ,oSetZeroCounterOccured
                    ,oMastMinBlock
                    ,oMastMaxBlock
                    ;

extern volatile UINT16 setZeroCounter;

extern sUartLineBuffer_t buffer;

//==============================================================================
// Mast manual functions
//==============================================================================
void MastManualLeft (void)
{
  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    DRVB_SLEEP = 1;

    Pwm.SetDutyCycle(PWM_2, 585);
    Pwm.SetDutyCycle(PWM_3, 415);

    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    DRVA_SLEEP = 1;

    Pwm.SetDutyCycle(PWM_4, 585);
    Pwm.SetDutyCycle(PWM_5, 415);

    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}


void MastManualRight (void)
{
  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    DRVB_SLEEP = 1;

    Pwm.SetDutyCycle(PWM_2, 415);
    Pwm.SetDutyCycle(PWM_3, 585);

    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    DRVA_SLEEP = 1;

    Pwm.SetDutyCycle(PWM_4, 415);
    Pwm.SetDutyCycle(PWM_5, 585);

    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}

void MastStop (void)
{
  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    DRVB_SLEEP = 0;

    Pwm.SetDutyCycle(PWM_2, 500);
    Pwm.SetDutyCycle(PWM_3, 500);
    
    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    DRVA_SLEEP = 0;

    Pwm.SetDutyCycle(PWM_4, 500);
    Pwm.SetDutyCycle(PWM_5, 500);

//    DRVA_SLEEP = 1;
    
    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}

void MastManualStop (void)
{
  mastDirMan = SignFloat(mastCurrentSpeed);
  buffer.length = sprintf(buffer.buffer, "mastDirMan = %f \r\n\n", mastDirMan);
  Uart.PutTxFifoBuffer(UART6, &buffer);
  if(mastDirMan == MAST_DIR_LEFT)
  {
    buffer.length = sprintf(buffer.buffer, "Mast Left \r\n\n");
    Uart.PutTxFifoBuffer(UART6, &buffer);
    
    if(!MAST_MIN_OK && !oMastMinBlock)    
    {
      buffer.length = sprintf(buffer.buffer, "Man:Set Min Block \r\n\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
      oMastMinBlock = 1;  //Block further mast movement involving the stopping sequence
      oEnableMastStopProcedure = 1;  // Start stop procedure using TIMER 2
    }
    else if(!MAST_MIN_OK && oMastMinBlock)
    {
      //Do nothing, Mast has reached Min limit and is already blocked
    }
    else    //Mast MIN limit hasn't been reached
    {
      oEnableMastStopProcedure = 1;
    }
  }
  else if(mastDirMan == MAST_DIR_RIGHT)
  {
    buffer.length = sprintf(buffer.buffer, "Mast Right \r\n\n");
    Uart.PutTxFifoBuffer(UART6, &buffer);
    
    if(!MAST_MAX_OK && !oMastMaxBlock)
    {
      buffer.length = sprintf(buffer.buffer, "Man:Set Max Block \r\n\n");
      Uart.PutTxFifoBuffer(UART6, &buffer);
      oMastMaxBlock = 1;
      oEnableMastStopProcedure = 1;  // Start stop procedure using TIMER 2
    }
    else if(!MAST_MAX_OK && oMastMaxBlock)
    {
      //Do nothing
    }
    else
    {
      oEnableMastStopProcedure = 1;
    }
  }
  
  mastDirMan = 0;

  // DRIVE B
  //==========================================================
  if (USE_DRIVE_B == 1)
  {
    WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================


  // DRIVE A
  //==========================================================
  if (USE_DRIVE_A == 1)
  {
    WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors at the drive
  }
  //==========================================================
}


//==============================================================================
// Buttons functions
//==============================================================================
void AssessButtons (void)   // Since we dont decode steering messages containing all the button states anymore, the following code should be cleaned
{
  sUartLineBuffer_t buffer;
  
  //
  // Switches detection
  //
  
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
  if (buttons.chng.byte)  // If any change has occured on any button
  {
    // <editor-fold defaultstate="collapsed" desc="SW1 on board">
    if (buttons.chng.bits.boardSw1)
    {
      buttons.chng.bits.boardSw1 = 0;

      if (!buttons.buttons.bits.boardSw1)     // If SW1 is pressed
      {
        mastAngle.currentValue = 0;
        mastAngle.previousValue = 0;
#ifdef USE_POTENTIOMETER
        potValues.zeroInBits = potValues.lastAverage;
        potValues.potStepValue = POT_TO_MOTOR_RATIO >> 1;
#endif

        //SEND_CALIB_DONE;  // Confirm that the calib is done
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
        else if (oManualMode && !oEnableMastStopProcedure)
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
            if (mastCurrentSpeed != 0)
            {
              MastManualStop();
            }
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
        else if (oManualMode && !oEnableMastStopProcedure)
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
            if (mastCurrentSpeed != 0)
            {
              MastManualStop();
            }
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
  
  // TODO : re-evaluate if code is really necessary ??
  
// <editor-fold defaultstate="collapsed" desc="Potentiometer of steering wheel">
  /*
  if (steeringpotentiometer.chng)
  {
    oManualFlagChng = 1;
    steeringpotentiometer.chng = 0;

    if (oManualMode //&& !oEnableMastStopProcedure//)
    {
      if ( (steeringpotentiometer.value < MAST_L_POT_MIN) && (steeringpotentiometer.value > MAST_L_POT_MAX) )
      {
//        oManualMastRight = 1;
          oManualMastLeft = 1;
//        oManualFlagChng = 1;
//        buffer.length = sprintf(buffer.buffer, "Manual R\r\n\n");
//        Uart.PutTxFifoBuffer(UART6, &buffer);
      }
      else if ( (steeringpotentiometer.value > MAST_R_POT_MIN) && (steeringpotentiometer.value < MAST_R_POT_MAX) )
      {
        oManualMastRight = 1;
//        oManualMastLeft = 1;
//        oManualFlagChng = 1;
//        buffer.length = sprintf(buffer.buffer, "Manual L\r\n\n");
//        Uart.PutTxFifoBuffer(UART6, &buffer);
      }
      else
      {
//        if ( oManualMastLeft || oManualMastRight)
//        {
//          oEnableMastStopProcedure = 1;
//          MastStop(); // Mast action is only made in StateManual
        //oManualMastLeft = 0;
        //oManualMastRight = 0;
//        buffer.length = sprintf(buffer.buffer, "oManualMast dropped\r\n\n");
//        Uart.PutTxFifoBuffer(UART6, &buffer);
//        }
      }
    }
  }
    */
    // </editor-fold>
  // </editor-fold>
  
  if (oTimerSetZero && oSetZeroCounterOccured)
  {
    oTimerSetZero = 0;
    setZeroCounter = 0;
    oSetZeroCounterOccured = 0;
  }
}

//==============================================================================
// Math functions
//==============================================================================

INT8 SignFloat (float value)
{
  return (value >= 0) ? 1 : -1;
}

INT8 SignInt (INT32 value)
{
  return (value >= 0) ? 1 : -1;
}

float AbsFloat (float value)
{
  return (value >= 0) ? value : -value;
}

INT32 AbsInt (INT32 value)
{
  return (value >= 0) ? value : -value;
}

float MaxFloat (float v1, float v2)
{
  return (v1 >= v2) ? v1 : v2;
}

INT32 MaxInt (INT32 v1, INT32 v2)
{
  return (v1 >= v2) ? v1 : v2;
}

float MinFloat (float v1, float v2)
{
  return (v1 <= v2) ? v1 : v2;
}

INT32 MinInt (INT32 v1, INT32 v2)
{
  return (v1 <= v2) ? v1 : v2;
}