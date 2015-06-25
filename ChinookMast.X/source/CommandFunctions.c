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

sCmdData_t data = {0};

const float  KP = 0.015f
//            ,KI = 0.075f
            ,KI = 0.100f
            ,K  = 1.5f
            ,T  = 0.2f
            ,pwmMaxDutyCycle  = 0.98f
            ,pwmMinDutyCycle  = 0.2f
            ;

volatile sCmdValue_t windAngle          = {.currentValue = 60, .previousValue = 60}
                    ,mastAngle          = {0}
                    ,mastSpeed          = {0}
                    ,mastSpeedRealTime  = {0}   // Used as fast as possible
                    ,mastPosRealTime    = {0}   // Used as fast as possible
                    ;

sCmdValue_t  inPi   = {0}
            ,outPi  = {0}
            ;

sInpCapValues_t inpCapTimes = {0};

// Mast general value
extern volatile float  mastCurrentSpeed      // Actual speed of Mast
                      ;

extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oTimerReg
                    ;

BOOL  oAtLeastOneEventOccured = 0
     ,oFirstTimeInMastStop    = 0
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
        UINT8 buffer[130];
        UINT8 n;
        n = sprintf(buffer, "\n\ri\tpSeed\tSpeed\tpPos\tPos\tpWind\tWind\tError\tpInPi\tinPi\tpOutPi\tOutPi\tcmd\n\r");
        Uart.SendDataBuffer(UART6, buffer, n);
        UINT8 i;
        for (i = 0; i < data.length; i++)
        {
          n = sprintf(buffer, "%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n\r", i, data.speedPrevious[i], data.speedCurrent[i], data.posPrevious[i]
                  ,data.posCurrent[i],data.windPrevious[i], data.windCurrent[i], data.error[i], data.inPiPrevious[i], data.inPiCurrent[i], data.outPiPrevious[i]
                  ,data.outPiCurrent[i], data.cmd[i]);
          Uart.SendDataBuffer(UART6, buffer, n);
        }
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
        ;

  // Update wind angle
  windAngle.previousValue = windAngle.currentValue;
//  memcpy((void *) &windAngle.currentValue, (void *) &rxWindAngle, 4);
  windAngle.currentValue = 60;

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
  else if ( (SIGN(mastSpeed.currentValue) == MAST_DIR_RIGHT) && (!MAST_MAX_OK) )  // Mast too far
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

    if (ABS(cmd) > pwmMaxDutyCycle)
    {
      cmd = SIGN(cmd) * pwmMaxDutyCycle;
    }
    else if (ABS(cmd) < pwmMinDutyCycle)
    {
      cmd = SIGN(cmd) * pwmMinDutyCycle;
    }
  }

  if (PRINT_DATA && oFirstTimeInMastStop)
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
  
  if (oCapture2 && oCapture4)
  {
    oAtLeastOneEventOccured = 1;
    oCapture2 = 0;
    oCapture4 = 0;

    rx2 = InputCapture.GetTimeBetweenCaptures(IC2, SCALE_US);

    rx4 = InputCapture.GetTimeBetweenCaptures(IC4, SCALE_US);

    if (ABS(100 - rx2*100/rx4) < 20)
    {
      inpCapTimes.inpCapTime2[inpCapTimes.n] = rx2;
      inpCapTimes.inpCapTime4[inpCapTimes.n] = rx4;

      firstIc = InputCapture.GetDirection(IC2, IC4, rx4, SCALE_US);

      if (firstIc == IC2)
      {
        inpCapTimes.dir[inpCapTimes.n] = MAST_DIR_LEFT;
        inpCapTimes.n++;
      }
      else if (firstIc == IC4)
      {
        inpCapTimes.dir[inpCapTimes.n] = MAST_DIR_RIGHT;
        inpCapTimes.n++;
      }
    }

    if (inpCapTimes.n >= INP_CAP_EVENTS_FOR_AVERAGE + 3)
    {
      UINT64 meanTime   = 0;
      INT16  meanDir    = 0;
      UINT8  i          = 0;

      for (i = 3; i < inpCapTimes.n; i++)  // Skip 3 first values
      {
        meanTime  += inpCapTimes.inpCapTime2[i] + inpCapTimes.inpCapTime4[i];
        meanDir   += inpCapTimes.dir[i];
      }

//      float mastTime = SIGN(meanDir) * (float) meanTime / ((2*inpCapTimes.n - 6) * MOTOR_ENCODER_RATIO * MAST_MOTOR_RATIO);

//      mastCurrentSpeed = MOTOR_DEG_PER_PULSE * MAST_MOTOR_RATIO / mastTime;

      float mastTime = SIGN(meanDir) * (float) meanTime / ((2*inpCapTimes.n - 6));

      mastCurrentSpeed = MOTOR_ENCODER_RATIO / (mastTime * 360.0f * TIMER_SCALE_US);
//      mastCurrentSpeed = 980 / (mastTime * 360.0f * TIMER_SCALE_US);

//      float mastTime = SIGN(meanDir) * (float) meanTime / ( (2*inpCapTimes.n - 6) * MOTOR_ENCODER_RATIO);
//
//      mastCurrentSpeed = MOTOR_DEG_PER_PULSE / mastTime * TIMER_SCALE_US;

      inpCapTimes.n = 0;
    }
  }
}