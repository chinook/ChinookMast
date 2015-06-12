//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Chinook Project Template
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// File    : Interrupts.h
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


#ifndef __INTERRUPT_H__
#define	__INTERRUPT_H__

#include "Setup.h"

//==============================================================================
// DEFINITIONS
//==============================================================================

/*
 *  ==========================================
 *            Table of interrupts
 *  ==========================================
 * 
 * +--------------------------------------------+
 * | Priority | Subpriority | Interrupt handler |
 * |----------+-------------+-------------------|
 * | 1        | 0           | Input Capture 2   |
 * | 1        | 1           | Input Capture 4   |
 * | 1        | 2           | Input Capture 1   |
 * | 1        | 3           | Input Capture 3   |
 * |----------+-------------+-------------------|
 * | 2        | 0           |                   |
 * | 2        | 1           |                   |
 * | 2        | 2           |                   |
 * | 2        | 3           | Timer 1           |
 * |----------+-------------+-------------------|
 * | 3        | 0           | Timer 2           |
 * | 3        | 1           | Timer 3           |
 * | 3        | 2           |                   |
 * | 3        | 3           | CAN 1             |
 * |----------+-------------+-------------------|
 * | 4        | 0           |                   |
 * | 4        | 1           |                   |
 * | 4        | 2           |                   |
 * | 4        | 3           | SPI 4             |
 * |----------+-------------+-------------------|
 * | 5        | 0           |                   |
 * | 5        | 1           |                   |
 * | 5        | 2           | I2C 4             |
 * | 5        | 3           |                   |
 * |----------+-------------+-------------------|
 * | 6        | 0           |                   |
 * | 6        | 1           |                   |
 * | 6        | 2           |                   |
 * | 6        | 3           | UART 6            |
 * |----------+-------------+-------------------|
 * | 7        | 0           |                   |
 * | 7        | 1           |                   |
 * | 7        | 2           |                   |
 * | 7        | 3           |                   |
 * +----------+-------------+-------------------+
 * 
 */

//===============================================
// Timer interrupts priorities and subpriorities
//===============================================
#define TIMER1_INTERRUPT_PRIORITY       2         // Used in ChinookLib function
#define T1_INTERRUPT_PRIORITY           ipl2auto  // Used in ISR
#define TIMER2_INTERRUPT_PRIORITY       3         // Used in ChinookLib function
#define T2_INTERRUPT_PRIORITY           ipl3auto  // Used in ISR
#define TIMER3_INTERRUPT_PRIORITY       3         // Used in ChinookLib function
#define T3_INTERRUPT_PRIORITY           ipl3auto  // Used in ISR

#define TIMER1_INTERRUPT_SUBPRIORITY    3         // Highest subpriority
#define TIMER2_INTERRUPT_SUBPRIORITY    0
#define TIMER3_INTERRUPT_SUBPRIORITY    1         // Highest subpriority
//===============================================


//===============================================
// UART interrupts priorities and subpriorities
//===============================================
#define UART6_INTERRUPT_PRIORITY        6           // Used in ChinookLib function
#define U6_INTERRUPT_PRIORITY           ipl6auto    // Used in ISR

#define UART6_INTERRUPT_SUBPRIORITY     3           // Lowest subpriority
//===============================================


//===============================================
// SPI interrupts priorities and subpriorities
//===============================================
#define SPI4_INTERRUPT_PRIORITY         4           // Used in ChinookLib function
#define S4_INTERRUPT_PRIORITY           ipl4auto    // Used in ISR

#define SPI4_INTERRUPT_SUBPRIORITY      3           // Lowest subpriority
//===============================================


//===============================================
// Input Capture interrupts priorities and subpriorities
//===============================================
#define IC1_INTERRUPT_PRIORITY          1           // Used in ChinookLib function
#define IC1_INT_PRIORITY                ipl1auto    // Used in ISR
#define IC2_INTERRUPT_PRIORITY          1           // Used in ChinookLib function
#define IC2_INT_PRIORITY                ipl1auto    // Used in ISR
#define IC3_INTERRUPT_PRIORITY          1           // Used in ChinookLib function
#define IC3_INT_PRIORITY                ipl1auto    // Used in ISR
#define IC4_INTERRUPT_PRIORITY          1           // Used in ChinookLib function
#define IC4_INT_PRIORITY                ipl1auto    // Used in ISR

#define IC1_INTERRUPT_SUBPRIORITY       2
#define IC2_INTERRUPT_SUBPRIORITY       0
#define IC3_INTERRUPT_SUBPRIORITY       3
#define IC4_INTERRUPT_SUBPRIORITY       1
//===============================================


//===============================================
// CAN interrupts priorities and subpriorities
//===============================================
#define CAN1_INTERRUPT_PRIORITY         3           // Used in ChinookLib function
#define CAN1_INT_PRIORITY               ipl3auto    // Used in ISR

#define CAN1_INTERRUPT_SUBPRIORITY      3           // Highest subpriority
//===============================================


//===============================================
// I2C interrupts priorities and subpriorities
//===============================================
#define I2C4_INTERRUPT_PRIORITY         5           // Used in ChinookLib function
#define I2C4_INT_PRIORITY               ipl5auto    // Used in ISR

#define I2C4_INTERRUPT_SUBPRIORITY      2
//===============================================


//==============================================================================
// VARIABLES
//==============================================================================

#endif	/* __INTERRUPT_H__ */

