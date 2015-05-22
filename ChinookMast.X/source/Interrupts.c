//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Interrupts.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This file declares all interrupt subroutines used.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : All priorities must be set and verified by the developper. All
//           priorities and subpriorities used are there as example. Also, note
//           that interrupt can be defined as iplnauto, iplnsrs and iplnsoft,
//           where n can be from 0 to 7. 7 is the highest priority, while 1 is
//           the lowest. A priority of 0 will disable the interrupt.
//           Subpriorities can go from (lowest to highest) 0 to 3.
//           SRS mode uses the shadow registers, soft uses the software
//           registers and auto is an automatic assignment.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\Interrupts.h"

volatile INT32 Flag_Main_While = 0;

//==============================================================================
//	TIMER INTERRUPTS
//==============================================================================

//=============================================
// Configure the Timer 1 interrupt handler
//=============================================
void __ISR(_TIMER_1_VECTOR, T1_INTERRUPT_PRIORITY) Timer1InterruptHandler(void)
{
  Flag_Main_While = 1;

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[0]++;

  mT1ClearIntFlag();
}

//=============================================
// Configure the Timer 2 interrupt handler
//=============================================
void __ISR(_TIMER_2_VECTOR, T2_INTERRUPT_PRIORITY) Timer2InterruptHandler(void)
{

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[1]++;

  mT2ClearIntFlag();
}

//=============================================
// Configure the Timer 3 interrupt handler
//=============================================
void __ISR(_TIMER_3_VECTOR, T3_INTERRUPT_PRIORITY) Timer3InterruptHandler(void)
{

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[2]++;

  mT3ClearIntFlag();
}

//=============================================
// Configure the Timer 4 interrupt handler
//=============================================
void __ISR(_TIMER_4_VECTOR, T4_INTERRUPT_PRIORITY) Timer4InterruptHandler(void)
{

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[3]++;

  mT4ClearIntFlag();
}

//=============================================
// Configure the Timer 5 interrupt handler
//=============================================
void __ISR(_TIMER_5_VECTOR, T5_INTERRUPT_PRIORITY) Timer5InterruptHandler(void)
{

  // Increment the number of overflows from this timer. Used primarily by Input Capture
  Timer.Var.nOverflows[4]++;

  mT5ClearIntFlag();
}


/*******************************************************************************
 ***********************                               *************************
 ********************           UART INTERRUPTS           **********************
 ***********************                               *************************
 *******************************************************************************/

//=============================================
// Configure the UART 1 interrupt handler
//=============================================

void __ISR(_UART_1_VECTOR, U1_INTERRUPT_PRIORITY) Uart1InterruptHandler(void)
{
	// RX interrupt handling
  //===========================================================
	if(INTGetFlag(INT_SOURCE_UART_RX(UART1)))
	{
    // Clear the RX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_RX(UART1));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================

	// TX interrupt handling
  //===========================================================
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART1)) )
	{
    // Clear the TX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_TX(UART1));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================
}
//=============================================

//=============================================
// Configure the UART 2 interrupt handler
//=============================================

void __ISR(_UART_2_VECTOR, U2_INTERRUPT_PRIORITY) Uart2InterruptHandler(void)
{
	// RX interrupt handling
  //===========================================================
	if(INTGetFlag(INT_SOURCE_UART_RX(UART2)))
	{
    // Clear the RX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_RX(UART2));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================

	// TX interrupt handling
  //===========================================================
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART2)) )
	{
    // Clear the TX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_TX(UART2));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================
}
//=============================================

//=============================================
// Configure the UART 3 interrupt handler
//=============================================

void __ISR(_UART_3_VECTOR, U3_INTERRUPT_PRIORITY) Uart3InterruptHandler(void)
{
	// RX interrupt handling
  //===========================================================
	if(INTGetFlag(INT_SOURCE_UART_RX(UART3)))
	{
    // Clear the RX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_RX(UART3));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================

	// TX interrupt handling
  //===========================================================
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART3)) )
	{
    // Clear the TX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_TX(UART3));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================
}
//=============================================

//=============================================
// Configure the UART 4 interrupt handler
//=============================================

void __ISR(_UART_4_VECTOR, U4_INTERRUPT_PRIORITY) Uart4InterruptHandler(void)
{
	// RX interrupt handling
  //===========================================================
	if(INTGetFlag(INT_SOURCE_UART_RX(UART4)))
	{
    // Clear the RX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_RX(UART4));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================

	// TX interrupt handling
  //===========================================================
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART4)) )
	{
    // Clear the TX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_TX(UART4));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================
}
//=============================================

