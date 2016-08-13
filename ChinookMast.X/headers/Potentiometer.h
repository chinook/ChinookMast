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
#define N_SAMPLES_TO_AVERAGE      200U
#define ADC_BIT_MAX               1023.0f

typedef struct sPotValues
{
  UINT16 potValuesInBits[N_SAMPLES_TO_AVERAGE];
  UINT16 potStepValues  [N_SAMPLES_TO_AVERAGE];
  const UINT16 adcDeadZonePos;
  const UINT16 adcDeadZoneNeg;
  UINT16 nSamples;
  const UINT16 dynamicLimit;
  BOOL oInDeadZone;
} sPotValues_t;


//==============================================================================
// Public function prototypes
//==============================================================================


//==============================================================================
// Public function prototypes
//==============================================================================

INT8  MastBitToAngle  (UINT16 bits, float *angle);
void  MastGetSpeed    (sCmdValue_t *mastPos, sCmdValue_t *mastSpeed, float acqTime);

#endif	/* __POTENTIOMETER__ */