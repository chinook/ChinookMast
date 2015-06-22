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

volatile sCmdValue_t windAngle  = {.currentValue = 60, .previousValue = 60}
                    ,mastAngle  = {0}
                    ,mastSpeed  = {0}
                    ;

sCmdValue_t  inPi   = {0}
            ,outPi  = {0}
            ;

// Mast general value
extern volatile float mastCurrentPos     // Actual position of Mast
                     ,mastCurrentSpeed   // Actual speed of Mast
                     ;


//==============================================================================
// Mast regulation private functions prototypes
//==============================================================================


//==============================================================================
// Mast regulation functions
//==============================================================================

void TustinZ (sCmdValue_t *input, sCmdValue_t *output)
{
  INT16 newValue;

  output->previousValue = output->currentValue;

  output->currentValue = output->previousValue + T/2 * (input->currentValue + input->previousValue);
}

void SetPwm (float cmd)
{
  if (cmd == 0)
  {
    DRVB_SLEEP = 0;
    Pwm.SetDutyCycle(PWM_2, 500);
    Pwm.SetDutyCycle(PWM_3, 500);
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
  memcpy((void *) &windAngle.currentValue, (void *) &rxWindAngle, 4);

  // Update mast speed
  mastSpeed.previousValue = mastSpeed.currentValue;
  mastSpeed.currentValue  = mastCurrentSpeed;

  // Get mast position from mast speed
  TustinZ((void *) &mastSpeed, (void *) &mastAngle); 

  error = windAngle.currentValue - mastAngle.currentValue;

  if (ABS(error) <= ERROR_THRESHOLD)  // Don't need to move the mast
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