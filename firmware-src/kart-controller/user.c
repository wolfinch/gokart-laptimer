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
#include "nrf24.h"
#include "util/xprintf.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

extern volatile uint8_t devId;


void
UART_Init(const long int baudrate)
{
#if 0
  unsigned int x;
  x = (_XTAL_FREQ - baudrate*64)/(baudrate*64); //SPBRG for Low Baud Rate
  if(x>255) //If High Baud Rate required
  {
    x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); //SPBRG for High Baud Rate
    BRGH = 1; //Setting High Baud Rate
  }
  if(x<256)
  {
#endif
  
    BRGH  = 0;  // To support 9600@4Mhz
    BRG16 = 0;
    SYNC = 0; //Selecting Asynchronous Mode
    SPBRG = 25; //Writing SPBRG register
    SPEN = 1; //Enables Serial Port
    //TRISC7 = 1;  
    //TRISC6 = 1; 
    //CREN = 1; //Enables Continuous Reception
    TXEN = 1; //Enables Transmission
 //   return 1;
 // }
//  return 0;
}

void UART_Write(char data) {
    while (!TXIF); //Waiting for Previous Data to Transmit completly
    TXREG = data; //Writing data to Transmit Register, Starts transmission
}

    /* init the xprintf library */
void init_xdev_lib() {
    
    // Init UART
    UART_Init (9600);
    
    /* Reg hook for UART Tx*/
    xdev_out (UART_Write);
}

#if 0
char UART_Data_Ready() {
    return RCIF;
}

char UART_Read() {
    while (!RCIF); //Waits for Reception to complete
    return RCREG; //Returns the 8 bit data
}
#endif

/* ------------------------------------------------------------------------- */
void nrf24_setupPins()
{
    CE  = 0; // CE output
    CSN = 1; // CSN active-low
   // set_bit(DDRA,1); // CSN output
   // set_bit(DDRA,2); // SCK output
   // set_bit(DDRA,3); // MOSI output
   // clr_bit(DDRA,4); // MISO input
}
/* ------------------------------------------------------------------------- */
void nrf24_ce_digitalWrite(uint8_t state)
{
    CE = state;
}

inline void nrf24_csn_digitalWrite(uint8_t state)
{
    CSN = state;
}

nrf24_mod_init() {
    // PRX
    //uint8_t tx_mac[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    //uint8_t rx_mac[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    // PTX
    //uint8_t rx_mac[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    //uint8_t tx_mac[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


    //uint8_t tx_mac[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};
    uint8_t     rx_mac[5] = {"Node"};
    uint8_t     tx_mac[5] = {"Base0"};
    
    rx_mac[4]   =devId+48;
    /* SPI config*/
    TRISMOSI 	=   0;		//SPI_MOSI    
    TRISMISO 	=   1;		//SPI_MISO
    TRISCSK 	=   0;
    TRISCSN 	=	0;	//SPI_CSN
    TRISCE  	=	0;	//SPI_CE
    SSPCONbits.CKP = 0; // Idle clock is low (need to check??)
    SSPCONbits.SSPM = 0x1; // SPI Master SCK = Fosc/16
    SSPSTATbits.SMP = 1; // (0) i/p data sampled at the middle
    SSPSTATbits.CKE = 1; // data xmit on rising edge of SCK
    SSPCONbits.SSPEN = 1; // SPI enable
    /* SPI config*/

    /* initializes hardware pins */
    nrf24_init();

    /* RF channel: #10 , payload length: 5 */
    nrf24_config(10, 5);

    /* Set the module's own address */
    nrf24_rx_address(rx_mac);

    /* Set the transmit address */
    nrf24_tx_address(tx_mac);
}

uint8_t 
read_battery_level (void) {
    GO_DONE = 1 ;        // Start ADC conversion for Channel AN3, connected to 9V bat
    // wait for ADC to complete
    while (GO_DONE);

    ADIF    = 0; // clear flag
    // Read ADC. Interested in Hi 8 bits (ADRESH)
    return ADRESH;  // Do logic to convert to readable battery level
}

void ADCInit () {
    ANS3  = 1;          //Setting AN3 analog

   // ADCS    = 0b001;      // ADC Clock Fosc/8 - 2uS conversion Time
    ADCON1    = 0b00010000; //ADCS    = 0b010;
   // ADFM    = 0;        // MSB 7 bits in ADRESH   
   // ADON    = 1;        // ADC ON/OFF
    ADCON0 = 0b00001101; // ADFM =0, VCFG =0, CHS=0011(AN3), GO|ADON = 01
    ADIF   = 0;          // Clear Flag indicating conversion.    

}

