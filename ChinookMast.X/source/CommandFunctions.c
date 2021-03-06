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

#include "CommandFunctions.h"
#include "StateFunctions.h"
#include "Potentiometer.h"


//==============================================================================
// Mast regulation variables
//==============================================================================

// Received from CAN
//=====================================
extern volatile UINT32 rxWindAngle;
//=====================================

extern sPotValues_t potValues;
float overDeadBand = 0;

// Used for the average of the wind angle
//========================================
volatile UINT32 nWindAngleSamples = 0;
volatile float  meanWindAngle = 0;
extern sUartLineBuffer_t buffer;
//========================================

// Used for the average of the wind angle
//========================================
volatile UINT32 nTurbineRpmSamples = 0;
volatile float meanTurbineRpm = 0;
//========================================

// Used when acquiring data from absolute regulator
//=========================================
sCmdData_t    data        = {0};
volatile BOOL oPrintData  =  0;
//=========================================


// Mast general values
//=====================================
extern volatile float  mastCurrentSpeed      // Actual speed of Mast
                      ;

volatile sCmdValue_t windAngle          = {0}
                    ,mastAngle          = {0}
                    ,mastSpeed          = {0}
                    ;

volatile BOOL oMastMaxBlock   = 0     // The system has detected a limit and blocks further rotation from MastManualStop()
             ,oMastMinBlock   = 0
             ,oMast
             ;
//=====================================


// PI values
//=====================================
sCmdValue_t  inPi   = {0}
            ,outPi  = {0}
            ;

// Regulator parameters

/* NEW MCC - NO LOAD
 * 
 * experimental data after initial caracterization
 */
volatile float KP = 0.010f  //Kept old value
              ,KI = 0.01f  //Kept old value
              ,K  = 0.1f  //Initial caract
              ,PWM_MAX_DUTY_CYCLE = 0.800f  //Due to benchtop psu; might otherwise pull too much current and burn mosfets
              ,PWM_MIN_DUTY_CYCLE = 0.0350f  //Otherwise erratic gain
              ,ERROR_THRESHOLD    = 1.000f  //Kept old value
              ,T                  = 0.100f    // Old Comment: Same as TIMER_1
              ;

// Relative mast regulator's variables
volatile float WIND_ANGLE_ZERO = 0.0f
              ,WIND_ANGLE_ERROR_LO_WIND = 12.0f
              ,WIND_ANGLE_ERROR_HI_WIND = 5.0f
              ,TURBINE_HIGH_RPM = 200.0f
              ,tempRpm
              ;
volatile UINT32 WIND_ANGLE_AVG_N = 20000;  // Make a low RPM wind angle avg ang a high rpm one



/* OLD DC MOTOR
 * These are the tested working values WITH the mast attached to the motor
 * shaft, but WITHOUT the blades.
 */
//volatile float KP = 0.010f
//              ,KI = 0.010f
//              ,K  = 0.100f
//              ,PWM_MAX_DUTY_CYCLE = 0.900f
//              ,PWM_MIN_DUTY_CYCLE = 0.030f
//              ,ERROR_THRESHOLD    = 4.000f
//              ,T                  = 0.100f    // Same as TIMER_1
//              ;
/* OLD DC MOTOR
 * These are the tested working values WITHOUT the mast attached to the motor
 * shaft.
 */
//volatile float KP = 0.015f
//              ,KI = 0.030f
//              ,K  = 0.300f
//              ,PWM_MAX_DUTY_CYCLE = 0.980f
//              ,PWM_MIN_DUTY_CYCLE = 0.010f
//              ,ERROR_THRESHOLD    = 0.100f
////              ,T                  = 0.100f    // Same as TIMER_1
//              ,T                  = 1.000f    // Same as TIMER_1
//              ;
//=====================================


// Flags
//=====================================
extern volatile BOOL oCapture1
                    ,oCapture2
                    ,oCapture3
                    ,oCapture4
                    ,oTimerReg
                    ;

BOOL  oFirstTimeInMastStop    = 0
     ,oEmergencyStop          = 0
     ;
