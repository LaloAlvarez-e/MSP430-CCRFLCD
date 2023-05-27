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

/**********************************************************************************/
/*  Function name: InitButtonLed               	                                  */
/*  	Parameters                                                                */
/*          Input   :  No       			                          */
/*          Output  :  No	                                                  */
/*	Action: Initialize Led and Button directions.                          	  */
/**********************************************************************************/
void InitButtonLed(void)
{
  // Enable button interrupt 
  P1DIR &= ~BIT7;
  P1REN |= BIT7;
  P1IES &= BIT7;
  P1IFG = 0;
  P1OUT |= BIT7;
  P1IE  |= BIT7;

  // Set up LED 
  P1OUT &= ~BIT0;
  P1DIR |= BIT0;
}


/****************************************************************************/
/*  Function name: RTC_Clock_init                                           */
/*  	Parameters                                                          */
/*          Input   :  No	                                            */
/*          Output  :  No	                                            */
/*	Action: Set up RTC clock operation                                  */
/****************************************************************************/
void RTC_Clock_init(void)
{
  P5OUT = 0x00;
  //P5DIR = 0x02;  // If this is uncommented RTC doesn't work!!!!!!!!!!!!!!!!!! P5DIR register must be set to 0x00!!!!!!
  P5SEL = 0x03; // Enabled alternative functions 
  
  _BIC_SR(OSCOFF);          // Enable the LFXT1 crystal oscillator
  UCSCTL6 &= (~XTS);        // Select Low-frequency mode. XCAP bits define the capacitance at the XIN and XOUT pins.
  UCSCTL6 |= (XCAP0);       // Set XCAP0
  UCSCTL6 |= (XCAP1);       // Set XCAP1
  // Then Qrystal Load Capacitance is:  
  //  (XCAP1=0),(XCAP0=0) -> 2pF     (XCAP1=0),(XCAP0=1) -> 5.5pF
  //  (XCAP1=1),(XCAP0=0) -> 8.5pF   (XCAP1=1),(XCAP0=1) -> 12pF
  UCSCTL6 &= (~XT1OFF);     // Turns on the XT1.
  
  // Loop until XT1,XT2 & DCO stabilizes
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
  
  UCSCTL4 = SELA__XT1CLK;   // ACLK source: XT1CLK
  SFRIFG1 &= (~OFIFG);      // Oscillator fault interrupt flag
  SFRIE1 |= (OFIE);         // Oscillator fault interrupt enable
  
}


/****************************************************************************/
/*  Function name: timer1_A3_init                                           */
/*  	Parameters                                                          */
/*          Input   :  No                                                   */
/*          Output  :  No	                                            */
/*	Action: Initialize Timer1_A3 operation                              */
/****************************************************************************/
//TIMER1 initialize - prescale:0 
// desired value:	2Hz, i.e. 32768/2 = 16384(0x4000)
void timer1_A3_init(void)
{
  TA1CTL = 0x0004;   // Timer1_A3 clear.
  TA1CCTL0 = 0x0010; // Timer1_A3 Capture/compare 0 interrupt enable.
  TA1CCR0 = 0x4000;  // Set TACCR0 value
  TA1CTL = 0x0110;   // Selected: ACLK, divider:0, Up mode.
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
 //RTC_Clock_init();
 ports_init();
 //timer1_A3_init();
 InitButtonLed();

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
  //LED_Togg;
}

/****************************************************************************/
/*  user NMI(UNMI) Non-Maskable interrupt service routine.                  */
/****************************************************************************/
#pragma vector=UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
  if(UCSCTL7 & XT2OFFG){          //XT2 oscillator fault flag
    UCSCTL7 &= (~XT2OFFG);        //Clear flag XT2OFFG
  }
  if(UCSCTL7 & XT1HFOFFG){        //XT1 oscillator fault flag (HF mode).
    UCSCTL7 &= (~XT1HFOFFG);      //Clear flag XT1HFOFFG
  }
  if(UCSCTL7 & XT1LFOFFG){        //XT1 oscillator fault flag (LF mode)
    UCSCTL7 &= (~XT1LFOFFG);      //Clear flag XT1LFOFFG
  }
  if(UCSCTL7 & DCOFFG){           //DCO fault flag.
    UCSCTL7 &= (~DCOFFG);         //Clear flag DCOFFG
  }
  SFRIFG1 &= (~OFIFG);    // Oscillator fault interrupt flag
}


/**********************************************************************************/
/*      PORT1, interrupt service routine.                                         */
/**********************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  switch(__even_in_range(P1IV, 4))
  {
    case  0: break;
    case  2: break;                         // P1.0 IFG
    case  4:
      break;                                // P1.1 IFG
    case  6: break;                         // P1.2 IFG
    case  8: break;                         // P1.3 IFG
    case 10: break;                         // P1.4 IFG
    case 12: break;                         // P1.5 IFG
    case 14: break;                         // P1.6 IFG
    case 16:
        LED_Togg;
        break;                         // P1.7 IFG
      
  }
}

