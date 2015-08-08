/*
 ******************************************************************************
 ******************************************************************************
 ***Gokartd - raspi conroller daemon for gokart lap-detector-iMproved (GLTDiM**
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/un.h>
#include <errno.h>
#include "gokartd.h"

extern uint16_t            resp_list_cnt;
extern gim_response_list_t *resp_list_tail;
extern FILE        *g_log_fd ;

struct timeval max_tv = MAX_TIME;
karts_t all_karts[MAX_KARTS][MAX_DET_TYPE][MAX_DET_CODE];

void
gokart_snapapic(uint8_t kart_id, struct timeval *tv) {
    pid_t pid = 0;
    char img_path[128] = "";

    snprintf(img_path, 64, "%s kart%d_%lu.jpg", PISTILLARGS, kart_id, tv->tv_sec);

    log_print ("raspistill arg path: %s\n", img_path);
    pid = fork ();

    if (pid == -1) {
        /* fork failed */
        return;
    }

    if (pid > 0) {
        /*this is parent */
        return;
    } else {
        /* this is child */
        if(-1 == execv(PISTILLPATH, (char * const *)img_path)) {
            log_print ("raspistill exec failed path:%s arg path: %s\n", PISTILLPATH, img_path);
        }
        exit (1);
    }

}

int check_fd_fine(FILE *fp) {
    struct stat _stat;
    int ret = -1;
    int fd  =  0;

    if (!fp)
        return -1;

    fd = fileno(fp);

    if(fd > 1) {
        if(!fstat(fd, &_stat)) {
            if(_stat.st_nlink >= 1) {
                return 0;
            }
            log_print ("file removed fd: %d st_nlink:%d\n", fd, _stat.st_nlink);
        }
    }
    log_print ("failure fd: %d\n", fd);
    return ret;
}


