/* 
 * File:   main.c
 * Author: Joshith
 *
 * Created on 23 March, 2015, 11:13 AM
 */

//__CONFIG(FOSC_INTOSCIO & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & IOSCFS_8MHZ & BOREN_NSLEEP);


// PIC16F684 Configuration Bit Settings

// 'C' source line config statements
#include "user.h"
#include <xc.h>


/* Initialize EEPROM */
__EEPROM_DATA(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
/* Dev_id*/
extern volatile unsigned short devId;
extern volatile unsigned short cfgMode;

void main()
{
    /* Initialize everything */
    InitApp();
    
    /* Read config */
    devId = eeprom_read(0x00);
    
    blinkLed(devId);

	while(1)
	{
        /* Send IR frame with programmed deviceID and default data key*/
        /* if not in config mode */
        if(devId && !cfgMode) {
            sendFrame(devId, DATA_KEY_1);
        }
        //__delay_ms(60);
    }
}

