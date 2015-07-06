/* 
 * File:   interrupts.c
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

volatile unsigned short devId  = 0x0;
volatile unsigned short comp  = 0x0;
volatile unsigned short cfgMode= 0x0;
volatile unsigned short tmr1IntCount = 0x0;
//unsigned short tmr0IntCount = 0x0;

void interrupt HiIsr(void)		
{   
    if (INTCONbits.RAIE && INTCONbits.RAIF)	{	// check the interrupt on change flag
        INTCONbits.RAIE = 0;
		__delay_ms(200);						//check for key debounce
		if(!KEY_4) {
            //IND_LED ^=1;                // Toggle the LED
            handleSwitch();
		}
            /*
			//sendFrame(ADDRESS,DATA_KEY_1);	//send the frame
	 		while(!KEY_1)					//if the key is still pressed 
			{
			//sendRepeate();					//send Repeate codes. 
			}	
		}else if(!KEY_2) */					//check if the Key_2 is pressed

       
		if(PORTA) {asm("nop");}             //this is requited to end the mismatch condition 
        INTCONbits.RAIF = 0;                // clear the interrupt on chage flag
        INTCONbits.RAIE = 1;
	}
         
    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
        // Fosc = 8Mhz/4. PS = 8 => 250000. TMR1 is 16bit (65535 ticks per Interrupt)
        // ~4 interrupts/ Sec. 5 sec = 20 ints
        if (tmr1IntCount < 20) {
            tmr1IntCount++;
        } else {
            GIE  = 0;              // Global interrupt disable
            T1CONbits.TMR1ON = 0;  //           -->> OFF TIMER1
            IND_LED          = 0;  // Turn OFF LED
            /* Test */
            //IND_LED ^=1;            
            /* End Test */
                        
            tmr1IntCount = 0;     // 5Sec
            cfgMode      = 0;
            /* COnfig timeout. Save the config devInt to EEPROM */
            devId &= 0x03;
            eeprom_write (0x00, devId);
            /* blink the Leds DevID numbers to give feedback of programmed devId*/
            __delay_ms(500);
            blinkLed(devId + 1);
            //comp = (devId << 4)|((~devId)&0x0F);
            //comp = comp|((~devId)&0x0F);
            GIE  = 1;            // Global interrup enable
        }
        
        PIR1bits.TMR1IF = 0; // clear flag           
    }

#if 0    
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        //TMR0 = 2;
        //OPTION_REGbits.PS   = 7;
        
        if (tmr0IntCount < 30) {
            tmr0IntCount++;
        } else {
            /* Test */
            IND_LED ^=1;
            /* End Test */
            
            tmr0IntCount = 0;     // 5Sec
            cfgMode      = 0;
            /* COnfig timeout. Save the config devInt to EEPROM */
            //eeprom_write (0x00, devId);
            
        }
        
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
    }
#endif
}

/* Handle switch event
 * 1. On Feedback Led
 * 2. Check Switch event
 *  a. Long press > 5Sec (Enter Device pgm mode)
 *  b. Short press < 1Sec(program device id)
 */
void handleSwitch()
{
    unsigned short cfgModeCounter = 0x0; // inc counter every 10Ms, 500 = 5Sec
                       
    if (cfgMode) {
        /* If in programming mode, increment devId */
        IND_LED = 0;
        devId++;             
        while (!KEY_4){__delay_ms(20);};
        
        /* Restart Timer1 for 5Sec*/
        TMR1RESTART ();
        __delay_ms (200);
        IND_LED = 1;
    } else {
        do {
            if ((cfgModeCounter >= 250) && !cfgMode) {
                cfgMode = 1;    // Enter config mode
                devId   = 0;    // reset the device Id
                IND_LED = 1;
            } else {
                cfgModeCounter++;
                __delay_ms(20);
            }      
            
            if (cfgMode) {                              
                /* Start the 5Sec timer1 to unset config mode*/
                TMR1RESTART ();
            }
        } while (!KEY_4);
    }     
    //IND_LED = 0;        // turn OFF led     
    return;
}