//=====================================


//==============================================================================
// Mast regulation functions
//==============================================================================

/*
 * Function : TustinZ
 * Desc :     Discrete integrator using Tustin's method
 * Graphic example :
 *
 *   1     T     z + 1
 *  --- = --- * -------
 *   s     2     z - 1
 *
 *         _______
 *  x(n)  |   1   | y(n)
 * ------>| ----- |------>
 *        |   s   |
 *        |_______|
 *
 *  iLaplace => y(n) = y(n-1) + T/2 * ( x(n-1) + x(n) )
 *
 */
void TustinZ (sCmdValue_t *input, sCmdValue_t *output)
{
  output->previousValue = output->currentValue;
  output->currentValue  = output->previousValue + T/2 * (input->currentValue + input->previousValue);
}


// Adjust the PWM of the motor
void SetPwm (float cmd)
{
  if (cmd == 0)
  {
    if (oEmergencyStop)   // When mast has gone beyond the acceptable limits
    {
      MastManualStop();
      oEmergencyStop = 0;

      // Reset PI internal values
      inPi.currentValue = 0;
      inPi.previousValue = 0;
      outPi.currentValue = 0;
      outPi.previousValue = 0;
      LED_DEBUG4_TOGGLE;
    }
    else if (oFirstTimeInMastStop)  // Do this procedure only once after every movement of the mast
    {

      // DRIVE B
      //==========================================================
      if (USE_DRIVE_B == 1)
      {
        DRVB_SLEEP = 0;
        Pwm.SetDutyCycle(PWM_2, 500);
        Pwm.SetDutyCycle(PWM_3, 500);
      }
      //==========================================================

      // DRIVE A
      //==========================================================
      if (USE_DRIVE_A == 1)
      {
        DRVA_SLEEP = 0;
        Pwm.SetDutyCycle(PWM_4, 500);
        Pwm.SetDutyCycle(PWM_5, 500);
      }
      //==========================================================

#ifndef USE_POTENTIOMETER
      mastCurrentSpeed = 0;
      mastSpeed.previousValue = 0;
      mastSpeed.currentValue = 0;
#endif

      oFirstTimeInMastStop = 0;

      // Reset PI internal values
      inPi.currentValue = 0;
      inPi.previousValue = 0;
      outPi.currentValue = 0;
      outPi.previousValue = 0;

      // DRIVE B
      //==========================================================
      if (USE_DRIVE_B == 1)
      {
        WriteDrive(DRVB, STATUS_Mastw);   // Reset any errors
      }
      //==========================================================

      // DRIVE A
      //==========================================================
      if (USE_DRIVE_A == 1)
      {
        WriteDrive(DRVA, STATUS_Mastw);   // Reset any errors
      }
      //==========================================================

      if (oPrintData)   // Used for debugging with skadi
      {
        UINT16 i;
        INT32 err = 0;
        sUartLineBuffer_t buffer;
        buffer.length = sprintf(buffer.buffer, "\n\ri\tpSeed\tSpeed\tpPos\tPos\tpWind\tWind\tError\tpInPi\tinPi\tpOutPi\tOutPi\tcmd\n\r");
        Uart.PutTxFifoBuffer(UART6, &buffer);
        for (i = 0; i < data.length; i++)
        {
          buffer.length = sprintf(buffer.buffer, "%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n\r", i, data.speedPrevious[i], data.speedCurrent[i], data.posPrevious[i]
                  ,data.posCurrent[i],data.windPrevious[i], data.windCurrent[i], data.error[i], data.inPiPrevious[i], data.inPiCurrent[i], data.outPiPrevious[i]
                  ,data.outPiCurrent[i], data.cmd[i]);
          do
          {
            err = Uart.PutTxFifoBuffer(UART6, &buffer);
          } while (err < 0);
        }
        buffer.buffer[0] = '\n';
        buffer.length = 1;
        do
        {
          err = Uart.PutTxFifoBuffer(UART6, &buffer);
        } while (err < 0);
        
        data.length = 0;
      }
    }
  }
  else // if (cmd != 0)
  {
    oFirstTimeInMastStop = 1;
    
    UINT16 pwm = AbsFloat(cmd) * 500;

    if (SignFloat(cmd) == MAST_DIR_LEFT)
    {
//      if(!MAST_MAX_OK)
//      {
//        buffer.length = sprintf(buffer.buffer, "Reg:Reset Max Block \r\n\n");
//        Uart.PutTxFifoBuffer(UART6, &buffer);
//        oMastMaxBlock = 0;  // Mast is leaving its max limit, reset blokcing flag
//      }
      // DRIVE B
      //==========================================================
      if (USE_DRIVE_B == 1)
      {
        DRVB_SLEEP = 1;
        Pwm.SetDutyCycle(PWM_2, 500 + pwm);
        Pwm.SetDutyCycle(PWM_3, 500 - pwm);
      }
      //==========================================================

      // DRIVE A
      //==========================================================
      if (USE_DRIVE_A == 1)
      {
        DRVA_SLEEP = 1;
        Pwm.SetDutyCycle(PWM_4, 500 + pwm);
        Pwm.SetDutyCycle(PWM_5, 500 - pwm);
      }
      //==========================================================
    }
    else if (SignFloat(cmd) == MAST_DIR_RIGHT)
    {
//      if(!MAST_MIN_OK)
//      {
//        buffer.length = sprintf(buffer.buffer, "Reg:Reset Min Block \r\n\n");
//        Uart.PutTxFifoBuffer(UART6, &buffer);
//        oMastMinBlock = 0;  // Reset blocking flag
//      }
      // DRIVE B
      //==========================================================
      if (USE_DRIVE_B == 1)
      {
        DRVB_SLEEP = 1;
        Pwm.SetDutyCycle(PWM_2, 500 - pwm);
        Pwm.SetDutyCycle(PWM_3, 500 + pwm);
      }
      //==========================================================

      // DRIVE A
      //==========================================================
      if (USE_DRIVE_A == 1)
      {
        DRVA_SLEEP = 1;
        Pwm.SetDutyCycle(PWM_4, 500 - pwm);
        Pwm.SetDutyCycle(PWM_5, 500 + pwm);
      }
      //==========================================================
    }
  }
}


