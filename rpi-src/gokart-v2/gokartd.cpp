/*
 ******************************************************************************
 ******************************************************************************
 ***Gokartd - raspi conroller daemon for gokart lap-detector-iMproved (GLTDiM)*
 ***gokartd.cpp ***************************************************************
 ***Copyright (C) 2015 Joshith (joe.cet@gmail.com)*****************************
 ******************************************************************************
 ******************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>
#include <signal.h>
#include <RF24/RF24.h>
#include "gokartd.h"

static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"daemon", no_argument, NULL, 'd'},
        {0, 0, 0, 0}
};

using namespace std;
uint8_t     tx_addr[6] = {"Node1"};
uint8_t     rx_addr[6] = {"Base0"};
//uint8_t     rx_addr[6] = {0xAB, 0xCD, 0xAB, 0xCD, 0x71};

FILE        *g_log_fd  = NULL;

//kart_data_t         rx_data[MAX_PAYLOAD_COUNT];
//uint16_t            rx_data_count = 0;
gim_response_list_t *resp_list_head = NULL;
gim_response_list_t *resp_list_tail = NULL;
uint16_t            resp_list_cnt = 0;

void resp_list_remove (gim_response_list_t *node) {
    if (node) {
        if(!(node->resp_sent && node->data_processed)) {
            return;                 //remove only if both are done
        }

        if(node->pprev)
            node->pprev->pnext = node->pnext;
        else
            resp_list_head = node->pnext;
        if(node->pnext)
            node->pnext->pprev = node->pprev;
        else
            resp_list_tail = node->pprev;

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
        if(!resp_list_tail)
            resp_list_tail = node;
        node->resp_sent = node->data_processed = false;  // for 2 Fixed uses (1. for sending ACK, 2. processing data)
        resp_list_cnt++;
    }
}

// RPi generic:
RF24 radio(25, 0);      // SPIDEV constructor, spidev handles CSN
void gokart_add_response (kart_data_t *rx_data) ;

void gokart_rx () {
    kart_data_t rx_data;
    if (resp_list_cnt >= MAX_PAYLOAD_COUNT) {
        log_print ("ERROR: MAX payload count exceed \n");
        return;
    }

    // Grab the response, compare, and send to debugging spew
    radio.read( &rx_data, sizeof(rx_data));

    // Spew it
    log_print("RX Data: dev_id: %d, bat_level: 0x%x sec: %d mS:%d type:0x%x code:0x%x lap_num:%d seed:%d\r\n",
        rx_data.dev_id, rx_data.battery_level, rx_data.time.sec,
        rx_data.time.m_sec, rx_data.detect_type, rx_data.detect_code,
        rx_data.lap_count, rx_data.seed);

    //Now add to the response list
    gokart_add_response(&rx_data);

}

void
gokart_add_response (kart_data_t *rx_data) {
    gim_response_list_t *resp_node = NULL;

    if(!(resp_node = (gim_response_list_t *)malloc (sizeof(gim_response_list_t)))) {
        log_print("Malloc fail for resp_node...\n");
        return;
    }

    resp_node->tx_data = *rx_data;
    /*Alter something in the packet. Some issue in h/w causes pkts with same contents to drop */
    resp_node->tx_data.seed++ ;
    resp_node->retry_count           = 0;
    resp_list_add (resp_node);
}

void gokart_send_response(void) {
    gim_response_list_t *tmp, *resp_list = resp_list_tail;

    if (!resp_list)
        return;

    log_print ("resp_list_count: %d \n", resp_list_cnt);

    do {
        radio.stopListening();

        log_print ("resp: dev: %d retry_cnt:%d \n", resp_list->tx_data.dev_id, resp_list->retry_count);

        //Set the Tx addr for the resp
        tx_addr[4] = uint8_t(48 + (resp_list->tx_data.dev_id));

        radio.openWritingPipe(tx_addr);
        bool ok = radio.write( &(resp_list->tx_data), sizeof(kart_data_t));

        if (!ok){
            log_print("failed to send response! tx_addr: %s\n", tx_addr);
            radio.print_observe_tx();
            if (++resp_list->retry_count > 70) {
                resp_list->resp_sent = true; // consider it done. :-P
                tmp = resp_list;
                resp_list = resp_list->pprev;
                resp_list_remove(tmp);
            } else {
                resp_list = resp_list->pprev;
            }
        } else {
            log_print ("Response success! tx_addr: %s\n", tx_addr);
            resp_list->resp_sent = true;
            tmp = resp_list;
            resp_list = resp_list->pprev;
            resp_list_remove(tmp);
        }

        radio.startListening();
        usleep(5000);
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
    radio.setPayloadSize(6);
    radio.setCRCLength(RF24_CRC_8);
    radio.setChannel(10);
    // Dump the configuration of the rf unit for debugging

    radio.openWritingPipe(tx_addr);
    radio.openReadingPipe(1, rx_addr);

}

int main (int argc, char *argv[])
{
    char buf[128];
    int c;
    const char *progname = GOKARTD_VERSION;
    int  daemonize = 0;
    pid_t pid = 0;

    while ((c = getopt_long(argc, argv, "hvd", long_options, NULL))
            != EOF) {
        switch (c) {
        case 'h':
            printf("Usage: %s [socket]\n", argv[0]);
            printf("\t -h --help \t\tdisplay usage summary\n");
            printf("\t -v --version \t\tdisplay version\n");
            printf("\t -d --daemon \t\t Run as a daemon\n");
            return (EXIT_SUCCESS);
        case 'v':
            printf("%s\n", progname);
            return (EXIT_SUCCESS);
        case 'd':
            daemonize = 1;
            break;
        case '?':
            fprintf(stderr, "unrecognized option: -%c\n", optopt);
            fprintf(stderr, "Try `%s --help' for more information.\n", progname);
            return (EXIT_FAILURE);
        }
    }

    printf("starting gokartd!\n");

    //setup log
#ifdef DEBUG
    g_log_fd = fopen (LOGFILE, "w+");
    //  if(!g_log_fd) g_log_fd = 1;
#endif

    /* set SIGCHLD to SIG_IGN, don't care about childs */
    signal(SIGCHLD, SIG_IGN);

    //Check if we are aleardy running
    if(access(LOCKFILE, F_OK)) {
        //We are not running, create lock file
        sprintf(buf, "touch %s\n", LOCKFILE);
        system (buf);
    } else {
        log_print ("gokartd is already running \n");
        exit (1);
    }

    nrf24_init();
    //radio.startListening();         // Power_UP|PRIM_RX|CE=1

    radio.printDetails();

    if (daemonize) {
        if (daemon(0, 1) == -1) {
            fprintf(stderr, "%s: can't daemonize\n", progname);
            exit(-1);
        }
    }

    //Change CWD to OUTDIR
    if(chdir(OUTDIR)) {
        log_print("Couldn't Change CWD to %s errno: %d\n", OUTDIR, errno);
        exit (-1);
    }

    log_print("CWD: %s\n", getcwd(buf, 128));

    // write pidfile
    pid = getpid();
    sprintf(buf, "echo %d > %s\n", (int)pid, PIDFILE);
    system (buf);

    radio.startListening();
    log_print ("RF24: Listening \n");
    while (1) {
        while (!radio.available() && !resp_list_head ) {
            usleep (100);
        }

        if (radio.available()) {
            gokart_rx();
        }

        if (resp_list_head) {
            gokart_send_response();
        }

        if (resp_list_head) {
            gokart_process_data();
        }
    }
}
