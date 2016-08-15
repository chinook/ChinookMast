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
inline INT8   PotFifoWrite    (sPotFifoBuffer_t *fifo, UINT16 *data);
inline INT8   PotFifoRead     (sPotFifoBuffer_t *fifo, UINT16 *data);
inline UINT32 PotFifoLongRead (sPotFifoBuffer_t *fifo, UINT16 *data, size_t length);


//==============================================================================
// Function definitions
//==============================================================================

/*
 *  0             360
 *  0             51200
 *       24555              
 *          28204           25,65703125
 * -180           180
 */
void MastUpdateAngle (sPotValues_t *potValues)
{
  potValues->angle->previousValue = potValues->angle->currentValue;
  if (potValues->lastAverage >= potValues->zeroInBits)
  {
    potValues->angle->currentValue  = (float) (potValues->lastAverage - potValues->zeroInBits) * BITS_TO_DEG;
  }
  else
  {
    potValues->angle->currentValue  = -1*((float) (potValues->zeroInBits - potValues->lastAverage)) * BITS_TO_DEG;
  }
  
  if (potValues->angle->currentValue > 180)
  {
    potValues->angle->currentValue -= 360;
  }
  else if (potValues->angle->currentValue < -180)
  {
    potValues->angle->currentValue += 360;
  }
}

void MastAngleToBits(sPotValues_t *potValues, float angle, UINT16 *step, UINT16 *bits)
{
  UINT32  totalBits;
  if (angle >= 0)
  {
    totalBits = angle / BITS_TO_DEG + potValues->zeroInBits;
  }
  else
  {
    totalBits = -angle / BITS_TO_DEG + potValues->zeroInBits;
  }
//  *step = totalBits / 1024;
  *step = totalBits >> 10;
  *bits = totalBits % 1024;
}


void MastGetSpeed (sPotValues_t *potValues, float acqTime)
{
  potValues->speed->previousValue = potValues->speed->currentValue;
  potValues->speed->currentValue  = (potValues->angle->currentValue - potValues->angle->previousValue) / acqTime;
}


INT8 PotAddSample (sPotValues_t *potValues, UINT16 newSample)
{
  UINT16 tempStep;
  sPotFifoBuffer_t   *potStepBuf    = &potValues->potStepSamples
                    ,*potSampleBuf  = &potValues->potSamples
                    ;
  
  if (potValues->oInLowerDeadZone || potValues->oInUpperDeadZone)
  {
    // Still in dead zone
    if ( (newSample < (1023 - potValues->dynamicLim)) || (newSample > potValues->dynamicLim) )
    {
      newSample = potValues->deadZoneAvgValue;
      PotFifoWrite(potSampleBuf, &newSample);
    }
    else if (newSample >= potValues->deadZoneUpperLim)
    {
      newSample = potValues->deadZoneAvgValue;
      PotFifoWrite(potSampleBuf, &newSample);

      if (potValues->oInLowerDeadZone)
      {
        potValues->oInLowerDeadZone = 0;
        tempStep = (potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)] - 1) % POT_TO_MOTOR_RATIO;
      }
      else
      {
        tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)];
      }
      PotFifoWrite(potStepBuf, &tempStep);

      potValues->oInUpperDeadZone = 1;
    }
    else if (newSample <= potValues->deadZoneLowerLim)
    {
      newSample = potValues->deadZoneAvgValue;
      PotFifoWrite(potSampleBuf, &newSample);

      if (potValues->oInUpperDeadZone)
      {
        potValues->oInUpperDeadZone = 0;
        tempStep = (potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)] + 1) % POT_TO_MOTOR_RATIO;
      }
      else
      {
        tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)];
      }
      PotFifoWrite(potStepBuf, &tempStep);

      potValues->oInLowerDeadZone = 1;
    }
    else
    {
      if (newSample >= potValues->deadZoneUpperLim)
      {
        newSample = potValues->deadZoneAvgValue;
        PotFifoWrite(potSampleBuf, &newSample);

        tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)];
        PotFifoWrite(potStepBuf, &tempStep);

        potValues->oInUpperDeadZone = 1;
      }
    }
  }
  else // Was not in deadzone
  {
    if (newSample >= potValues->deadZoneUpperLim)
    {
      newSample = potValues->deadZoneAvgValue;
      PotFifoWrite(potSampleBuf, &newSample);

      tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)];
      PotFifoWrite(potStepBuf, &tempStep);

      potValues->oInUpperDeadZone = 1;
    }
    else if (newSample <= potValues->deadZoneLowerLim)
    {
      newSample = potValues->deadZoneAvgValue;
      PotFifoWrite(potSampleBuf, &newSample);

      tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)];
      PotFifoWrite(potStepBuf, &tempStep);

      potValues->oInLowerDeadZone = 1;
    }
    else
    {
      PotFifoWrite(potSampleBuf, &newSample);
      tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx - 1)];
      PotFifoWrite(potStepBuf, &tempStep);
    }
  }
  
  potValues->nSamples++;
  
  return 0;
}

