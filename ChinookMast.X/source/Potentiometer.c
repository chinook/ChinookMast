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

void MastUpdateAngle (sPotValues_t *potValues)
{
  potValues->angle->previousValue = potValues->angle->currentValue;
  potValues->angle->currentValue  = (float) potValues->lastAverage / ADC_BIT_MAX
                                  + ADC_BIT_MAX / MAST_MOTOR_RATIO * potValues->potStepValues[1];
//  *angle = ((float) bits / ADC_BIT_MAX) + (ADC_BIT_MAX / MAST_MOTOR_RATIO * potValues->potStepValues[1]);
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
    if ( (newSample >= potValues->deadZoneUpperLim) || (newSample <= potValues->deadZoneLowerLim) )
    {
      potValues->potValuesInBits[potValues->nSamples] = potValues->deadZoneAvgValue;
      potValues->oInDeadZone[potValues->nSamples] = 1;
    }
    else
    {
      potValues->potValuesInBits[potValues->nSamples] = newSample;
      potValues->oInDeadZone[potValues->nSamples] = 0;
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
  UINT16 iSample = 0
        ,iMax    = potValues->nSamples
        ;
  
  UINT32 average = 0;
  
  for (iSample = 0; iSample < iMax; iSample++)
  {
    average += potValues->potValuesInBits[iSample];
  }
  
  average = (float) average / (float) iMax + 0.5f;
  
  potValues->lastAverage = average;
  
  potValues->nSamples = 0;
  
  return 0;
}