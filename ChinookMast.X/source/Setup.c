//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Setup.c
// Author  : Frederic Chasse
// Date    : 2015-01-03
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Purpose : This is the C file for the setup of the system. It contains the
//           initialization functions.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Notes   : For ChinookLib to be useable, it must be cloned on your hard
//           drive so the path
//               "..\..\..\ChinookLib\ChinookLib.X\headers\ChinookLib.h"
//           references an existing file.
//
//           Function names can and should be renamed by the user to improve the
//           readability of the code. Also, the LED used for testing errors in
//           TimerInit is a LED on the MAX32 development board. Developpers
//           should test for errors by the means (hardware of software) they
//           judge are the best.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "..\headers\Setup.h"
#include "..\headers\Interrupts.h"
#include "..\headers\SkadiFunctions.h"


//==============================================================================
//	INIT FUNCTIONS
//==============================================================================

/***********************************
 * Table of functions used in Skadi
 **********************************/
sSkadiCommand_t skadiCommandTable[] =
{
   {"LedDebug"    , LedDebug    , 1, "Usage : flash Led DEBUG"}   // 1 argument
  ,{"LedCan"      , LedCan      , 1, "Usage : flash Led CAN"}     // 1 argument
  ,{"ReInitSystem", ReInitSystem, 0, "Redo StateInit()"}          // 0 argument
};

//===========================
//	INIT TIMERS
//===========================
void InitTimer(void)
{

  INT32 timerCounterValue = 0;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Open timers
//%%%%%%%%%%%%%%%%%%%%%%%%%%%
  timerCounterValue = Timer.Open(TIMER_1, 500, SCALE_US);   // Open Timer 1 with a period of 500 ms
//  timerCounterValue = Timer.Open(TIMER_2, 500, SCALE_US);   // Open Timer 2 with a period of 500 us
//  timerCounterValue = Timer.Open(TIMER_3, 50, SCALE_US);   // Open Timer 3 with a period of 500 ms
//  timerCounterValue = Timer.Open(TIMER_4, 500, SCALE_MS);   // Open Timer 4 with a period of 500 ms
//  timerCounterValue = Timer.Open(TIMER_5, 500, SCALE_US);   // Open Timer 5 with a period of 500 us
  
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Configure timer interrupts
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Timer.ConfigInterrupt(TIMER_1, TIMER1_INTERRUPT_PRIORITY, TIMER1_INTERRUPT_SUBPRIORITY); // Sets the priority of the TIMER_1 to the values specified in Interrupt.h
//  Timer.ConfigInterrupt(TIMER_2, TIMER2_INTERRUPT_PRIORITY, TIMER2_INTERRUPT_SUBPRIORITY); // Sets the priority of the TIMER_2 to the values specified in Interrupt.h
//  Timer.ConfigInterrupt(TIMER_3, TIMER3_INTERRUPT_PRIORITY, TIMER3_INTERRUPT_SUBPRIORITY); // Sets the priority of the TIMER_3 to the values specified in Interrupt.h
//  Timer.ConfigInterrupt(TIMER_4, TIMER4_INTERRUPT_PRIORITY, TIMER4_INTERRUPT_SUBPRIORITY); // Sets the priority of the TIMER_4 to the values specified in Interrupt.h
//  Timer.ConfigInterrupt(TIMER_5, TIMER5_INTERRUPT_PRIORITY, TIMER5_INTERRUPT_SUBPRIORITY); // Sets the priority of the TIMER_5 to the values specified in Interrupt.h

}

//===========================
//	INIT SKADI
//===========================
void InitSkadi(void)
{
  Skadi.Init(skadiCommandTable, sizeof(skadiCommandTable)/sizeof(sSkadiCommand_t), UART6, FALSE);
}

//===========================
//	INIT SPI
//===========================
void InitSpi(void)
{
  INT8 err = 0;
  SpiOpenFlags_t oMasterFlags =   SPI_MASTER_MODE
                                | SPI_16_BITS_CHAR
                                | SPI_ENHANCED_BUFFER_MODE
                                | SPI_TX_EVENT_BUFFER_SR_EMPTY
                                | SPI_RX_EVENT_BUFFER_NOT_EMPTY
                ;

  err = Spi.Open(SPI4, oMasterFlags, 5e5);   // Open the SPI4 as a master at a bitrate of 5 MHz
  if (err < 0)                // Check for errors
  {
    LED_ERROR_OFF;
  }

  // SPI interrupts not functionnal as of yet
//  Spi.ConfigInterrupt(SPI2, SPI2_INTERRUPT_PRIORITY, SPI2_INTERRUPT_SUBPRIORITY);  // Configure Interrupt for SPI2
//  Spi.ConfigInterrupt(SPI3, SPI3_INTERRUPT_PRIORITY, SPI4_INTERRUPT_SUBPRIORITY);  // Configure Interrupt for SPI3
//  Spi.ConfigInterrupt(SPI4, SPI3_INTERRUPT_PRIORITY, SPI4_INTERRUPT_SUBPRIORITY);  // Configure Interrupt for SPI4
}


