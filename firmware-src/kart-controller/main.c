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
extern volatile uint8_t ir_cur_code;
extern volatile uint8_t hall_detect;
extern volatile uint16_t jiffies;

extern volatile gim_timeval ir_cur_hit;
extern volatile gim_timeval hall_cur_hit;

extern volatile uint8_t ir_lap_count;
extern volatile uint8_t hall_lap_count;

//uint8_t     hall_send_success = true;
extern volatile kart_data_t data[MAX_PAYLOAD_COUNT];                       // Payload list to send
extern volatile int8_t      data_count;
extern volatile uint8_t      data_rx_slot;
extern volatile uint8_t      data_next_rx_slot;
volatile uint8_t     battery_level = 0xff;
uint8_t              gim_seed;

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

void nrf24_send_detect_data(void ) {
    kart_data_t rx_data;
    uint8_t retry;
    uint8_t d_cnt = 0;
    
    xprintf("%s: ENTER data_count:%d\r\n", "nrf24_send_detect_data", data_count);
    
    while (data_ready()) {
        if (!is_data_valid(d_cnt)) {
            if (data_next_rx_slot == data_rx_slot) {
                data_next_rx_slot = d_cnt;
            }
            if (++d_cnt >= MAX_PAYLOAD_COUNT) {
                return;
            } else {
                continue;
            }
        }
        /* Automatically goes to TX mode */
        xprintf("bat_level: 0x%x sec: %d mS:%d type:0x%x code: 0x%x lap_cnt:%d\r\n", data[d_cnt].battery_level, data[d_cnt].time.sec,
                data[d_cnt].time.m_sec, data[d_cnt].detect_type, data[d_cnt].detect_code, data[d_cnt].lap_count);

        data[d_cnt].seed = gim_seed;
        nrf24_send((uint8_t *) (&data[d_cnt]));
        while (nrf24_isSending());
        /* Make analysis on last tranmission attempt */
        if (nrf24_lastMessageStatus() == NRF24_MESSAGE_LOST) {
            xprintf("Message is lost ...\r\n");
        }
        
        /* Retranmission count indicates the tranmission quality */
        xprintf("Retranmission count: 0x%x\r\n", nrf24_retransmissionCount());

        /* Even if the data send show failure, it could be an ack miss. 
         * If we get a response back for the previous sent, we are ok
         */
        nrf24_powerUpRx();
        retry = 20;
        gim_seed++;
        do {
            if (nrf24_dataReady()) {
                nrf24_getData((uint8_t *) & rx_data);
                xprintf("Rx: devId:0x%x retry: 0x%d\r\n", rx_data.dev_id, retry);
                if (rx_data.dev_id != devId) {
                    /* Error, wrong response recvd, ignore, resend*/
                    break;
                }
                data_count--;
                /* unset the data slot, */
                data_unset(d_cnt);
                if (data_next_rx_slot == data_rx_slot) {
                    data_next_rx_slot = d_cnt;
                }
                if (!data_ready()) {
                    nrf24_powerDown();
                    return;
                }
                break;
            }
            retry--;
            __delay_ms(5); // wait and retry
        } while (retry);
        
        if (++d_cnt >= MAX_PAYLOAD_COUNT) {
            return;
        }
    }
 }
    
