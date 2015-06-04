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
volatile UINT8 iByteToSend = 0;


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
          U6TXREG = data;                                         // Put data in PIC32's TX buffer
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


/*******************************************************************************
 ***********************                               *************************
 ********************           I2C INTERRUPT             **********************
 ***********************                               *************************
 *******************************************************************************/

//=============================================
// Configure the I2C4 interrupt handler
//=============================================
void __ISR(_I2C_4_VECTOR, I2C4_INT_PRIORITY) I2c4InterruptHandler(void)
{
//  if (INTGetFlag(INT_I2C4S))  // Slave interrupt
//  {
//    INTClearFlag(INT_I2C4S);
//  }
  sI2cCmdBuffer_t masterData;
  
  if (INTGetFlag(INT_I2C4B))  //Bus Collision interrupt
  {
    INTClearFlag(INT_I2C4B);
  }
  
  if (INTGetFlag(INT_I2C4M))  // Master interrupt
  {
    INTClearFlag(INT_I2C4M);

    if (I2c.Var.oReadDataInNextInterrupt[I2C4])   // If a read was started last interrupt
    {
      masterData.data  = I2C4RCV;   // Read from I2C buffer
      masterData.state = I2C_MASTER_RECEIVE_DATA;
      I2cFifoWrite((void *) &I2c.Var.i2cUserFifo[I2C4], &masterData);
      I2c.Var.oRxDataAvailable[I2C4] = 1;
      I2c.Var.oReadDataInNextInterrupt[I2C4] = 0;
    }
    
    if (I2c.Var.oI2cWriteIsRunning)
    {
      I2cFifoRead((void *) &I2c.Var.i2cWriteQueue[I2C4], &masterData);
      switch (masterData.state)
      {
      //======================================================  
        case I2C_MASTER_RECEIVE_DATA : 
          I2C4CONbits.RCEN = 1;                         //Receive byte sequence
          I2c.Var.oReadDataInNextInterrupt[I2C4] = 1;   // Flag for the next interrupt to read the rx buffer
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_START_CONDITION : 
          I2C4CONbits.SEN = 1;                          //start condition sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_STOP_CONDITION : 
          I2C4CONbits.PEN = 1;
//          if (iCurrentState >=8)
//          {
//            if (!I2CByteWasAcknowledged(I2C4))
//            {
//              iCurrentState -= 3;
//            }
//          }
          break;
      //====================================================== 

      //======================================================    
        case I2C_MASTER_TRANSMIT_DATA : 
          I2C4TRN = masterData.data;
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_DONE : 
          if (I2c.Var.i2cWriteQueue[I2C4].bufEmpty)   // Nothing more to send
          {
            I2c.Var.oI2cWriteIsRunning[I2C4] = 0;       // Turn off writing process
          }
          else
          {
            I2c.EnableInterrupt(I2C4, I2C_MASTER_INTERRUPT);  // Start another writing process
          }
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_REPEAT_START : 
          I2C4CONbits.RSEN = 1;   //repeated start condition sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_SLAVE_SENT_STOP :
          LED_ERROR_ON;
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_SEND_ACK : 
          I2C4CONbits.ACKDT = 0;  //ACK
          I2C4CONbits.ACKEN = 1;  //Send ACK sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_SEND_NACK : 
          I2C4CONbits.ACKDT = 1;  //NACK
          I2C4CONbits.ACKEN = 1;  //Send NACK sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_CMD_ERROR : 
          LED_ERROR_ON;
          break;
      //====================================================== 

      //======================================================  
        default : 
          break;
      //======================================================  
      } // end switch
    } // end if
    
    

    if (I2c.Var.oI2cReadIsRunning)
    {
      I2cFifoRead((void *) &I2c.Var.i2cReadQueue[I2C4], &masterData);
      switch (masterData.state)
      {
      //======================================================  
        case I2C_MASTER_RECEIVE_DATA : 
          I2C4CONbits.RCEN = 1;   //Receive byte sequence
          I2c.Var.oReadDataInNextInterrupt[I2C4] = 1;
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_START_CONDITION : 
          I2C4CONbits.SEN = 1;    //start condition sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_REPEAT_START : 
          I2C4CONbits.RSEN = 1;   //repeated start condition sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_STOP_CONDITION : 
          I2C4CONbits.PEN = 1;
          break;
      //====================================================== 

      //======================================================    
        case I2C_MASTER_TRANSMIT_DATA : 
          I2C4TRN = masterData.data;
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_DONE : 
          if (I2c.Var.i2cReadQueue[I2C4].bufEmpty)            // Nothing more to send
          {
            I2c.Var.oI2cReadIsRunning[I2C4] = 0;              // Turn off reading process
          }
          else
          {
            I2c.EnableInterrupt(I2C4, I2C_MASTER_INTERRUPT);  // Start another reading process
          }
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_SLAVE_SENT_STOP :
          LED_ERROR_ON;
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_SEND_ACK : 
          I2C4CONbits.ACKDT = 0;  //ACK
          I2C4CONbits.ACKEN = 1;  //Send ACK sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_MASTER_SEND_NACK : 
          I2C4CONbits.ACKDT = 1;  //NACK
          I2C4CONbits.ACKEN = 1;  //Send NACK sequence
          break;
      //====================================================== 

      //======================================================  
        case I2C_CMD_ERROR : 
          LED_ERROR_ON;
          break;
      //======================================================

      //======================================================  
        default : 
          break;
      //======================================================  
      } // end switch
    } // end if
  }  // end if
}   // end if
//=============================================