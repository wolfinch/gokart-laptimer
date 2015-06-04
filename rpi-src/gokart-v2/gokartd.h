#ifndef __GOKARTD_H__
#define __GOKARTD_H__

#define PACKED __attribute__ ((packed))

#define MAX_PAYLOAD_COUNT 40

typedef enum gim_detect_type_ {
     IR = 0,
     HALL
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
    uint8_t     detect_type   :1;
    uint8_t     dev_id        :5;
    uint8_t     padding       :2;
}PACKED kart_data_t;

typedef struct gim_response_list_ {
    kart_data_t tx_data;
    int         retry_count;
    struct gim_response_list_ *pnext;
    struct gim_response_list_ *pprev;
}gim_response_list_t;


#endif
