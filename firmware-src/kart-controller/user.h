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



// PIC16F690 Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>
#include <stdint.h>         /* For uint8_t definition */

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)


#define _XTAL_FREQ 4000000			// 4MHz. required for delay Routines. 

/*
 * SPI_CONFIG for NRF24 Chip
 * ======================
 * MISO - RB4 (7), UART Rx
 * MOSI - RC7 (8), UART Tx
 * SCK - RB6 (9) , Dig o/p
 * CSN - RC6 (10), Dig o/p
 * CE  - RC3 (11),  Dig o/p
 * IRQ - ?????
 * 
 * Hall Sensor I/p (Can be connected to comp. Or Dig i/p through external comp)
 * ===============
 * RA2 (12) - IOC
 * 
 * Trigger Switch:
 * ===============
 * RA1 (13) - IOC (weak pullup)
 * 
 * Notif LED:
 * ==========
 * RC5 (18)
 *  
 */

#define TRISMOSI 	TRISCbits.TRISC7		//SPI_MOSI    
#define TRISMISO 	TRISBbits.TRISB4		//SPI_MISO
#define TRISUART_RX	TRISBbits.TRISB5		//UART_RX
#define TRISUART_TX	TRISBbits.TRISB7		//UART_TX    
#define TRISCSK 	TRISBbits.TRISB6		//SPI_CLK
#define TRISCSN 	TRISCbits.TRISC6		//SPI_CSN
#define TRISCE  	TRISCbits.TRISC3		//SPI_CE
    
#define MOSI 	PORTCbits.RC7		//SPI_MOSI    
#define MISO 	PORTBbits.RB4		//SPI_MISO
#define UART_RX	PORTBbits.RB5		//UART_RX
#define UART_TX	PORTBbits.RB7		//UART_TX    
#define CSK 	PORTBbits.RB6		//SPI_CLK
#define CSN 	PORTCbits.RC6		//SPI_CSN
#define CE  	PORTCbits.RC3		//SPI_CE

#define SENSOR_IR       PORTAbits.RA3       // IR Sensor connected (???) (to use IOC)     //FIXME: CLOCK IN, can't be used in LP
#define SENSOR_BATT     PORTAbits.RA0       // Battery level ind connected (9v) (ANSEL=1) //FIXME: CLOCK OUT, can't be used in LP
#define SENSOR_HALL  	PORTAbits.RA2		//Hall sensor input extINT available
#define KEY_1           PORTAbits.RA1		//Push Switch Key_1
        
#define IND_LED PORTCbits.RC5       // Indicator LED (OUT)
    
/*********TMR1 Time macros*********/
    /* The Fixed Frame beacon format uses Modified NEC code */
/* |---4MsPulse---|---2.5MsGap|--562uS(1)--|--562uS(0)--|--562uS(1)--|--1682uS(0)--|--562uS(1)--|*/
/* |--------Header------------|----------1--------------|------------0-------------|--Trailer---|*/  
/*
 * Each timer tick is 64uS. TMR0 is 8Bit MAX = 255
 * 20% signal Tolerance
 */    
#define TMR4mS   192      // 4MS, 62.5 Ticks, Preset TMR REg to Max - 62.5 => 255 - 63 
#define TMR2mS5  215      // 2.5Ms, 39.06 Ticks = 215     
#define TMR562uS 246           // 562uS (~564uS) = 8.96 => TMR0 = 246ticks, 
#define TMR1mS6  228           // 1.68Ms    = 26.25 Ticks, => 228
#if 0
    /* +10% */
#define TMR4mSHI   186      // 4.4MS, 68.75 Ticks, Preset TMR REg to Max - 69 => 186
#define TMR2mS5HI  212      // 2.75Ms, ~43 Ticks      
#define TMR562uSHI 245           // 620uS  = 10ticks, 
#define TMR1mS6HI  226           // 1.80Ms    = 29 Ticks, 

    /* -10% . Lower threshold = TMR1 Start + lo Ticks */ 
#define TMR4mSLO   242      // 3.6MS, 56 Ticks, Preset TMR REg to Max + 56 => 186 + 56 = 242   
#define TMR2mS5LO  247      // 2.25Ms, 35 Ticks = 247
#define TMR562uSLO 252           // 505uS (~564uS) = 7.9ticks, 
#define TMR1mS6LO  249          // 1.53Ms    = 23.9 Ticks, 
#endif
    /* +20% */
#define TMR4mSHI   180      // 4.8MS, 75 Ticks, Preset TMR REg to Max - 75 => 180
#define TMR2mS5HI  208      // 3Ms, ~47 Ticks      
#define TMR562uSHI 244           // 674.4uS  = 11ticks, 
#define TMR1mS6HI  223           // 2.02Ms    = 32 Ticks, 

    /* -20% . Lower threshold = TMR1 Start + lo Ticks */
#define TMR4mSLO   230      // 3.2MS, 50 Ticks, Preset TMR REg to Max + 50 => 180 + 50 = 230   
#define TMR2mS5LO  239      // 2.0Ms, 31 Ticks = 239
#define TMR562uSLO 251           // 449uS = 7.01ticks, 
#define TMR1mS6LO  244          // 1.344Ms    = 21 Ticks, 
#define TMR1mS6LO562  230          // 1.344Ms    = 21 Ticks,     
    
/*********TMR1 Time macros*********/
	
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */

void handleSwitch(void);

void handleIRSignal(void);

void blinkLed(unsigned short blinks);

uint8_t read_battery_level (void);

#if 0
#ifndef TMR1RESTART
#define TMR1RESTART(tmrL, tmrH) \
            do{          \
                TMR1L = tmrL; \
                TMR1H = tmrH;  \
                PIR1bits.TMR1IF  = 0; \
                PIE1bits.TMR1IE  = 1; \
                T1CONbits.TMR1ON = 1; \
            } while(0);//           -->> On
#endif        
#endif
#ifdef	__cplusplus
}
#endif

#define MAX_PAYLOAD_COUNT 8
extern volatile uint8_t     data_valid_bitmap;
#define data_set(x) (data_valid_bitmap |= (1<<(x)))
#define data_unset(x) (data_valid_bitmap &= ~(1<<(x)))
#define is_data_valid(x) (data_valid_bitmap & (1<<(x)))
#define data_ready() (data_valid_bitmap)
#define data_q_flush() (data_valid_bitmap = 0)

typedef enum gim_detect_type_ {
     IR = 0,
     HALL
}gim_detect_type;

typedef struct gim_timeval_ {
    uint16_t sec;       // 1/2sec epoch counter
    uint8_t  m_sec;     // 2mS counter
}gim_timeval;

typedef struct gim_kart_data_ {
    gim_timeval    time;    
    uint8_t     battery_level;
    uint8_t     dev_id        :5;
    uint8_t     detect_type   :1;
    uint8_t     detect_code   :2;   //Used for IR code, section detection
    uint8_t     lap_count     :6;   // supports max 64 laps before wraparound
    uint8_t     seed          :2;   //randomness. To avoid h/w dropping the packet in case of resend
}kart_data_t;

#endif	/* USER_H */