//===========================
//	INIT PWM
//===========================
void InitPwm(void)
{

  // Open PWM1 using Timer3 with 50% duty cycle and 0% offset
  Pwm.Open(PWM_1);
  Pwm.SetDutyCycle  (PWM_1, 50);
  Pwm.SetPulseOffset(PWM_1,  0);

  // Open PWM2 using Timer3 with 50% duty cycle and 0% offset
  Pwm.Open(PWM_2);
  Pwm.SetDutyCycle  (PWM_2, 50);
  Pwm.SetPulseOffset(PWM_2,  0);

  // Open PWM3 using Timer3 with 50% duty cycle and 0% offset
  Pwm.Open(PWM_3);
  Pwm.SetDutyCycle  (PWM_3, 50);
  Pwm.SetPulseOffset(PWM_3,  0);

  // Open PWM4 using Timer3 with 50% duty cycle and 0% offset
  Pwm.Open(PWM_4);
  Pwm.SetDutyCycle  (PWM_4, 50);
  Pwm.SetPulseOffset(PWM_4,  0);

  // Open PWM5 using Timer3 with 50% duty cycle and 0% offset
  Pwm.Open(PWM_5);
  Pwm.SetDutyCycle  (PWM_5, 50);
  Pwm.SetPulseOffset(PWM_5,  0);

}


