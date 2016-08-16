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
#define ADC_BITS_PER_REVOLUTION   1024u
#define ADC_TOTAL_BITS            51200u
#define POT_DEG_PER_REVOLUTION    7.2f
#define POT_TO_MOTOR_RATIO        50u
#define BITS_TO_DEG               0.00703125    // 360 deg / ADC_TOTAL_BITS


//=========================================
// sUartLineBuffer_t
// Purpose : structure used when receiving
//           data from UART line
//=========================================
typedef struct sPotLineBuffer
{
  size_t length;
  UINT16 buffer [256];
} sPotLineBuffer_t;
//=========================================


//=========================================
// sUartFifoBuffer_t
// Purpose : structure used for the FIFO
//           functions.
//=========================================
typedef struct sPotFifoBuffer
{
  UINT8             inIdx;
  UINT8             outIdx;
  BOOL              bufFull;
  BOOL              bufEmpty;
  const UINT16      maxBufSize;
  sPotLineBuffer_t  lineBuffer;
} sPotFifoBuffer_t;
//=========================================

typedef struct sPotValues
{
  sPotFifoBuffer_t potSamples;
  sPotFifoBuffer_t potStepSamples;
  UINT16  potStepValue;
  UINT32  lastAverage;
  UINT16  lastBits;
  UINT32  zeroInBits;
  UINT16  deadZoneDetect;
  UINT16  nSamples;
  UINT16  stepZero;               /*! Which step represents zero */
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
INT8 AdcAddSample     (sPotValues_t *potValues, UINT16 newSample);
INT8 PotAverage       (sPotValues_t *potValues);
void MastAngleToBits  (sPotValues_t *potValues, float angle, UINT16 *step, UINT16 *bits);

#endif	/* __POTENTIOMETER__ */