void
handle_ir_cmd (void) {
    static gim_timeval ir_first_hit, ir_last_hit;
    //static uint8_t     is_ir_first_hit     = true;
    static uint8_t     is_ir_lap_valid     = true;

    if (ir_cmd_valid) {
        //xprintf("va%d", ir_state_pos);
        ir_cmd_valid = false;
        if (is_ir_lap_valid || ir_last_hit.sec + 4 < ir_cur_hit.sec) {
            // This is the first hit of a new lap
            ir_last_hit = ir_first_hit = ir_cur_hit;
            is_ir_lap_valid = false;           // Marking first IR hit since we are operational
        } else { //if the signal comes within 2Sec, advance the window
            ir_last_hit = ir_cur_hit;
        }
    } else if ((ir_last_hit.sec + 4 < jiffies) && !is_ir_lap_valid) {
        // Last IR detect was >2 Sec ago. Calculate the lap-time and send now

        xprintf("*** Valid IR lap!!***\r\n");
        is_ir_lap_valid = true;
        if (ir_lap_count<64) {
            ir_lap_count++;     // Set lap_count wraparound = 63
        } else {
            ir_lap_count = 0;
        }
        
        data[data_rx_slot].time.sec = ir_first_hit.sec/2 + ir_cur_hit.sec/2 + 
                (ir_first_hit.sec%2 | ir_cur_hit.sec%2); // Take avg. (adjust reminder =>.5+.5 = .5)
        data[data_rx_slot].time.m_sec = ir_first_hit.m_sec / 2 + ir_cur_hit.m_sec / 2; // Take avg.
                
        /* Create send data and insert */
        battery_level = read_battery_level();
        data[data_rx_slot].battery_level = battery_level;
        data[data_rx_slot].dev_id = devId;
        data[data_rx_slot].detect_type = IR;
        data[data_rx_slot].detect_code = ir_cur_code;
        data[data_rx_slot].lap_count = ir_lap_count;
        data_set (data_rx_slot);
        data_rx_slot = data_next_rx_slot;
        data_count++; //new data to send

        return;
    }
}
#if 0
void cont_wave_test (void) {
    
    nrf24_powerDown();
    nrf24_configRegister(RF_SETUP, (1 << CONT_WAVE) | (1 << PLL_LOCK) | (0 << RF_DR_HIGH) | (0 << RF_DR_LOW) | ((0x03) << RF_PWR));
    nrf24_configRegister(RF_CH, 10);
    
    nrf24_powerUpTx();
    
    CE = 1;

}
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

void ping_out (void) {
    uint8_t val = 10;
    // Select this channel
    nrf24_configRegister(RF_CH, 5);
  //  nrf24_powerUpTx();
    nrf24_send (&val);
}

void rx_test (void) {
    kart_data_t rx_data;
    uint8_t     rcount =0;
    
    nrf24_powerUpRx();
    while (1) {
        while(!nrf24_dataReady());
        if(nrf24_dataReady()) {
            gim_seed++;
            nrf24_getData((uint8_t *) & rx_data);
            xprintf("Rx: devId:0x%x lap_cnt: %d\r\n", rx_data.dev_id, rx_data.lap_count);
            rx_data.seed = gim_seed;

            nrf24_send((uint8_t *) (&rx_data));
            /* Wait for transmission to end */
            xprintf("nrf24_isSending?\r\n");
            while (nrf24_isSending()) {
                print_observe_tx();
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
            nrf24_powerUpRx();
        }
        __delay_ms(60);
    }
}
#endif

void main() {
    uint8_t i = 0;
    
    /* Read config */
    devId = eeprom_read(0x00);
    
    /* Initialize everything */
    InitApp();
    xprintf("InitApp OK\r\n");
    
    battery_level = read_battery_level();

    xprintf("Dev Id 0x%x\r\n", devId);

    blinkLed(devId);
    xprintf("Init Complete\r\n");
    nrf_print_details();

//    cont_wave_test();
//    rx_test();
//#if 0
	while(1)
    {
        handle_ir_cmd();
        
        /* if there is a signal hit, current data slot must be used up, get next slot*/
        if (data_next_rx_slot == data_rx_slot) {
            for (i = 0; i < MAX_PAYLOAD_COUNT; i++) {
                if(i == data_rx_slot)
                    continue;
                if (!is_data_valid(i)) {
                    data_next_rx_slot = i;
                    break;
                }
            }
            if (i == MAX_PAYLOAD_COUNT) {
                if(data_next_rx_slot == MAX_PAYLOAD_COUNT-1) {
                    data_next_rx_slot = 0; //If no slots are available, select (remove) the oldest data
                } else {
                    data_next_rx_slot++;
                }
            }
            //xprintf("Rx_slot:%d next_slot:%d \r\n", data_rx_slot, data_next_rx_slot);
        }
        
        if (hall_detect) { //Hall detected
            hall_detect = 0;
            xprintf("*** Hall Signal Received!!*** d_cnt: %d bmap: 0b%b\r\n", data_count, data_valid_bitmap);
        }

        if (data_ready()) {
            nrf24_send_detect_data();
        }

        if (ShouldSend) {
            ShouldSend = 0;
            //         nrf24_send_detect_data();
        }

        /* If no activity for time more than 2mins, Let's power down and Sleep now.*/
        if ((ir_cur_hit.sec + 240 < jiffies) && (hall_cur_hit.sec + 240 < jiffies ) && !data_ready()) {
            xprintf("--- Entering SLEEP mode!!---\r\n");
            nrf24_powerDown();
            jiffies = 0; // Reset the jiffies
            SLEEP ();
            battery_level = read_battery_level();
        }
        __delay_ms(60);
    }        
//#endif
}

