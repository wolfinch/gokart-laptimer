/* 
 * File:   user.c
 * Author: Joshith
 *
 * Created on 23 March, 2015, 11:13 AM
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    /* Initialize port states*/
    TRISC       = 0x00;      // On PortC, all are o/p
    PORTC       = 0x00;      // PortC, all Pin states set to down      
    TRISA       = 0x08;      // on PortA, set all o/p exect RA3 (which in default i/p - MCLR)         
    PORTA       = 0x00;      // Set the init Pin Values    
    WPU         = 0x00;      // Weak pullup disabled for all pins in PORT-A
    
    /* Initialize OSC */
    OSCCONbits.IRCF0    = 1;
    OSCCONbits.IRCF1    = 1;  // Set OSC freq to 8MHz
    OSCCONbits.IRCF2    = 1;
    
    CMCON0bits.CM   = 0x7;  // Comparators off pins in Digital I/O
    ANSEL           = 0x00;	// all pin are Digital

	OPTION_REG      = 0x1F; //RA int on falling edge. 0x5F;		//pullups are enabled	
							//timer0 clock source is internal
							//timer0 perscaller is 1:1 (disabled "assigned to WDT")
#if 0
    T2CON        	= 0x04;	// prescaler 1:1 (Timer2 is required BY the ECCP module to generate 38Khz Frequency) 
							// Timer2 is on	
							// postscaller 1:1
    PR2 			= 51;	//CCP module period register to generate 38Khz signal
    CCPR1L			= 0x1A;	//CCP module comprison register to set 50% duty for 38KHz signal	
	CCP1CON 		= 0x20;	//set Two LSB of PWM Duty 8MSB are in CCPR1L the module will turned on later
#endif
        /* PWM Logic*/

    /*
    * PWM registers configuration
    * Fosc = 8000000 Hz
    * Fpwm = 37735.85 Hz (Requested : 38000 Hz)
    * Duty Cycle = 50 %
    * Resolution is 7 bits
    * Prescaler is 1
    * Ensure that your PWM pin is configured as digital output
    * see more details on http://www.micro-examples.com/
    * this source code is provided 'as is',
    * use it at your own risks
    */
#if 0
    PR2 = 0b00110100 ;
    T2CON = 0b00000100 ;
    CCPR1L = 0b00011010 ;
    CCP1CON = 0b00010000 ; //set Two LSB of PWM Duty 8MSB are in CCPR1L the module will turned on later (LSB 4)
#endif
    /*
 * PWM registers configuration
 * Fosc = 8000000 Hz
 * Fpwm = 38461.54 (Requested : 38000 Hz)
 * Duty Cycle = 50 %
 * Resolution is 7 bits
 * Prescaler is 1
 * Ensure that your PWM pin is configured as digital output
 * see more details on http://www.micro-examples.com/
 * this source code is provided 'as is',
 * use it at your own risks
 */
PR2 = 0b00110011 ;
T2CON = 0b00000100 ;
CCPR1L = 0b00011001 ;
CCP1CON = 0b00110000 ;
    /* PWM Logic End */
 
#if 0
    // Let's not use TMR0 now. Got TMR1 working, which is 16bit
    /* TMR0 COnfig*/
    OPTION_REGbits.T0CS = 0; // TImer mode
    OPTION_REGbits.PSA  = 0; // Prescaler used by Timer
    OPTION_REGbits.PS   = 7; // Prescaller 1:256
    INTCONbits.T0IE     = 1; // Enable interrupt on overflow
    /* TMR0 config end*/
#endif    
    
    /* TMR1 Config*/
    T1CONbits.TMR1CS = 0; // Internal clock source
    TMR1L = 0;
    TMR1H = 0;
    T1CONbits.T1CKPS = 3; // Prescaler = 0b11 ie. F/8
    PIR1bits.TMR1IF  = 0;   // Clear the flag
    T1CONbits.TMR1ON = 0;  //           -->> OFF, turn-on from switch handler
    PIE1bits.TMR1IE  = 1; // TMR1 interrupt enable
    /* TMR1 Config end*/

    /* Interrupt config*/
    IOC 			= 0x08; //interrupt on change is enabled on RA3
	INTCONbits.RAIE = 1;	// external interrupt on GPIO3 pin(4) is enabled
	INTCONbits.RAIF = 0;	// clear the external interrrupt flag	
    INTCONbits.PEIE = 1;    // peripheral intrrupt enable
    INTCONbits.GIE 	= 1;    // GLOBL interrupt enable

}

/* Modified NEC format. Reduced header time from 9ms to 6ms, abandoned command. 
 * To keep the pulse duration within ~30 Ms
 */