//===========================
//	INIT PORTS
//===========================
void InitPorts(void)
{
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Set unused pins as input to protect the pins of the microcontroller
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Port.A.CloseBits  ( BIT_0  | BIT_1  | BIT_2  | BIT_3      // RA8, RA11-13 non existent
                    | BIT_4  | BIT_5  | BIT_6  | BIT_7
                    | BIT_9  | BIT_10 | BIT_14 | BIT_15 );

  Port.B.CloseBits  ( BIT_0  | BIT_1  | BIT_2  | BIT_3
                    | BIT_4  | BIT_5  | BIT_6  | BIT_7
                    | BIT_8  | BIT_9  | BIT_10 | BIT_11
                    | BIT_12 | BIT_13 | BIT_14 | BIT_15 );

  Port.C.CloseBits  ( BIT_1  | BIT_2  | BIT_3  | BIT_4      // RC0, RC5-11 non existent
                    | BIT_12 | BIT_13 | BIT_14 | BIT_15 );

  Port.D.CloseBits  ( BIT_0  | BIT_1  | BIT_2  | BIT_3
                    | BIT_4  | BIT_5  | BIT_6  | BIT_7
                    | BIT_8  | BIT_9  | BIT_10 | BIT_11
                    | BIT_12 | BIT_13 | BIT_14 | BIT_15 );

  Port.E.CloseBits  ( BIT_0  | BIT_1  | BIT_2  | BIT_3      // RE10-15 non existent
                    | BIT_4  | BIT_5  | BIT_6  | BIT_7
                    | BIT_8  | BIT_9  );

  Port.F.CloseBits  ( BIT_0  | BIT_1  | BIT_2  | BIT_3      // RF6-7, RF9-11, RF14-15 non existent
                    | BIT_4  | BIT_5  | BIT_8  | BIT_12
                    | BIT_13 );

  Port.G.CloseBits  ( BIT_0  | BIT_1  | BIT_2  | BIT_3      // RG4-5, RG10-11 non existent
                    | BIT_6  | BIT_7  | BIT_8  | BIT_9
                    | BIT_12 | BIT_13 | BIT_14 | BIT_15 );

   /* Setup IO LED */
    Port.B.SetPinsDigitalOut(BIT_8);      // LED_DEBG0
    Port.B.SetPinsDigitalOut(BIT_9);      // LED_DEBG1
    Port.B.SetPinsDigitalOut(BIT_10);     // LED_DEBG2
    Port.B.SetPinsDigitalOut(BIT_11);     // LED_DEBG3
    Port.B.SetPinsDigitalOut(BIT_12);     // LED_DEBG4
    Port.B.SetPinsDigitalOut(BIT_13);     // LED_CAN
    Port.B.SetPinsDigitalOut(BIT_15);     // LED_ERROR
    Port.F.SetPinsDigitalOut(BIT_3);      // LED_STATUS

    /* Setup  IO switch */
    Port.E.SetPinsDigitalIn(BIT_5);      // SW1
    Port.E.SetPinsDigitalIn(BIT_6);      // SW2
    Port.E.SetPinsDigitalIn(BIT_7);      // SW3

    /* Setup  Output to control Drive A */
    Port.E.SetPinsDigitalOut(BIT_3);      // DRVA_RESET
    Port.E.SetPinsDigitalOut(BIT_4);      // DRVA_SLEEP
    Port.E.SetPinsDigitalOut(BIT_1);      // DRVA_DIR
    Port.E.SetPinsDigitalOut(BIT_2);      // DRVA_STEP
    Port.D.SetPinsDigitalOut(BIT_4);      // DRVA_BIN1
    Port.D.SetPinsDigitalOut(BIT_2);      // DRVA_BIN2

    /* Setup  Input Drive A */
    Port.D.SetPinsDigitalIn(BIT_6);      // DRVA_FLT
    Port.D.SetPinsDigitalIn(BIT_5);      // DRVA_STALL

    /* Setup  Output to control Drive B */
//    Port.G.SetPinsDigitalOut(BIT_2);      // DRVB_RESET
    Port.D.SetPinsDigitalOut(BIT_0);      // DRVB_SLEEP
    Port.E.SetPinsDigitalOut(BIT_0);      // DRVB_DIR
    Port.C.SetPinsDigitalOut(BIT_15);      // DRVB_STEP
    Port.D.SetPinsDigitalOut(BIT_3);      // DRVB_BIN1
    Port.D.SetPinsDigitalOut(BIT_1);      // DRVB_BIN2

    /* Setup  Input Drive B */
    Port.D.SetPinsDigitalIn(BIT_5);       // DRVB_FLT
//    Port.C.SetPinsDigitalIn(BIT_15);      // DRVB_STALL

    /* Setup  SPI SCn */
    Port.F.SetPinsDigitalIn(BIT_4);       // SDI
    Port.F.SetPinsDigitalOut(BIT_5);      // SD0
    Port.C.SetPinsDigitalOut(BIT_13);     // DRVB_SC
    Port.C.SetPinsDigitalOut(BIT_14);     // DRVA_SC
    Port.B.SetPinsDigitalOut(BIT_14);     // CLK

    Port.D.SetPinsDigitalIn(BIT_11);        // LIMIT_SW

    Port.G.SetPinsDigitalIn (BIT_9);        // U6RX
    Port.G.SetPinsDigitalOut(BIT_6);        // U6TX
}


//===========================
//	INIT UART
//===========================
void InitUart (void)
{

  UartConfig_t        oConfig       = UART_ENABLE_PINS_TX_RX_ONLY;
  UartFifoMode_t      oFifoMode     = UART_INTERRUPT_ON_TX_BUFFER_EMPTY | UART_INTERRUPT_ON_RX_NOT_EMPTY;
  UartLineCtrlMode_t  oLineControl  = UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1;

  Uart.Open(UART6, BAUD57600, oConfig, oFifoMode, oLineControl);   // Open UART 6 as : 9600 BAUD, 1 stop bit, no parity and 8 bits data, not used as main UART for Skadi
  Uart.EnableTx(UART6);
  Uart.EnableRx(UART6);

  Uart.ConfigInterrupt(UART6, UART6_INTERRUPT_PRIORITY, UART6_INTERRUPT_SUBPRIORITY);
  
}


//===========================
//	INIT CAN BUS
//===========================
void InitCan(void)
{
  Can.Initialize(CAN1, FALSE);
  Can.Initialize(CAN2, FALSE);
}


//===========================
//	INIT I2C
//===========================
void InitI2c(void)
{
  I2c.Open(I2C4, I2C_FREQ_400K);
}


//===========================
//	INIT WATCHDOG TIMER
//===========================
void InitWdt(void)
{
  Wdt.Enable();
}


