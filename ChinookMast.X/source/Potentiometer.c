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

INT8 PotAddFirstSample (sPotValues_t *potValues)
{
  UINT16 tempStep  = potValues->potStepValue
        ,newSample = potValues->lastBits
        ;
  sPotFifoBuffer_t   *potStepBuf    = &potValues->potStepSamples
                    ,*potSampleBuf  = &potValues->potSamples
                    ;
  
  if (tempStep >= POT_TO_MOTOR_RATIO)
  {
    tempStep = POT_TO_MOTOR_RATIO >> 1;
  }
  PotFifoWrite(potSampleBuf, &newSample);
  PotFifoWrite(potStepBuf  , &tempStep);
  
  potValues->nSamples++;
  
  return 0;
}


INT8 PotAddSample (sPotValues_t *potValues, UINT16 newSample)
{
  UINT16 tempStep;
  sPotFifoBuffer_t   *potStepBuf    = &potValues->potStepSamples
                    ,*potSampleBuf  = &potValues->potSamples
                    ;
//  potSampleBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1] = 1000;
//  newSample = 16;
//  potSampleBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1] = 23;
//  newSample = 970;
  
  INT32 oldChar = potSampleBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1];
  INT32 newChar = newSample;
  INT32 diff    = oldChar - newChar;
//  INT32 result = AbsInt(diff);
  
//  INT32 diff = ((INT32) potSampleBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1]) - ((INT32) newSample);

  if (AbsInt(diff) > potValues->deadZoneDetect)
  {
    if (SignInt(diff) == 1)
    {
      tempStep = (potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1] + 1) % POT_TO_MOTOR_RATIO;
    }
    else
    {
      tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1];
      tempStep = (tempStep ? tempStep : POT_TO_MOTOR_RATIO) - 1;
    }
  }
  else
  {
    tempStep = potStepBuf->lineBuffer.buffer[(potStepBuf->inIdx ? potStepBuf->inIdx : potStepBuf->maxBufSize) - 1];
  }
  if (tempStep >= POT_TO_MOTOR_RATIO)
  {
    tempStep = POT_TO_MOTOR_RATIO - 1;
  }
  PotFifoWrite(potSampleBuf, &newSample);
  PotFifoWrite(potStepBuf, &tempStep);
  
  potValues->nSamples++;
  
  return 0;
}

INT8 PotAverage(sPotValues_t *potValues)
{
  UINT16 iSample  = 0
        ,iMax1    = potValues->nSamples
        ,iMax2    = potValues->nSamples
        ,iMax     = 0
        ,tempBits [256] = {0}
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
    average += tempBits[iSample] + (((UINT32) tempSteps[iSample]) << 10);
  }
  
  average = (float) average / (float) iSample + 0.5f;
  
//  potValues->potStepValue = average / ADC_BITS_PER_REVOLUTION;
  potValues->potStepValue = average >> 10;
  
  potValues->lastAverage = average;
  
  potValues->lastBits = average % ADC_BITS_PER_REVOLUTION;
  
  potValues->nSamples -= iSample;
  
  return 0;
}


inline INT8 PotFifoWrite(sPotFifoBuffer_t *fifo, UINT16 *data)
{
  if (fifo->bufFull)
  {
//    return -1;
    fifo->outIdx = (fifo->outIdx + 1) % fifo->maxBufSize;
    LED_STATUS_ON;
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