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

#include "..\headers\CommandFunctions.h"


//==============================================================================
// Mast regulation variables
//==============================================================================
extern volatile UINT32 rxWindAngle;

UINT16 nTurns = 0;

sCmdData_t data = {0};

//volatile sCmdValue_t windAngle          = {.currentValue = 60, .previousValue = 60}
volatile sCmdValue_t windAngle          = {0}
                    ,mastAngle          = {0}
                    ,mastSpeed          = {0}
                    ,mastSpeedRealTime  = {0}   // Used as fast as possible
                    ,mastPosRealTime    = {0}   // Used as fast as possible
                    ;

sCmdValue_t  inPi   = {0}
            ,outPi  = {0}
            ;

//sInpCapValues_t inpCapTimes = {0};

// Mast general value
extern volatile float  mastCurrentSpeed      // Actual speed of Mast
                      ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oTimerReg
                    ;

BOOL  oFirstTimeInMastStop    = 0
     ;

//==============================================================================
// Mast regulation private functions prototypes
//==============================================================================


//==============================================================================
// Mast regulation functions
//==============================================================================

void TustinZ (sCmdValue_t *input, sCmdValue_t *output)
{
  output->previousValue = output->currentValue;
  output->currentValue  = output->previousValue + T/2 * (input->currentValue + input->previousValue);
}

void SetPwm (float cmd)
{
  if (cmd == 0)
  {
    if (oFirstTimeInMastStop)
    {
      MastManualStop();
      oFirstTimeInMastStop = 0;
      inPi.currentValue = 0;
      inPi.previousValue = 0;
      outPi.currentValue = 0;
      outPi.previousValue = 0;

      if (PRINT_DATA)
      {
        UINT8 i;
        INT32 err = 0;
        sUartLineBuffer_t buffer;
        buffer.length = sprintf(buffer.buffer, "\n\ri\tpSeed\tSpeed\tpPos\tPos\tpWind\tWind\tError\tpInPi\tinPi\tpOutPi\tOutPi\tcmd\n\r");
        Uart.PutTxFifoBuffer(UART6, &buffer);
        for (i = 0; i < data.length; i++)
        {
          if (err < 0)
          {
            i--;
          }
          buffer.length = sprintf(buffer.buffer, "%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n\r", i, data.speedPrevious[i], data.speedCurrent[i], data.posPrevious[i]
                  ,data.posCurrent[i],data.windPrevious[i], data.windCurrent[i], data.error[i], data.inPiPrevious[i], data.inPiCurrent[i], data.outPiPrevious[i]
                  ,data.outPiCurrent[i], data.cmd[i]);
          err = Uart.PutTxFifoBuffer(UART6, &buffer);
        }
//        UINT8 buffer[130];
//        UINT8 n;
//        n = sprintf(buffer, "\n\ri\tpSeed\tSpeed\tpPos\tPos\tpWind\tWind\tError\tpInPi\tinPi\tpOutPi\tOutPi\tcmd\n\r");
//        Uart.SendDataBuffer(UART6, buffer, n);
//        for (i = 0; i < data.length; i++)
//        {
//          n = sprintf(buffer, "%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n\r", i, data.speedPrevious[i], data.speedCurrent[i], data.posPrevious[i]
//                  ,data.posCurrent[i],data.windPrevious[i], data.windCurrent[i], data.error[i], data.inPiPrevious[i], data.inPiCurrent[i], data.outPiPrevious[i]
//                  ,data.outPiCurrent[i], data.cmd[i]);
//          Uart.SendDataBuffer(UART6, buffer, n);
//        }
        data.length = 0;
      }
    }
  }
  else
  {
    oFirstTimeInMastStop = 1;
    
    UINT16 pwm = ABS(cmd) * 500;

    if (SIGN(cmd) == MAST_DIR_LEFT)
    {
      DRVB_SLEEP = 1;
      Pwm.SetDutyCycle(PWM_2, 500 + pwm);
      Pwm.SetDutyCycle(PWM_3, 500 - pwm);
    }
    else if (SIGN(cmd) == MAST_DIR_RIGHT)
    {
      DRVB_SLEEP = 1;
      Pwm.SetDutyCycle(PWM_2, 500 - pwm);
      Pwm.SetDutyCycle(PWM_3, 500 + pwm);
    }
  }
}