//===========================
//	INIT ADC
//===========================
void InitAdc(void)
{
  // Mode of operation. Uncomment the one you need.
  //================================================
  UINT32 samplingClk = ADC_CLK_TMR;     // Timer3 used for sampling
//  UINT32 samplingClk = ADC_CLK_AUTO;    // Internal counter used for sampling
//  UINT32 samplingClk = ADC_CLK_INT0;    // Active transition on INT0 used for sampling
//  UINT32 samplingClk = ADC_CLK_MANUAL;  // Manual sampling.
  //================================================

  // Hardware config. These are exemples.
  //================================================
//  UINT32 configHardware = ADC_VREF_AVDD_AVSS      // Vref+ is AVdd and Vref- is AVss
//                        | ADC_SAMPLES_PER_INT_16; // 16 samples/interrupt
  UINT32 configHardware = ADC_VREF_EXT_AVSS       // Vref+ is External and Vref- is AVss
                        | ADC_SAMPLES_PER_INT_1; // 1 sample/interrupt => used in manual mode
  //================================================

  // Port config. These are exemples.
  //================================================
//  UINT32 configPort = ENABLE_ALL_ANA; // Enable AN0-AN15 in analog mode
//  UINT32 configScan = 0; // Scan all ANs
//  UINT32 configScan = SKIP_SCAN_ALL; // Skip all scans

  UINT32 configPort = ENABLE_AN2_ANA; // Enable AN0, AN4 and AN13 in analog mode
//
  UINT32 configScan = SKIP_SCAN_AN0
                    | SKIP_SCAN_AN1
                    | SKIP_SCAN_AN3
                    | SKIP_SCAN_AN4
                    | SKIP_SCAN_AN5
                    | SKIP_SCAN_AN6
                    | SKIP_SCAN_AN7
                    | SKIP_SCAN_AN8
                    | SKIP_SCAN_AN9
                    | SKIP_SCAN_AN10
                    | SKIP_SCAN_AN11
                    | SKIP_SCAN_AN12
                    | SKIP_SCAN_AN13
                    | SKIP_SCAN_AN14
                    | SKIP_SCAN_AN15; // Don't scan the channels that are not enabled by configPort
  //================================================

  // Open ADC with parameters above
  Adc.Open(samplingClk, configHardware, configPort, configScan);

  Adc.ConfigInterrupt(ADC_INTERRUPT_PRIORITY, ADC_INTERRUPT_SUBPRIORITY);

}


//===========================
//	INIT INPUT CAPTURE
//===========================
void InitInputCapture(void)
{
  // Capture every rising edge, 1 interrupt each capture, use the 16 bits Timer 2, capture the first rising edge, Input Capture ON
  UINT16 config = IC_EVERY_RISE_EDGE | IC_INT_1CAPTURE | IC_TIMER2_SRC | IC_CAP_16BIT | IC_FEDGE_RISE | IC_ON;

  // Capture every rising edge, 1 interrupt each capture, use the 32 bits Timer 23, capture the first rising edge, Input Capture ON
//  UINT16 config = IC_EVERY_RISE_EDGE | IC_INT_1CAPTURE | IC_CAP_32BIT | IC_FEDGE_RISE | IC_ON;

  InputCapture.Open(IC1, config);
  InputCapture.Open(IC3, config);

  InputCapture.ConfigInterrupt(IC1, IC1_INTERRUPT_PRIORITY, IC1_INTERRUPT_SUBPRIORITY);
  InputCapture.ConfigInterrupt(IC3, IC3_INTERRUPT_PRIORITY, IC3_INTERRUPT_SUBPRIORITY);
}


//===========================
//	START INTERRUPTS
//===========================
void StartInterrupts(void)
{

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Enable timer interrupts
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Timer.EnableInterrupt(TIMER_1);
//  Timer.EnableInterrupt(TIMER_2);
//  Timer.EnableInterrupt(TIMER_3);
//  Timer.EnableInterrupt(TIMER_4);
//  Timer.EnableInterrupt(TIMER_5);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Enable UART interrupts             // Not functionnal yet
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Uart.EnableRxInterrupts (UART6);  // Enable RX Interrupts for UART6
  Uart.DisableTxInterrupts(UART6);  // Disable TX Interrupts for UART6


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Enable SPI interrupts             // Not functionnal yet
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Spi.EnableRxInterrupts(SPI4);   // Enable RX Interrupts for SPI4
//  Spi.EnableTxInterrupts(SPI4);   // Enable TX Interrupts for SPI4


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Enable ADC interrupts
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Adc.EnableInterrupts();   // Works only when not in manual mode


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Enable InputCapture interrupts
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  InputCapture.EnableInterrupt(IC1);
//  InputCapture.EnableInterrupt(IC3);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Enable multi-vector interrupts
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
  INTEnableInterrupts();

}
