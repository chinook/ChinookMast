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
  UINT16 nSamples;
  UINT16 stepZero;            /*! Which step represents zero */
  UINT16 bitZero;             /*! Which bit at the zero step represents zero perfect zero */
  const UINT16 dynamicLimit;  /*! In bits */
  volatile sCmdValue_t *angle;
  volatile sCmdValue_t *speed;
} sPotValues_t;


//==============================================================================
// Public function prototypes
//==============================================================================


//==============================================================================
// Public function prototypes
//==============================================================================

void MastBitToAngle (UINT16 bits, float *angle, sPotValues_t *potValues);
void MastGetSpeed   (sCmdValue_t *mastPos, sCmdValue_t *mastSpeed, float acqTime);
INT8 AdcAddSample   (UINT16 newSample, sPotValues_t *potValues);

#endif	/* __POTENTIOMETER__ */