/*
 *  ____________     ___  error  ___     ___  inPi  _______ outPi ____         ___     ________   __________      _______
 * | wind angle |-> /+  \ ----->| K |-> /+  \ ---> | 1 / s |---->| KI |-----> /+  \ ->| Motor |->| Encodeur |--->| 1 / s |__
 * |____________|   \_-_/       |___|   \_-_/  |   |_______|     |____|       \_+_/   |_______|  |__________| |  |_______|  |
 *                    ^                   ^    |                  ____          ^                             |             |
 *                    |                   |    |                 | KP |         |                             |             |
 *                    |                   |     ---------------->|____|---------             mastCurrentSpeed |             |
 *                    |                   |___________________________________________________________________|             |
 *                    |_____________________________________________________________________________________________________|
 *                                                                                        mastCurrentPos
 */
void Regulator (void)
{
  float  cmd
        ,error
        ,tempWind
        ;

  Nop();
  // Update wind angle
  windAngle.previousValue = windAngle.currentValue;
  
//  memcpy ((void *) &tempWind, (void *) &rxWindAngle, 4);  // Copy contents of UINT32 into float
  //========================//
  //Wind Angle Update/Processing
  //========================//
  if (nWindAngleSamples != 0)
  {
    tempWind = meanWindAngle / nWindAngleSamples;
    meanWindAngle = 0;
    nWindAngleSamples = 0;
  }
  else  // If no new sample was received
  {
    tempWind = windAngle.previousValue;   // Keep previous value as current value
  }

  /*
   * If the wind is not in the acceptable range, change the command to the MAX
   * or MIN. Mast must not go beyond these values.
   */
  if (tempWind > MAST_MAX)
  {
    windAngle.currentValue = MAST_MAX;
  }
  else if (tempWind < MAST_MIN)
  {
    windAngle.currentValue = MAST_MIN;
  }
  else // if (tempWind != windAngle.currentValue)
  {
    windAngle.currentValue = tempWind;
  }

//========================//
//Mast Speed Update
//========================//
#ifdef USE_POTENTIOMETER
  //MastGetSpeed(&potValues, T);
  mastCurrentSpeed = potValues.speed->currentValue;
#else
  mastSpeed.previousValue = mastSpeed.currentValue;
  mastSpeed.currentValue  = mastCurrentSpeed;

  // Get mast position from mast speed
  TustinZ((void *) &mastSpeed, (void *) &mastAngle);  // Discrete integrator
#endif

//========================//
//Mast Angle Update
//========================//
  /*
   * Some kind of modulo
   */
  if (mastAngle.currentValue > 180)
  {
    mastAngle.currentValue -= 360;
    Nop();
  }
  else if (mastAngle.currentValue < -180)
  {
    mastAngle.currentValue += 360;
    Nop();
  }

  /*
   * The rest of the code goes like the graphic at the top of this function.
   * The only parts added adjust the command from PWM_MIN_DUTY_CYCLE to
   * PWM_MAX_DUTY_CYCLE. The algorithm also checks if the mast has passed the
   * limits MAST_MIN and MAST_MAX.
   */

//========================//
// Error - Signal Processing
//========================//
  error = windAngle.currentValue - mastAngle.currentValue;
  Nop();
  
  if (AbsFloat(error) <= ERROR_THRESHOLD)  // Don't need to move the mast
  {
    cmd = 0;
  }
  else if ( (SignFloat(mastSpeed.currentValue) == MAST_DIR_LEFT) && (!MAST_MIN_OK) )   // Mast too far
  {
    oEmergencyStop = 1;
    cmd = 0;
    Nop();
  }
  else if ( (SignFloat(mastSpeed.currentValue) == MAST_DIR_RIGHT) && (!MAST_MAX_OK) && (mastSpeed.currentValue != 0) )  // Mast too far
  {
    oEmergencyStop = 1;
    cmd = 0;
    Nop();
  }
  else
  {
    oEmergencyStop = 0;
    oFirstTimeInMastStop = 1;

    error -= SignFloat(error) * ERROR_THRESHOLD;   // Substract the ERROR_THRESHOLD to reduce the risk of an abrupt stop by the mast

//========================//
// inPI - Signal Processing
//========================//    
    
    inPi.previousValue = inPi.currentValue;
    inPi.currentValue  = K * error - mastSpeed.currentValue;
    
//========================//
// Integral Computation
//========================//
    
    TustinZ((void *) &inPi, (void *) &outPi);
    Nop();

//========================//
// Process Command Processing
//========================//
    
    cmd = inPi.currentValue * KP + outPi.currentValue * KI;

//========================//
// PWM Duty Cycle Calculation
//========================//
    
    if      (AbsFloat(cmd) > PWM_MAX_DUTY_CYCLE)
    {
      cmd = SignFloat(cmd) * PWM_MAX_DUTY_CYCLE;
    }
    else if (AbsFloat(cmd) < PWM_MIN_DUTY_CYCLE)
    {
      cmd = SignFloat(cmd) * PWM_MIN_DUTY_CYCLE;
    }
  }

  if (oPrintData && oFirstTimeInMastStop)   // Used for debugging with skadi
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

void RelativeWAngleRegulator(void)
{
  float tempWind, windAngleError;
  
  if (nTurbineRpmSamples > 0)
  {
    tempRpm = meanTurbineRpm / nTurbineRpmSamples;
    meanTurbineRpm = 0;
    nTurbineRpmSamples = 0;
    
    // Sets the wind angle tolerance
    windAngleError = WIND_ANGLE_ERROR_LO_WIND;
  }
  
  // Compute Wind Angle from sensor
  if (nWindAngleSamples >= WIND_ANGLE_AVG_N)
  {
    // Depending on SensTel's wind direction broadcast frequency, 
    tempWind = meanWindAngle / nWindAngleSamples; // nWindAngleSamples was used
    meanWindAngle = 0;
    nWindAngleSamples = 0;
    
    overDeadBand = (tempWind >= WIND_ANGLE_ERROR_LO_WIND);
  
	if ( (tempWind >= WIND_ANGLE_ERROR_LO_WIND) )
	{
	  MastManualRight();
//      MastManualLeft();
	}
	else if (tempWind <= -(WIND_ANGLE_ERROR_LO_WIND) )
	{
	  MastManualLeft();
//      MastManualRight();
	}
	else{
	  MastStop();
    }
  }
}
//==============================================================================
// Input Capture functions
//==============================================================================
void AssessMastValues (void)
{
  INT64  rx1
        ,rx2
        ,rx3
        ,rx4
        ;
  INT8 firstIc;
  UINT64 meanTime   = 0;
  INT8 dir    = 0;

  /*
   * If a capture event has occured on both inputs, check the time of both and
   * get the direction of the mast (MAST_DIR_LEFT or MAST_DIR_RIGHT). Then, using
   * the known ratios of ENCODER / MOTOR / MAST, calculate the speed of the mast
   * in degrees / second [deg/s].
   */

  // DRIVE B
  //============================================================================
  if (USE_DRIVE_B == 1)
  {
    if (oCapture2 && oCapture4)
    {
      oCapture2 = 0;
      oCapture4 = 0;

      rx2 = InputCapture.GetTimeBetweenCaptures(IC2, SCALE_US);
      rx4 = InputCapture.GetTimeBetweenCaptures(IC4, SCALE_US);

      if ( !((rx2 > 2000000) || (rx4 > 2000000)) )  // It would mean 0.34 deg/s for the motor shaft, consider it zero
      {
        firstIc = InputCapture.GetDirection(IC2, IC4, rx4, SCALE_US);

        if (firstIc == IC2)
        {
          dir = MAST_DIR_LEFT;
        }
        else if (firstIc == IC4)
        {
          dir = MAST_DIR_RIGHT;
        }

  //      meanTime = (rx2 + rx4) / 2;
        meanTime = (rx2 + rx4) >> 1;   // Divide by 2

        float mastTime = SignInt(dir) * (float) meanTime * TIMER_SCALE_US;

        if (mastTime == 0)
        {
          mastCurrentSpeed = 0;
        }
        else
        {
            mastCurrentSpeed = MOTOR_DEG_PER_PULSE / (mastTime * MAST_MOTOR_RATIO);
        }
      }
      else
      {
        mastCurrentSpeed = 0;
      }
    }
  }
  //============================================================================


  // DRIVE A
  //============================================================================
  if (USE_DRIVE_A == 1)
  {
    if (oCapture1 && oCapture3)
    {
      oCapture1 = 0;
      oCapture3 = 0;

      rx1 = InputCapture.GetTimeBetweenCaptures(IC1, SCALE_US);
      rx3 = InputCapture.GetTimeBetweenCaptures(IC3, SCALE_US);

      if ( !((rx1 > 2000000) || (rx3 > 2000000)) )  // It would mean 0.34 deg/s for the motor shaft, consider it zero
      {
        firstIc = InputCapture.GetDirection(IC1, IC3, rx3, SCALE_US);

        if (firstIc == IC1)
        {
          dir = MAST_DIR_LEFT;
        }
        else if (firstIc == IC3)
        {
          dir = MAST_DIR_RIGHT;
        }

  //      meanTime = (rx1 + rx3) / 2;
        meanTime = (rx1 + rx3) >> 1;   // Divide by 2

        float mastTime = SignInt(dir) * (float) meanTime * TIMER_SCALE_US;

        if (mastTime == 0)
        {
          mastCurrentSpeed = 0;
        }
        else
        {
          mastCurrentSpeed = MOTOR_DEG_PER_PULSE / (mastTime * MAST_MOTOR_RATIO);
        }
      }
      else
      {
        mastCurrentSpeed = 0;
      }
    }
  }
  //============================================================================
}