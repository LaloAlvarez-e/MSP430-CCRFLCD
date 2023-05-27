/****************************************************************************/
/*    Demo program for: 						    */
/*	  Board: MSP430-CCRF                                                */
/*    Manufacture: OLIMEX                                                   */
/*	  COPYRIGHT (C) 2011                                                */
/*    Designed by:	Penko Todorov Bozhkov                               */
/*    Module Name:  main                                                    */
/*    File   Name:  main.c		                	            */
/*    Revision:		initial          	   		            */
/*    Date:  		07.06.2011		                            */
/*    Built with IAR Embedded Workbench C/C++ Compiler for MSP430 4.21.2    */
/****************************************************************************/
// Target : CC430F5137
#include <cc430x513x.h>
#include  <intrinsics.h>


//Definitions
#define LED_On			P1OUT |= 0x01;		P1DIR |= 0x01;
#define LED_Off		        P1OUT &= (~0x01);	P1DIR |= 0x01;
#define LED_Togg		P1OUT ^= 0x01;		P1DIR |= 0x01;
#define LED_Chk		        (P1IN & 0x01)


/****************************************************************************/
/*  Function name: delay                                                    */
/*  	Parameters                                                          */
/*          Input   :  p	                                            */
/*          Output  :  No	                                            */
/*	Action: Simple delay                                                */
/****************************************************************************/
void delay(volatile unsigned long p){
	while(p){p--;}
}


/****************************************************************************/
/*  Function name: ports_init                                               */
/*  	Parameters                                                          */
/*          Input   :  No	                                            */
/*          Output  :  No	                                            */
/*	Action: Initialize all Port's directions and states                 */
/****************************************************************************/
void ports_init(void)
{
  P1OUT = 0x00;
  P1DIR = 0x01; // Led out
  P2OUT = 0x00;
  P2DIR = 0x00;
  P3OUT = 0x00;
  P3DIR = 0x00;
}


/****************************************************************************/
/*  Function name: timer1_A3_init                                           */
/*  	Parameters                                                          */
/*          Input   :  No                                                   */
/*          Output  :  No	                                            */
/*	Action: Initialize Timer1_A3 operation                              */
/****************************************************************************/
//TIMER1 initialize - prescale:8 => 
// desired value:	2Hz, i.e. 1000000/8/62500(0xF424)
void timer1_A3_init(void)
{
  TA1CTL = 0x0004;   // Timer1_A3 clear.
  TA1CCTL0 = 0x0010; // Timer1_A3 Capture/compare 0 interrupt enable.
  TA1CCR0 = 0xF424;  // Set TACCR0 value
  TA1CTL = 0x02D0;   // Selected: SMCLK, divider:8, Up mode.
}


/****************************************************************************/
/*  Function name: init_devices                                             */
/*  	Parameters                                                          */
/*          Input   :  No	                                            */
/*          Output  :  No	                                            */
/*	Action: Initialize all peripherals                                  */
/****************************************************************************/
void init_devices(void)
{
 //1. Stop errant interrupts until set up
 _BIC_SR(GIE); // Disable interrupts during initialization process
 //2. Init System Clock
 // By defaut the FLL stabilizes MCLK and SMCLK to 1.048576 MHz and fDCO = 2.097152 MHz.

 //3. Init Peripherals
 ports_init();
 timer1_A3_init();

 _BIS_SR(GIE); // Global Interrupt enabled. Do this at the END of the initialization!!!!!!!!
 //all peripherals are now initialized
}


int main(void){
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  init_devices();
  
  while(1){
    
    asm("NOP");
    
  }
  
}


/****************************************************************************/
/*  Timer1_A3 CC0 interrupt service routine.                                */
/****************************************************************************/
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_Capture_Compare_ISR(void)
{
  LED_Togg;
}


