//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Mast
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Potentiomer.c
// Author  : Frederic Chasse
// Date    : 2016-08-13
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the C file containing the functions associated with the
//           potentiometer.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#include "..\headers\Potentiometer.h"
#include "..\headers\CommandFunctions.h"


//==============================================================================
// Variable declarations
//==============================================================================


//==============================================================================
// Private function prototypes
//==============================================================================
inline INT8 PotFifoWrite(sPotFifoBuffer_t *fifo, UINT16 *data);
inline INT8 PotFifoRead (sPotFifoBuffer_t *fifo, UINT16 *data);


//==============================================================================
// Function definitions
//==============================================================================

void MastUpdateAngle (sPotValues_t *potValues)
{
  potValues->angle->previousValue = potValues->angle->currentValue;
  potValues->angle->currentValue  = (float) potValues->lastAverage / ADC_BIT_MAX
                                  + ADC_BIT_MAX / MAST_MOTOR_RATIO * potValues->potStepValue;
}


void MastGetSpeed (sPotValues_t *potValues, float acqTime)
{
  potValues->speed->previousValue = potValues->speed->currentValue;
  potValues->speed->currentValue  = (potValues->angle->currentValue - potValues->angle->previousValue) / acqTime;
}


INT8 PotAddSample (UINT16 newSample, sPotValues_t *potValues)
{
  if (potValues->nSamples < N_SAMPLES_TO_AVERAGE)
  {
    if (potValues)
    if ( (newSample >= potValues->deadZoneUpperLim) || (newSample <= potValues->deadZoneLowerLim) )
    {
      potValues->potValuesInBits[potValues->nSamples] = potValues->deadZoneAvgValue;
      potValues->oInDeadZone = 1;
    }
    else
    {
      potValues->potValuesInBits[potValues->nSamples] = newSample;
      potValues->oInDeadZone = 0;
    }
    potValues->nSamples++;
  }
  else
  {
    return -1;  // The average should be processed
  }
  
  return 0;
}

INT8 PotAverage(sPotValues_t *potValues)
{
  UINT16 iSample  = 0
        ,iMax     = potValues->nSamples
        ,temp     = 0
        ;
  
  UINT32 average = 0;
  
  for (iSample = 0; iSample < iMax; iSample++)
  {
    PotFifoRead(&potValues->potSamples, &temp);
    average += temp;
  }
  
  average = (float) average / (float) iMax + 0.5f;
  
  potValues->lastAverage = average;
  
  return 0;
}


inline INT8 PotFifoWrite(sPotFifoBuffer_t *fifo, UINT16 *data)
{
  if (fifo->bufFull)
  {
    return -1;
  }
  fifo->bufEmpty = 0;
  fifo->lineBuffer.buffer[fifo->inIdx] = *data;
  fifo->inIdx = (fifo->inIdx + 1) % fifo->maxBufSize;
  if (fifo->inIdx == fifo->outIdx)
  {
    fifo->bufFull = 1;
  }
  fifo->lineBuffer.length++;
  return 0;
}


inline INT8 PotFifoRead (sPotFifoBuffer_t *fifo, UINT16 *data)
{
  if (fifo->bufEmpty)
  {
    return -1;
  }
  fifo->bufFull = 0;
  *data = fifo->lineBuffer.buffer[fifo->outIdx];
  fifo->outIdx = (fifo->outIdx + 1) % fifo->maxBufSize;
  if (fifo->outIdx == fifo->inIdx)
  {
    fifo->bufEmpty = 1;
  }
  fifo->lineBuffer.length--;
  return 0;
}