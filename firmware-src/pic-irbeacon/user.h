/* 
 * File:   user.h
 * Author: joshith
 *
 * Created on 30 March, 2015, 5:33 PM
 */

#ifndef USER_H
#define	USER_H

#ifdef	__cplusplus
extern "C" {
#endif
    /******************************************************************************/
    /* User Level #define Macros                                                  */
    /******************************************************************************/

    // #pragma config statements should precede project file includes.
    // Use project enums instead of #define for ON and OFF.

    // CONFIGURATION BITS
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#define _XTAL_FREQ 8000000			// required for delay Routines. 
    /*
     * 
     */

#define IR_LED 	PORTCbits.RC5		//IR LED is conntected to this port
#define IND_LED PORTCbits.RC4       // Indicator LED

    //#define KEY_1	PORTAbits.RA0		//Switces pin definition 
    //#define KEY_2	PORTAbits.RA1
    //#define KEY_3	PORTAbits.RA2
#define KEY_4	PORTAbits.RA3
    //#define KEY_5	PORTAbits.RA4
    //#define KEY_6	PORTAbits.RA4

#define ADDRESS 0x01				//NEC protocol address for the Data Frame (MSB first)

#define DATA_KEY_1	0xF8			//Data For KEY_1 (MSB first)
#define DATA_KEY_2	0x50			//Data For KEY_2 (MSB first)
#define DATA_KEY_3	0xD8			//Data For KEY_3 (MSB first)
#define DATA_KEY_4	0xB0			//Data For KEY_4 (MSB first)
#define DATA_KEY_5	0x30			//Data For KEY_5 (MSB first)


    /******************************************************************************/
    /* User Function Prototypes                                                   */
    /******************************************************************************/

    /* TODO User level functions prototypes (i.e. InitApp) go here */

    void InitApp(void); /* I/O and Peripheral Initialization */

    void sendFrame(unsigned char, unsigned char); //sendFrame(address,data) sends the whole IR frame
    void sendByte(unsigned char); //sendByte(byte) this function is used by the sendFrame to send 4 indivisual btyes
    //SHOULD NOT BE CALL FROM ANY WERE ELSE except the sendFrame Function.
    void sendRepeate(); //Sends the Repeate Code after Message Frame.				

    void handleSwitch();

    void blinkLed(unsigned short blinks);

#ifndef TMR1RESTART
#define TMR1RESTART() \
            do{          \
                tmr1IntCount = 0; \
                TMR1L = 0; \
                TMR1H = 0;  \
                T1CONbits.T1CKPS = 3; \
                PIR1bits.TMR1IF  = 0; \
                PIE1bits.TMR1IE  = 1; \
                T1CONbits.TMR1ON = 1; \
            } while(0)//           -->> On
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* USER_H */