void sendFrame(unsigned char address, unsigned char command) // this routine send the whole frame including 9ms leading pulse 4.5ms space address ~address command ~command end of message bit.
{	
	TMR2 = 0x00;					//clear the TMR2 register before we start generating 38Khz Signal on the GPIO
	CCP1CONbits.CCP1M = 0xC;		//put The CCP module into PWM mode , the Duty is 50% and freqeucny is 38Khz as allredy set, 
	__delay_us(3999);				//wait for ~9ms this rountine will delay for 8.999 ms + (3*500ns) = ~9ms (500ns is the instruction execution time at 8Mhz) and next instruciton will take 3 ins cycle to execute.
	CCP1CONbits.CCP1M = 0x0;		//turn off the CCP module stop generating 38Khz singal
	__delay_us(2490);				//wait for ~4.5ms  the value 4490 is compensated with the next instrucitons execution timing , it helps to keep precise timing. as described avobe
	
	sendByte(address);				//send address byte. (sendByte functions should not be called independently, only sendFrame should call it)
	//sendByte(~address);				//send address logical invert 
	//sendByte(command);				//send command
	//sendByte(~command);				//send command logical invert
	
									//addres and command is sent now send the end of message bit
	TMR2 = 0x00;					//clear the TMR2 register before we start generating 38Khz Signal on the GPIO
	CCP1CONbits.CCP1M = 0xC;		//Start generating 38Khz singal
	__delay_us(561);				//wait for ~562.5us again value 561 is compensated with the next instrucitons timing
	CCP1CONbits.CCP1M = 0x0;		//stop generating 38Khz singal.
	__delay_us(4490);					// wait for the Data Frame time. 
}

#if 0
void sendByte(unsigned char byte) {
    
    /* First Bit: 1*/
    TMR2 = 0x00; //clear the TMR2 register before we start generating 38Khz Signal on the GPIO
    CCP1CONbits.CCP1M = 0xC; //Start generating 38Khz singal
    __delay_us(561); //wait for ~562.5us again value 561 is compensated with the next instrucitons timing
    CCP1CONbits.CCP1M = 0x0; //stop generating 38Khz singal
    __delay_us(558); //wait for ~562.5us again value 558 is compensated with the next instrucitons timing

    /* Second bit: 0*/
    TMR2 = 0x00; //clear the TMR2 register before we start generating 38Khz Signal on the GPIO
    CCP1CONbits.CCP1M = 0xC; //Start generating 38Khz singal
    __delay_us(561); //wait for ~562.5us again value 561 is compensated with the next instrucitons timing
    CCP1CONbits.CCP1M = 0x0; //stop generating 38Khz singal
    __delay_us(1686); //delay for ~1.6875 ms again value 1686 is compensated with the next instrucitons timing

}

#else
void sendByte(unsigned char byte)	// this function is called only by the sendFrame , to send each byte of data total 4bytes.
{
	unsigned char i;				//variable to hold the counter vlaue
	for(i=2 ;i>0;i--)				// Sending 2bits here
	{	
		TMR2 = 0x00;				//clear the TMR2 register before we start generating 38Khz Signal on the GPIO
		CCP1CONbits.CCP1M=0xC;		//Start generating 38Khz singal
		__delay_us(561);			//wait for ~562.5us again value 561 is compensated with the next instrucitons timing
		CCP1CONbits.CCP1M=0x0;		//stop generating 38Khz singal
			
		if(byte & 0x02)				// we send MSB first order. 
		{							// check for MSB bit if it is 1 then delay for 1.6875ms if it is zero then delay for 562.5us
		__delay_us(1686);			//delay for ~1.6875 ms again value 1686 is compensated with the next instrucitons timing
		}
		else
		{
		__delay_us(558);			//wait for ~562.5us again value 558 is compensated with the next instrucitons timing
		}
		byte = byte <<1;			//get the next lsb into msb (shift left the byte)
	}	
}
#endif

void sendRepeate()
{
	TMR2 = 0x00;					//clear the TMR2 register before we start generating 38Khz Signal on the GPIO
	CCP1CONbits.CCP1M = 0xC;		//Start generating 38Khz singal
	__delay_us(8999);				//wait for ~9ms 	
	CCP1CONbits.CCP1M = 0x0;		//stop generating 38Khz singal
	__delay_us(2245);				//wait for 2.25ms

	TMR2 = 0x00;					//clear the TMR2 register before we start generating 38Khz Signal on the GPIO
	CCP1CONbits.CCP1M = 0xC;		//Start generating 38Khz singal
	__delay_us(556);				//wait for ~562.5us
	CCP1CONbits.CCP1M = 0x0;		//stop generating 38Khz singal	
	__delay_us(96187);				//delay for 96.187 ms before sending the next repeate code
	
}

/* Blink the LED 'blinks' times .5Sec Gap*/
void blinkLed(unsigned short blinks)
{
    short i = 0;
    
    if(!blinks) return;
    
    // turn off LED
    IND_LED = 0;
    for(i =(blinks <<1); i>0; i--) {
        IND_LED ^= 1;       // Toggle LED
        __delay_ms(500);    // Delay .5S
    }
}