void Regulator (void)
{
  float  cmd
        ,error
        ,tempWind
        ;

  // Update wind angle
  windAngle.previousValue = windAngle.currentValue;

  memcpy ((void *) &tempWind, (void *) &rxWindAngle, 4);

  if (tempWind > MAST_MAX)
  {
    windAngle.currentValue = MAST_MAX;
  }
  else if (tempWind < MAST_MIN)
  {
    windAngle.currentValue = MAST_MIN;
  }
  else if (tempWind != windAngle.currentValue)
  {
    windAngle.currentValue = tempWind;
  }

  // Update mast speed
  mastSpeed.previousValue = mastSpeed.currentValue;
  mastSpeed.currentValue  = mastCurrentSpeed;

  // Get mast position from mast speed
  TustinZ((void *) &mastSpeed, (void *) &mastAngle);

  if (mastAngle.currentValue > 180)
  {
    mastAngle.currentValue -= 360;
  }
  else if (mastAngle.currentValue < -180)
  {
    mastAngle.currentValue += 360;
  }

  error = windAngle.currentValue - mastAngle.currentValue;

  if (ABS(error) <= ERROR_THRESHOLD)  // Don't need to move the mast
  {
    cmd = 0;
  }
  else if ( (SIGN(mastSpeed.currentValue) == MAST_DIR_LEFT) && (!MAST_MIN_OK) )   // Mast too far
  {
    cmd = 0;
  }
  else if ( (SIGN(mastSpeed.currentValue) == MAST_DIR_RIGHT) && (!MAST_MAX_OK) && (mastSpeed.currentValue != 0) )  // Mast too far
  {
    cmd = 0;
  }
  else
  {
    oFirstTimeInMastStop = 1;

    inPi.previousValue = inPi.currentValue;
    inPi.currentValue  = K * error - mastSpeed.currentValue;

    TustinZ((void *) &inPi, (void *) &outPi);

    cmd = inPi.currentValue * KP + outPi.currentValue * KI;

    if (ABS(cmd) > PWM_MAX_DUTY_CYCLE)
    {
      cmd = SIGN(cmd) * PWM_MAX_DUTY_CYCLE;
    }
    else if (ABS(cmd) < PWM_MIN_DUTY_CYCLE)
    {
      cmd = SIGN(cmd) * PWM_MIN_DUTY_CYCLE;
    }
  }

  if (PRINT_DATA && oFirstTimeInMastStop)
//  if (PRINT_DATA)
  {
    if (data.length < N_DATA_TO_ACQ)
    {
      data.cmd            [data.length] = cmd;
      data.error          [data.length] = error;
      data.inPiCurrent    [data.length] = inPi.currentValue;
      data.inPiPrevious   [data.length] = inPi.previousValue;
      data.outPiCurrent   [data.length] = outPi.currentValue;
      data.outPiPrevious  [data.length] = outPi.previousValue;
      data.posCurrent     [data.length] = mastAngle.currentValue;
      data.posPrevious    [data.length] = mastAngle.previousValue;
      data.speedCurrent   [data.length] = mastSpeed.currentValue;
      data.speedPrevious  [data.length] = mastSpeed.previousValue;
      data.windCurrent    [data.length] = windAngle.currentValue;
      data.windPrevious   [data.length] = windAngle.previousValue;
      data.length++;
    }
  }

  SetPwm(cmd);
}


//==============================================================================
// Input Capture functions
//==============================================================================
void AssessMastValues (void)
{
  INT64 rx2, rx4;
  INT8 firstIc;
  UINT64 meanTime   = 0;
  INT8 dir    = 0;
  
  if (oCapture2 && oCapture4)
  {
    oCapture2 = 0;
    oCapture4 = 0;

    rx2 = InputCapture.GetTimeBetweenCaptures(IC2, SCALE_US);
    rx4 = InputCapture.GetTimeBetweenCaptures(IC4, SCALE_US);

    if ( !((rx2 > 2000000) || (rx4 > 2000000)) )  // It would mean 0.34 deg/s for the motor shaft, consider it zero
    {
//      if (ABS(100 - rx2*100/rx4) < 30)
//      {
        firstIc = InputCapture.GetDirection(IC2, IC4, rx4, SCALE_US);

        if (firstIc == IC2)
        {
          dir = MAST_DIR_LEFT;
        }
        else if (firstIc == IC4)
        {
          dir = MAST_DIR_RIGHT;
        }
//      }
//      else
//      {
////        LED_ERROR_ON;
//      }

      meanTime = (rx2 + rx4) / 2;

//        float mastTime = SIGN(meanDir) * (float) meanTime / ((2*inpCapTimes.n - 6));
      float mastTime = SIGN(dir) * (float) meanTime * TIMER_SCALE_US;
      if (mastTime == 0)
      {
        mastCurrentSpeed = 0;
      }
      else
      {
        mastCurrentSpeed = MOTOR_DEG_PER_PULSE / (mastTime * MAST_MOTOR_RATIO);
      }
      

//      mastCurrentSpeed = MOTOR_ENCODER_RATIO / (mastTime * 360.0f * TIMER_SCALE_US);
//      mastCurrentSpeed = MOTOR_ENCODER_RATIO * MOTOR_DEG_PER_PULSE / (mastTime * TIMER_SCALE_US);
    }
    else
    {
      mastCurrentSpeed = 0;
    }
  }
}