//=============================================
// Configure the UART 5 interrupt handler
//=============================================

void __ISR(_UART_5_VECTOR, U5_INTERRUPT_PRIORITY) Uart5InterruptHandler(void)
{
	// RX interrupt handling
  //===========================================================
	if(INTGetFlag(INT_SOURCE_UART_RX(UART5)))
	{
    // Clear the RX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_RX(UART5));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================

	// TX interrupt handling
  //===========================================================
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART5)) )
	{
    // Clear the TX interrupt Flag
    INTClearFlag(INT_SOURCE_UART_TX(UART5));

    /*
     *  DEVELOPPER CODE HERE
     */
	}
  //===========================================================
}
//=============================================

//=============================================
// Configure the UART 6 interrupt handler
//=============================================

void __ISR(_UART_6_VECTOR, U6_INTERRUPT_PRIORITY) Uart6InterruptHandler(void)
{
  UINT8  i
        ,iMax   // Read/write max 8 bytes/interrupt
        ,data   // used in UartFifoWrite/Read functions
        ;

	// TX interrupt handling
  //===========================================================

  if ( INTGetEnable ( INT_SOURCE_UART_TX(UART6) ) )               // If TX interrupts enabled
  {
    if ( INTGetFlag ( INT_SOURCE_UART_TX(UART6) ) )               // If TX interrupt occured
    {
      if ( UARTTransmitterIsReady(UART6) && !Uart.Var.uartTxFifo[UART6].bufEmpty )  // If TX buffer is ready to receive data and the user's TX buffer is not empty
      {
        if (Uart.Var.uartTxFifo[UART6].lineBuffer.length < 8)     // Write max 8 bytes/interrupt
        {
          iMax = Uart.Var.uartTxFifo[UART6].lineBuffer.length;
        }
        else
        {
          iMax = 8;
        }

        for (i = 0; i < iMax; i++)
        {
          UartFifoRead((void *) &Uart.Var.uartTxFifo[UART6], &data);  // Copy from user
          U1TXREG = data;                                         // Put data in PIC32's TX buffer
        }
      }

      if (Uart.Var.uartTxFifo[UART6].bufEmpty)                    // If User's TX buffer is empty
      {
        Uart.DisableTxInterrupts(UART6);                          // Disable TX interrupts
      }

      INTClearFlag(INT_SOURCE_UART_TX(UART6));                    // Clear the TX interrupt Flag
    }
  }
  //===========================================================
  

	// RX interrupt handling
  //===========================================================
  if ( INTGetEnable ( INT_SOURCE_UART_RX(UART6) ) )               // If RX interrupts enabled
  {
    if ( INTGetFlag ( INT_SOURCE_UART_RX(UART6) ) )               // If RX interrupt occured
    {
      i = 0;
      iMax = 8;                                                   // Read max 8 bytes/interrupt
      while (   UARTReceivedDataIsAvailable(UART6)                // While RX data available
            && !Uart.Var.uartRxFifo[UART6].bufFull                // and user's RX buffer not full
            && (i < iMax)                                         // and under 8 bytes read
            )
      { // while ^
        data = UARTGetDataByte(UART6);                            // Get data for PIC32's RX FIFO buffer and copy it to user (next line)
        if ( UartFifoWrite((void *) &Uart.Var.uartRxFifo[UART6], &data) < 0 ) // If copy to user did not work
        {
          break;                                                  // Exit while loop
        }
        i++;
      } // end while

      if (!Uart.Var.uartRxFifo[UART6].bufEmpty)                   // If there is data in the user's RX buffer
      {
        Uart.Var.oIsRxDataAvailable[UART6] = 1;                   // Set according flag
      }

      INTClearFlag (INT_SOURCE_UART_RX(UART6) );                  // Clear the RX interrupt Flag

    }
	}
  //===========================================================
}
//=============================================


/*******************************************************************************
 ***********************                               *************************
 ********************           ADC INTERRUPT             **********************
 ***********************                               *************************
 *******************************************************************************/

//=============================================
// Configure the ADC interrupt handler
//=============================================
void __ISR(_ADC_VECTOR, ADC_INTERRUPT_PRIO) AdcInterruptHandler(void)
{
  Adc.Read();               // Read the enabled channels and puts them in Adc.Var.adcReadValues[]
  INTClearFlag(INT_AD1);    // Clear the ADC conversion done interrupt Flag
}
//=============================================