int
gokart_process_data(void)
{
    kart_data_t *one_data = NULL;
    gim_response_list_t *tmp, *resp_list = resp_list_tail;

    karts_t        *this_kart = NULL;
    struct timeval tv_real, tv;
    char           file_name[64] = "";
    FILE           *fp = NULL;
    int            session_lap_count = 0;

    if (!resp_list) {      //early return for empty list
        return 0;
    }

    log_print("%s:ENTER: count:%d \n", __func__, resp_list_cnt);

    do {
        if(resp_list->data_processed){ //already processed, skip
            resp_list = resp_list->pprev;
            continue;
        }

        one_data = &resp_list->tx_data;

        if (one_data->dev_id < 0 || one_data->dev_id >= MAX_KARTS) { // Error
            log_print ("invalid Kart_id: %d \n", one_data->dev_id);
            goto cleanup;
        }

        /* Now get the kart struct */
        this_kart = &all_karts[one_data->dev_id][one_data->detect_type][one_data->detect_code];

        /* Fix the time issue here */
        tv_real.tv_sec = one_data->time.sec/2; //Each tick is .5Sec
        tv_real.tv_usec = 1000*(500*(one_data->time.sec%2) + one_data->time.m_sec*2); //Each tick is 2mSec

        this_kart->kart_id = one_data->dev_id;
        this_kart->battery_level    = (one_data->battery_level*100/255);    //Convert to n%
        this_kart->detect_type      = one_data->detect_type;
        this_kart->detect_code      = one_data->detect_code;

        log_print ("%s:LOG: dev_id:%d, type:%s, code: 0x%x battery_level:%d%% real_time:%ds.%dms lap_count:%d\n", __func__,
                this_kart->kart_id, (this_kart->detect_type?"HALL":"IR"), this_kart->detect_code,
                      (int)this_kart->battery_level, (int)tv_real.tv_sec, (int)tv_real.tv_usec, one_data->lap_count);

        /* New whole set session or
         * individual kart session (only file for this kart was deleted) */
        if (this_kart->kart_id == 0 || check_fd_fine (this_kart->fp)) {
            /* We are seeing this kart for the first time */

            this_kart->num_laps         = 0;
            this_kart->num_laps_offset  = one_data->lap_count;
            this_kart->lap_num_wrap_count = 0;
            this_kart->curr_lap_count     = 0;
            this_kart->prev_lap_time    = this_kart->curr_lap_time = (struct timeval){0, 0};
            this_kart->time_offset      = tv_real;

            sprintf(file_name, "KART%d_%s_%d", this_kart->kart_id,
                (this_kart->detect_type?"HALL":"IR"), this_kart->detect_code);

            /* open a file for data */
            if ((fp = fopen (file_name, "w+")) == NULL) {       //FIXME: type detect
                log_print ("file open with name : %s failed\n", file_name);
                goto cleanup;
            }

            log_print ("new session detected. file_name: %s\n", file_name);

            this_kart->fp = fp;
            fprintf (fp, "KART:%d:TIME:%d.%d:LAP_NUM:%d:BAT:%d:DET_TYPE:%d:DET_CODE:%d:\n",
                this_kart->kart_id,(int)this_kart->curr_lap_time.tv_sec, (int)this_kart->curr_lap_time.tv_usec,
                0, this_kart->battery_level, this_kart->detect_type, this_kart->detect_code);

            fflush (this_kart->fp);

            /* take a snap */
            //gokart_snapapic (this_kart->kart_id, &(this_kart->curr_lap_time));
            /* ALL success */
            goto cleanup;
        }

        /* subsequent lap */
        if (this_kart->kart_id != one_data->dev_id) {
            /* Error */
            log_print ("inconsistent data. kart_id db: %x, kart_id signal: %x",
                    this_kart->kart_id, one_data->dev_id);
            goto cleanup;
        }

        /* Adjust real time with offset */
        if (timercmp(&tv_real, &this_kart->time_offset, >=)) {
            timersub ( &tv_real, &this_kart->time_offset, &tv);
            log_print ("real_time.S: %d, adj_time.S: %d offset.S:%d\n", (int)tv_real.tv_sec, (int)tv.tv_sec, (int)this_kart->time_offset.tv_sec);
        } else {
            /* wrapped around. this will not handle full timer  */
            struct timeval tv_adj;
            timersub ( &max_tv, &this_kart->time_offset, &tv_adj);
            timeradd (&tv_real, &tv_adj, &tv);
            log_print ("wrapped around. real_time.S: %d, adj_time.S: %d offset.S:%d\n", (int)tv_real.tv_sec, (int)tv.tv_sec, (int)this_kart->time_offset.tv_sec);
        }

        /* Check for duplicate lap data.
         * Wraparound time for timer is 9Hrs.
         * This is not a fool-proof idea for duplicates, but handle most other tricky cases
         *  however, other duplicates will get addressed later with the lap_num data we have */
        if (this_kart->curr_lap_time.tv_sec >= tv.tv_sec){
            if (tv.tv_sec == this_kart->curr_lap_time.tv_sec ||
                    tv.tv_sec == this_kart->prev_lap_time.tv_sec ) {
                //Definitely a duplicate. e.g: 1-2, 1-2 pattern duplicate is handled here
                log_print ("%s: kart:%d: lap_num:%d duplicate lap data, ignore\n", __func__,
                                this_kart->kart_id, one_data->lap_count);
                goto cleanup;
            } else if (tv.tv_sec > this_kart->prev_lap_time.tv_sec) {
                    /* lap between 1 & 2. update 1 */
                    this_kart->prev_lap_time = tv;
            } else {
                /* This must be a lap older than 1, that could be a duplicate as well */
            }
        } else {
            /* Keep the most latest laps time */
            this_kart->prev_lap_time = this_kart->curr_lap_time ;
            this_kart->curr_lap_time = tv;
        }

        session_lap_count = (one_data->lap_count >= this_kart->num_laps_offset) ? (one_data->lap_count - this_kart->num_laps_offset):
                        (MAX_LAP_COUNT - this_kart->num_laps_offset + one_data->lap_count);
        /* Handle lap_count > MAX_LAP_COUNT */
        if((this_kart->curr_lap_count == MAX_LAP_COUNT-1)) {
            this_kart->lap_num_wrap_count++;
        }
        this_kart->curr_lap_count = session_lap_count;
        session_lap_count = this_kart->lap_num_wrap_count* MAX_LAP_COUNT + session_lap_count;
        this_kart->num_laps++;

        fprintf (this_kart->fp, "KART:%d:TIME:%d.%d:LAP_NUM:%d:BAT:%d:DET_TYPE:%d:DET_CODE:%d:\n",
              this_kart->kart_id, (int)tv.tv_sec, (int)tv.tv_usec, session_lap_count,
              this_kart->battery_level, this_kart->detect_type, this_kart->detect_code);

        fflush (this_kart->fp);

        /* take a snap */
        //gokart_snapapic (this_kart->kart_id, &(this_kart->curr_lap_time));

        /* Clean up for the current node */
        cleanup:
        resp_list->data_processed = true;
        tmp = resp_list;
        resp_list = resp_list->pprev;
        resp_list_remove(tmp);
    } while (resp_list);

    return (0);
}