INT8 PotAverage(sPotValues_t *potValues)
{
  UINT16 iSample  = 0
        ,iMax1    = potValues->nSamples
        ,iMax2    = potValues->nSamples
        ,iMax     = 0
        ,tempBit  = 0
        ,tempBits [256] = {0}
        ,tempStep
        ;
  UINT16 tempSteps[256] = {0}
        ;
  
  UINT32 average = 0;
  
  iMax1 = PotFifoLongRead(&potValues->potSamples    , tempBits , iMax1);
  iMax2 = PotFifoLongRead(&potValues->potStepSamples, tempSteps, iMax2);
  
  if (iMax1 != iMax2)
  {
    iMax = MinInt(iMax1, iMax2);
    LED_ERROR_ON;
  }
  else
  {
    iMax = iMax1;
  }
  
  for (iSample = 0; iSample < iMax; iSample++)
  {
//    average += tempBits[iSample] + tempSteps[iSample] * ADC_BITS_PER_REVOLUTION;
    average += tempBits[iSample] + (tempSteps[iSample] << 10);
  }
  
  average = (float) average / (float) iMax + 0.5f;
  
//  potValues->potStepValue = average / 1024;
  potValues->potStepValue = average >> 10;
  
  potValues->lastAverage = average;
  
  potValues->nSamples -= iSample;
  
  return 0;
}


inline INT8 PotFifoWrite(sPotFifoBuffer_t *fifo, UINT16 *data)
{
  if (fifo->bufFull)
  {
//    return -1;
    fifo->outIdx = (fifo->outIdx + 1) % fifo->maxBufSize;
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


inline UINT32 PotFifoLongRead (sPotFifoBuffer_t *fifo, UINT16 *data, size_t length)
{
  size_t tempLength;
  
  if (fifo->bufEmpty)
  {
    return 0;
  }
  
  if (fifo->lineBuffer.length < length)
  {
    length = fifo->lineBuffer.length;
  }
  
  fifo->bufFull = 0;
  
  if ( fifo->inIdx > fifo->outIdx)
  {
    memcpy(&data[0], &fifo->lineBuffer.buffer[fifo->outIdx], 2*length);
  }
  else
  {
    if ( (fifo->maxBufSize - fifo->outIdx) >= length)
    {
      memcpy(&data[0], &fifo->lineBuffer.buffer[fifo->outIdx], 2*length);
    }
    else
    {
      tempLength = fifo->maxBufSize - fifo->outIdx;
      memcpy(&data[0], &fifo->lineBuffer.buffer[fifo->outIdx], 2*tempLength);
      memcpy(&data[tempLength], &fifo->lineBuffer.buffer[0], 2*(length - tempLength));
    }
  }
  
  fifo->outIdx = (fifo->outIdx + length) % fifo->maxBufSize;
  if (fifo->outIdx == fifo->inIdx)
  {
    fifo->bufEmpty = 1;
  }
  fifo->lineBuffer.length -= length;
  return length;
}