/*
 * File:   Potentiometer.h
 * Author: Frederic
 * 
 * Purpose : This is the C file containing the functions associated with the
 *           potentiometer.
 *
 * Created on 2016-08-13
 */

#ifndef __POTENTIOMETER__
#define	__POTENTIOMETER__

#include "Setup.h"
#include "StateMachine.h"
#include "StateFunctions.h"
#include "CommandFunctions.h"


//==============================================================================
// General definitions
//==============================================================================
#define N_SAMPLES_TO_AVERAGE      200u
#define ADC_BIT_MAX               1023.0f

typedef struct sPotValues
{
  UINT16 potValuesInBits[N_SAMPLES_TO_AVERAGE];
  UINT16 potStepValues  [N_SAMPLES_TO_AVERAGE];
  BOOL   oInDeadZone    [N_SAMPLES_TO_AVERAGE];
  UINT16 lastAverage;
  UINT16 nSamples;
  UINT16 stepZero;                /*! Which step represents zero */
  UINT16 bitZero;                 /*! Which bit at the zero step represents zero perfect zero */
  const UINT16 deadZoneUpperLim;  /*! In bits */
  const UINT16 deadZoneLowerLim;  /*! In bits */
  const UINT16 deadZoneAvgValue;  /*! Value to use when in deadzone from either side */
  volatile sCmdValue_t *angle;
  volatile sCmdValue_t *speed;
} sPotValues_t;


//==============================================================================
// Public function prototypes
//==============================================================================


//==============================================================================
// Public function prototypes
//==============================================================================

void MastUpdateAngle  (sPotValues_t *potValues);
void MastGetSpeed     (sPotValues_t *potValues, float acqTime);
INT8 AdcAddSample     (UINT16 newSample, sPotValues_t *potValues);
INT8 PotAverage       (sPotValues_t *potValues);

#endif	/* __POTENTIOMETER__ */