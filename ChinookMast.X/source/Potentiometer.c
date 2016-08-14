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


//==============================================================================
// Function definitions
//==============================================================================

void MastBitToAngle (UINT16 bits, float *angle, sPotValues_t *potValues)
{
  *angle = ((float) bits / ADC_BIT_MAX) + (ADC_BIT_MAX / MAST_MOTOR_RATIO * potValues->potStepValues[1]);
}


void MastGetSpeed (sCmdValue_t *mastPos, sCmdValue_t *mastSpeed, float acqTime)
{
  mastSpeed->previousValue = mastSpeed->currentValue;
  mastSpeed->currentValue  = (mastPos->currentValue - mastPos->previousValue) / acqTime;
}


INT8 PotAddSample (UINT16 newSample, sPotValues_t *potValues)
{
  if (potValues->nSamples < N_SAMPLES_TO_AVERAGE)
  {
    potValues->potValuesInBits[potValues->nSamples] = newSample;
  
    if (AbsFloat(newSample - potValues->angle potValues->dynamicLimit))
    potValues->nSamples++;
  }
  else
  {
    return -1;  // Should be processed
  }
  
  
}