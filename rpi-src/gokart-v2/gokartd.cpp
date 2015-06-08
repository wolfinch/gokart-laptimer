/*
 Copyright (C) 2015 Joshith (joe.cet@gmail.com)

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>
#include "gokartd.h"

using namespace std;
uint8_t     tx_addr[6] = {"Node1"};
uint8_t     rx_addr[6] = {"Base0"};

kart_data_t rx_data[MAX_PAYLOAD_COUNT];
uint16_t         rx_data_count = 0;
gim_response_list_t *resp_list_head = NULL;
uint16_t         resp_list_cnt = 0;

void resp_list_remove (gim_response_list_t *node) {
    if (node) {
        if(node->pprev)
            node->pprev->pnext = node->pnext;
        else
            resp_list_head = node->pnext;
        if(node->pnext)
            node->pnext->pprev = node->pprev;

        free(node);
        resp_list_cnt--;
    }
}

void resp_list_add (gim_response_list_t *node) {
    if (node) {
        if(resp_list_head)
            resp_list_head->pprev = node;
        node->pnext = resp_list_head;
        node->pprev = NULL;
        resp_list_head = node;
        resp_list_cnt++;
    }
}

// RPi generic:
RF24 radio(25, 0);      // SPIDEV constructor, spidev handles CSN
void gokart_add_response (kart_data_t *rx_data) ;

void gokart_rx () {

    if (rx_data_count >= MAX_PAYLOAD_COUNT) {
        printf ("ERROR: MAX payload count exceed \n");
        return;
    }

    // Grab the response, compare, and send to debugging spew
    radio.read( &rx_data[rx_data_count], sizeof(rx_data));

    // Spew it
    printf("RX Data: dev_id: %d, bat_level: 0x%x sec: %d mS:%d type:0x%x code:0x%x\r\n",
        rx_data[rx_data_count].dev_id, rx_data[rx_data_count].battery_level, rx_data[rx_data_count].time.sec,
        rx_data[rx_data_count].time.m_sec, rx_data[rx_data_count].detect_type, rx_data[rx_data_count].detect_code);

    //Now Send the response back
    gokart_add_response(&rx_data[rx_data_count]);

    rx_data_count++;
}

void
gokart_add_response (kart_data_t *rx_data) {
    gim_response_list_t *resp_node = NULL;

    if(!(resp_node = (gim_response_list_t *)malloc (sizeof(gim_response_list_t)))) {
        printf("Malloc fail for resp_node...\n");
        return;
    }

    resp_node->tx_data = *rx_data;
    /*Alter something in the packet. Some known issue in h/w causes pkts with same contents to drop */
    resp_node->tx_data.battery_level = 0xff;
    resp_node->retry_count           = 0;
    resp_list_add (resp_node);
}

void gokart_send_response(void) {
    gim_response_list_t *tmp, *resp_list = resp_list_head;

    if (!resp_list)
        return;

    printf ("resp_list_count: %d \n", resp_list_cnt);

    do {
        radio.stopListening();

        printf ("resp: dev: %d retry_cnt:%d \n", resp_list->tx_data.dev_id, resp_list->retry_count);

        //Set the Tx addr for the resp
        tx_addr[4] = uint8_t(48 + (resp_list->tx_data.dev_id));

        radio.openWritingPipe(tx_addr);
        bool ok = radio.write( &(resp_list->tx_data), sizeof(kart_data_t));

        if (!ok){
            printf("failed to send response! tx_addr: %s\n", tx_addr);
            radio.print_observe_tx();
            if (resp_list->retry_count++ > 70) {
                tmp = resp_list;
                resp_list = resp_list->pnext;
                resp_list_remove(tmp);
            } else {
                resp_list = resp_list->pnext;
            }
        } else {
            printf ("Response success! tx_addr: %s\n", tx_addr);
            tmp = resp_list;
            resp_list = resp_list->pnext;
            resp_list_remove(tmp);
        }

        radio.startListening();
        usleep(1000);
        if(radio.available())
            gokart_rx();

    } while (resp_list);
}

void
nrf24_init(void) {
    // Setup and configure rf radio
    radio.begin();

    // optionally, increase the delay between retries & # of retries
    radio.setRetries(15,15);
    radio.setPayloadSize(5);
    radio.setCRCLength(RF24_CRC_8);
    radio.setChannel(10);
    // Dump the configuration of the rf unit for debugging

    radio.openWritingPipe(tx_addr);
    radio.openReadingPipe(1, rx_addr);

}

void
gokart_process_data (void) {

}

int main ()
{

    printf("gokartd started!\n");

    nrf24_init();
    //radio.startListening();         // Power_UP|PRIM_RX|CE=1

    radio.printDetails();

    radio.startListening();
    printf ("RF24: Listening \n");
    while (1) {
        while (!radio.available() && !resp_list_head && !rx_data_count) {
            usleep (100);
        }

        if (radio.available()) {
            gokart_rx();
        }

        if (resp_list_head) {
            gokart_send_response();
        }

        if (rx_data_count) {
            gokart_process_data();
        }
    }
}
