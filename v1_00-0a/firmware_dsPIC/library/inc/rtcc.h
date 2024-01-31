#ifndef __rtcc_h__
#define __rtcc_h__

dspic_33ck_generic

// If the user wants to route the CLKO signal of the RTCC module to dsPeak buzzer
// connected on RD8, uncomment the following define
//#define RTCC_CLKO_ENABLE

#define RTC_QTY 1
#define RTC_1   0

typedef struct
{
    uint8_t channel;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t weekday;
    uint8_t date;    
    uint8_t month;
    uint16_t year;
}STRUCT_RTCC;

#define RTC_SECOND      1
#define RTC_MINUTE      2
#define RTC_HOUR        3
#define RTC_WEEKDAY     4
#define RTC_DATE        5
#define RTC_MONTH       6
#define RTC_YEAR        7

void RTCC_init (STRUCT_RTCC *t, uint8_t channel);
void RTCC_write_time (STRUCT_RTCC *t);
void RTCC_read_time (STRUCT_RTCC *t);
uint8_t RTCC_get_time_parameter (STRUCT_RTCC *t, uint8_t type);

#endif