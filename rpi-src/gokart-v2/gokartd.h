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
#ifndef __GOKARTD_H__
#define __GOKARTD_H__

#define PACKED __attribute__ ((packed))

#define MAX_DET_CODE      4
#define MAX_PAYLOAD_COUNT 40
#define PACKET_SIZE      256
#define MAX_KARTS        32
#define LIRCD                   "/var/run/lirc/lircd"
#define PISTILLPATH             "/usr/bin/raspistill"
#define PISTILLARGS             "-vf -o "
#define OUTDIR                  "/etc/gokart/data/"
#define PIDFILE                 "/etc/gokart/var/run/gokartd.pid"
#define LOCKFILE                "/etc/gokart/var/run/lock"
#define LOGFILE                 "/etc/gokart/var/gokartd.log"

#define GOKARTD_VERSION "gokartd v1.1";

#define DEBUG_NO
#ifndef DEBUG
#define log_print(args...) { \
            printf (args);   \
        };
#else
#define log_print(args...) { \
        fprintf(g_log_fd, args); \
        fflush (g_log_fd); \
};
#endif

typedef enum gim_detect_type_ {
     IR = 0,
     HALL,
     MAX_DET_TYPE
} gim_detect_type;

//#pragma pack(push,1)
typedef struct gim_timeval_ {
    uint16_t sec;       // 1/2sec epoch counter
    uint8_t  m_sec;     // 2mS counter
}PACKED gim_timeval;
//#pragma pop(1)

//#pragma pack(push,1)
typedef struct gim_kart_data_ {
    gim_timeval    time;
    uint8_t     battery_level;
    uint8_t     dev_id        :5;
    uint8_t     detect_type   :1;
    uint8_t     detect_code   :2;   //Used for IR code, section detection
}kart_data_t;

typedef struct gim_response_list_ {
    kart_data_t tx_data;
    int         retry_count;
    struct gim_response_list_ *pnext;
    struct gim_response_list_ *pprev;
}gim_response_list_t;

typedef struct karts_ {
    uint8_t             kart_id;
    uint8_t             num_laps;
    struct timeval      lap_time;
    struct timeval      new_lap_time;
    uint8_t             battery_level;
    uint8_t             detect_type     :1;
    uint8_t             detect_code     :2;
    uint8_t             padding         :5;
    FILE                *fp;
}karts_t;

#endif
