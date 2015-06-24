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

const float  KP = 0.02
            ,KI = 0.08
            ,K  = 0.2
            ,T  = 0.2
            ,pwmMaxDutyCycle  = 0.98
            ,pwmMinDutyCycle  = 0.2
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

BOOL oAtLeastOneEventOccured = 0;

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
    MastManualStop();
//    DRVB_SLEEP = 0;
//    Pwm.SetDutyCycle(PWM_2, 500);
//    Pwm.SetDutyCycle(PWM_3, 500);
  }
  else
  {
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
    mastCurrentSpeed = 0;
  }
  else if ( (SIGN(mastSpeed.currentValue) == MAST_DIR_LEFT) && (!MAST_MIN_OK) )   // Mast too far
  {
    cmd = 0;
    mastCurrentSpeed = 0;
  }
  else if ( (SIGN(mastSpeed.currentValue) == MAST_DIR_RIGHT) && (!MAST_MAX_OK) )  // Mast too far
  {
    cmd = 0;
  }
  else
  {
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

    if (inpCapTimes.n >= INP_CAP_EVENTS_FOR_AVERAGE)
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