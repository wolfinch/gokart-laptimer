/* 
 * File:   main.c
 * Author: Joshith
 *
 * Created on 23 March, 2015, 11:13 AM
 */

// 'C' source line config statements
#include "user.h"
#include "nrf24.h"
#include "util/xprintf.h"
#include <xc.h>
#include <stdbool.h>

/* Initialize EEPROM */
__EEPROM_DATA(0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
/* Dev_id*/
extern volatile uint8_t devId;
extern volatile uint8_t cfgMode;
extern volatile uint8_t ShouldSend;
extern volatile uint8_t ir_cmd_valid;
extern volatile uint8_t hall_detect;
extern volatile uint16_t jiffies;

extern volatile gim_timeval hall_cur_hit;
extern volatile gim_timeval ir_cur_hit;
//gim_timeval ir_laptime;
uint8_t     ir_laptime_valid = false;
uint8_t     ir_first_hit     = false;

//uint8_t     hall_send_success = true;
kart_data_t data[MAX_PAYLOAD_COUNT];                       // Payload list to send
uint8_t     data_count = 0;

/* TMR1 Tick time = 8uS 
 * Gives .5242Sec TMR1 Overflow time (Interrupt Time - clock resolution)
 * if we inc jiffies every int, gives an error of 1/40 Sec.
 * jiffies stores timeval in .5Sec resolution. 
 * TMR1H Gives 2mS resolution tick (TMRH Preset with value 0xB which to be considered). 
 * so, Time in Sec:mS = jiffies/2:TMR1H*2
 */
inline uint16_t epoch (void) {
    return jiffies; ///2;  Lets do the costly operations from Serverside.
}

void nrf_print_details() {
    uint8_t status = nrf24_getStatus();
    
    xprintf("NRF24+ STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n",
           status,
           (status & (1 << RX_DR))?1:0,
           (status & (1 << TX_DS))?1:0,
           (status & (1 << MAX_RT))?1:0,
           ((status >> RX_P_NO) & 0b111),
           (status & (1 << TX_FULL))?1:0
          );
    
    xprintf("Data Rate\t = 0x%x\r\n", (getDataRate()));
    xprintf("CRC Length\t = 0x%x\r\n", (getCRCLength()));
    xprintf("PA Power\t = 0x%x\r\n", getPALevel()); 
     
}

// Rx Test:

void rx_test(void) {
    uint8_t data[5] = {0};
    if (nrf24_dataReady()) {
        nrf24_getData(&data);
        xprintf("Rx data: 0x%x 0x%x 0x%x 0x%x\r\n", data[0], data[1], data[2], data[3]);
    }
}

void nrf24_send_detect_data(void ) {
    uint8_t rcount = data_count-1;

    xprintf("%s: ENTER rcount:%d\r\n", "nrf24_send_detect_data", rcount);

    /* Automatically goes to TX mode */
    xprintf("bat_level: 0x%x sec: %d mS:%d type:0x%x\r\n", data[rcount].battery_level, data[rcount].time.sec,
                                     data[rcount].time.m_sec, data[rcount].detect_type);
    nrf24_send((uint8_t *)(&data[rcount])); 
    while (nrf24_isSending());
    
#if 0
    /* Wait for transmission to end */
    xprintf("nrf24_isSending\r\n");
    while (nrf24_isSending()){
        print_observe_tx ();
    };

    /* Make analysis on last tranmission attempt */
    rcount = nrf24_lastMessageStatus();

    if (rcount == NRF24_TRANSMISSON_OK) {
        xprintf("Tranmission went OK\r\n");
    } else if (rcount == NRF24_MESSAGE_LOST) {
        xprintf("Message is lost ...\r\n");
        return;
    }
   /* Retranmission count indicates the tranmission quality */
    rcount = nrf24_retransmissionCount();
    xprintf("Retranmission count: 0x%x\r\n", rcount);
    
    xprintf("go to Rx Mode \r\n");   
#endif
    nrf24_powerUpRx();

    kart_data_t rx_data;
    uint8_t     retry = 20;
    do {
        if (nrf24_dataReady()) {
            nrf24_getData((uint8_t *)&rx_data);
            xprintf("Rx: devId:0x%x \r\n", rx_data.dev_id );
            if(rx_data.dev_id != devId) {
                /* Error, wrong response recvd, ignore, resend*/
                return;
            }
            data_count--;
            if (!data_count)
                nrf24_powerDown();
            return;
        }
        retry--;
        __delay_ms (10); // wait and retry
    } while (retry);
    
    xprintf("Failed reciving response \r\n");   
 }

inline void
handle_ir_cmd (void) {
    static gim_timeval ir_first_hit, ir_last_hit;
    
    if (data_count >= MAX_PAYLOAD_COUNT) {
        xprintf("ERROR: data exceed max\n\r");
        return;
    }

    if (!ir_laptime_valid && (ir_cur_hit.sec + 4 < jiffies)) {
    // Last IR detect was >2 Sec ago. Calculate the laptime and send now
        data[data_count].time.sec = ir_first_hit.sec/2 + ir_last_hit.sec/2 + 
                (ir_first_hit.sec%2 | ir_last_hit.sec%2); // Take avg. (adjust reminder =>.5+.5 = .5)
        data[data_count].time.m_sec = ir_first_hit.m_sec / 2 + ir_last_hit.m_sec / 2; // Take avg.
        
        ir_laptime_valid = true;
        
        /* Create send data and insert */

        data[data_count].battery_level = read_battery_level();
//        data.time = ir_laptime;
        data[data_count].dev_id = devId;
        data[data_count].detect_type = IR;
        data_count ++ ;   //new data to send
        return;
    }
    
    if(ir_last_hit.sec + 2 > ir_cur_hit.sec ) {     //if the signal comes within 1Sec
        ir_last_hit = ir_cur_hit;        
    } else {        // This is the first hit of a new lap
        ir_last_hit = ir_first_hit = ir_cur_hit;
        ir_laptime_valid = false;
    }
}

inline void
handle_hall_cmd(void) {
    if (data_count >= MAX_PAYLOAD_COUNT) {
        xprintf("ERROR: data exceed max\n\r");
        return;
    }    
    data[data_count].battery_level = read_battery_level();    
    data[data_count].time       = hall_cur_hit;
    data[data_count].dev_id  = devId;
    data[data_count].detect_type = HALL;

    data_count ++ ;   //new data to send
}

void cont_wave_test (void) {
    
    nrf24_powerDown();
    nrf24_configRegister(RF_SETUP, (1 << CONT_WAVE) | (1 << PLL_LOCK) | (0 << RF_DR_HIGH) | (0 << RF_DR_LOW) | ((0x03) << RF_PWR));
    nrf24_configRegister(RF_CH, 10);
    
    nrf24_powerUpTx();
    
    CE = 1;

}
#if 0
void carrier_detect( void) {    
    uint8_t i = 40;
    nrf24_powerUpRx();

    while (i--) {
        // Select this channel
        nrf24_configRegister(RF_CH, i);

        // Start listening
        __delay_us(128);
        //radio.stopListening();

        // Did we get a carrier?
        if (testRPD())
            ++value[i];
        //stopListening();
    }
    
    for (i=0; i<40; i++) {
        xprintf("%x", (value[i]&0xF));
    }
    xprintf("\n\r");
}
#endif

void ping_out (void) {
    uint8_t val = 10;
    // Select this channel
    nrf24_configRegister(RF_CH, 5);
  //  nrf24_powerUpTx();
    nrf24_send (&val);
}

void main() {
    
    /* Read config */
    devId = eeprom_read(0x00);
    
    /* Initialize everything */
    InitApp();
    xprintf("InitApp OK\r\n");
    
    xprintf("Dev Id 0x%x\r\n", devId);

    blinkLed(devId);
    xprintf("Init Complete\r\n");
    nrf_print_details();

//    cont_wave_test();
	while(1)
    {
        if (ir_cmd_valid || (!ir_laptime_valid && ir_first_hit)) { //Valid IR Beacon detected
            ir_cmd_valid = false;
            ir_first_hit = true;
            xprintf("*** Valid IR Signal!!***\r\n");
            handle_ir_cmd();
        }

        if (hall_detect) { //Hall detected
            hall_detect = 0;
            handle_hall_cmd();
            xprintf("*** Hall Signal Received!!***\r\n");
        }

        if (data_count) {
            nrf24_send_detect_data();
        }

        if (ShouldSend) {
            ShouldSend = 0;
            //         nrf24_send_detect_data();
        }
        
        if ((ir_cur_hit.sec + 240 < jiffies) && (hall_cur_hit.sec + 240 < jiffies ) && !data_count) {
            /* no activity for long time (2mins), Let's power down and Sleep now.*/
            xprintf("--- Entering SLEEP mode!!---\r\n");
            nrf24_powerDown();
            SLEEP ();
            jiffies = 0;        // Reset the jiffies
        }
        __delay_ms(60);
    }        

}