void HallInit () {
    /* HALL connected to RA2 for ext INT*/
    ANS2    = 0;        //OFF
    TRISA2  = 1;        //i/p
    INTEDG = 0;    // Falling edge Trigger
    INTCONbits.INTF = 0;
    INTCONbits.INTE = 1; // External int enable on RA2
}

void
IrInit(void) {
    ANS3  = 0;
    TRISA3= 1; //i/p
    IOCA3 = 1; // IOC for RA3, IR_IN
}

void InitApp(void) {
    /* Set clock freq of Int Osc*/
    OSCCONbits.IRCF = 0x6; //4MHz Internal

    /* Turn OFF unwanted Modules*/
    ANSEL = 0b00000000; // Analog OFF except AN3 (RA4))
    ANSELH = 0;
    CCP1CONbits.CCP1M = 0x0; // CCP module is off pins in Digital I/O
    C1ON = 0; // COmparators are off
    C2ON = 0;
        //ADON = 0; // ADC OFF
    ADCInit (); // Init ADC module, Used for battery level indicator
    //VRCONbits.VREN      = 0;    // Vref Module OFF
    //VRCONbits.VROE      = 0;    // Vref Module OFF
    /* Turn OFF unwanted Modules*/

    OPTION_REGbits.nRABPU = 0; // Weak pullup enable for all i/p pins in PORT-A/B   
    OPTION_REGbits.INTEDG = 1;
    /* Initialize port states*/
    TRISB = 0b10010000; // On PortB, RB6(SPI) must be clear.
    TRISA = 0b00111101; // on PortA, set all o/p except RA0, RA2, RA4, RA5 (RA3 is always2, i/p only)
    TRISC = 0b00000000; //bit 7 must be clear for SPI (SDO), RC6 - CSN


    PORTB = 0b11000000;
    PORTA = 0b00000101; // Set the init Pin Values    
    WPUAbits.WPUA0 = 1; // WPU enabled on RA0 KEY_1
    WPUAbits.WPUA5 = 1; // WPU RA5
    IOCA0 = 1; // IOC enabled on RA0
    IOCA5 = 1; // IOC for RA5
    
    IrInit();
    HallInit();

    /* Init Xdev Lib for xprintf */
    init_xdev_lib();

    /* NRF24 Lib init*/
    nrf24_mod_init();
    /* NRF24 Lib init*/
    

    /* TMR0 COnfig: 8bit Timer */
    /* Fosc(4Mhz)/4, Ps:64 => tick 64uS*/
    OPTION_REGbits.T0CS = 0; // TImer mode, Internal OSC
    OPTION_REGbits.T0SE = 0; // Edge select
    OPTION_REGbits.PSA  = 0; // Prescaler used by Timer
    OPTION_REGbits.PS   = 5; // Prescaller 1:64
    INTCONbits.T0IF     = 0; // Clear Flag
    INTCONbits.T0IE     = 1; // Disable interrupt on overflow
    /* TMR0 config end*/
    
    /* TMR1 Config*/
#if 0
  ////TMR1 LP, Does not work without crystal. We dont need it now.
    T1CONbits.T1OSCEN = 0; //LP Osc enabled 32.768Khz
    T1CONbits.TMR1CS  = 0; // Select external clock source (which is, internal LP)
    T1CONbits.nT1SYNC = 1; // Async
    __delay_ms(10);         // Let the Oscillator Stabilize 
#endif
   
    ////TMR1 LP
//    T1CONbits.TMR1CS = 0; // Internal clock source
    TMR1L = 0;
    TMR1H = 0;
    T1CONbits.TMR1GE = 0;
    T1CONbits.T1CKPS = 3; // Prescaler = 0b11 ie. F/8
    PIR1bits.TMR1IF = 0; // Clear the flag
    PIE1bits.TMR1IE = 1; // TMR1 interrupt enable
    T1CONbits.TMR1ON = 1; //-->> Start the Timer, will act as System Time Source
    /* TMR1 Config end*/

    /* Interrupt config*/
    INTCONbits.RABIE = 1; // IOC is enabled PORTA/B
    INTCONbits.RABIF = 0; // clear the IOC  flag	
    INTCONbits.PEIE = 1; // peripheral intrrupt enable
    INTCONbits.GIE = 1; // GLOBL interrupt enable

}

/* Blink the LED 'blinks' times .5Sec Gap*/
void blinkLed(unsigned short blinks) {
    short i = 0;

    if (!blinks) return;

    // turn off LED
    IND_LED = 0;
    for (i = (blinks << 1); i > 0; i--) {
        IND_LED ^= 1; // Toggle LED
        __delay_ms(500); // Delay .5S
    }
}

