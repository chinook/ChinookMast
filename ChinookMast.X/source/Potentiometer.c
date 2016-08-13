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
sPotValues_t potValues = 
{
  .adcDeadZoneNeg   =  0
 ,.adcDeadZonePos   =  0
 ,.nSamples         =  0
 ,.potStepValues    = {0}
 ,.potValuesInBits  = {0}
 ,.dynamicLimit     =  0
 ,.oInDeadZone      =  0
};

//==============================================================================
// Private function prototypes
//==============================================================================


//==============================================================================
// Function definitions
//==============================================================================

INT8 MastBitToAngle (UINT16 bits, float *angle)
{
  *angle = (float) bits / ADC_BIT_MAX + ADC_BIT_MAX / MAST_MOTOR_RATIO * potValues.potStepValues[1];
}


void MastGetSpeed (sCmdValue_t *mastPos, sCmdValue_t *mastSpeed, float acqTime)
{
  mastSpeed->previousValue = mastSpeed->currentValue;
  mastSpeed->currentValue  = (mastPos->currentValue - mastPos->previousValue) / acqTime;
}