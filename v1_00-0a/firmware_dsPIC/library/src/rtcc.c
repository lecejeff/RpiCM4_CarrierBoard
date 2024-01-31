#include "rtcc.h"

STRUCT_RTCC RTCC_struct[RTC_QTY];

void RTCC_init (STRUCT_RTCC *t, uint8_t channel)
{
    switch (channel)
    {
        case RTC_1:
            t->channel = channel;
            TRISCbits.TRISC14 = 1;
            CNPDCbits.CNPDC14 = 1;
            __builtin_write_OSCCONL(OSCCON | 0x02); // Enable secondary oscillator
            __delay_us(100);
            while(OSCCONbits.LPOSCEN != 1);
            __builtin_write_RTCWEN();               // Enable access to the RTC registers
            __delay_us(100);
            RCFGCALbits.RTCEN = 0;

            // If using RTCC_output for 1Hz clock
#ifdef RTCC_CLKO_ENABLE
            TRISDbits.TRISD8 = 0;                       // RD8 configured as an output (RTCC_ALARM)
            PADCFG1bits.RTSECSEL = 1;                   // Enable 1Hz clock on RTCC pin
            RCFGCALbits.RTCOE = 1;                      // RTCC output is enabled
#endif
            break;
            
        default:
            break;
    }
}

void RTCC_write_time (STRUCT_RTCC *t)
{
    switch (t->channel)
    {
        case RTC_1:
            RCFGCALbits.RTCEN = 0;
            RCFGCALbits.RTCPTR = 3;
            RTCVAL = dec_to_bcd(t->year) & 0x00FF;
            RTCVAL = ((dec_to_bcd(t->month) << 8) | dec_to_bcd(t->date));
            RTCVAL = ((dec_to_bcd(t->weekday) << 8) | dec_to_bcd(t->hour));
            RTCVAL = ((dec_to_bcd(t->minute) << 8) | dec_to_bcd(t->second));
            RCFGCALbits.RTCEN = 1;
            break;
            
        default:
            break;
    }
}

void RTCC_read_time (STRUCT_RTCC *t)
{   
    uint16_t read3 = 0, read2 = 0, read1 = 0, read0 = 0;
    switch (t->channel)
    {
        case RTC_1:
            RCFGCALbits.RTCPTR = 3;    
            read3 = RTCVAL;
            read2 = RTCVAL;
            read1 = RTCVAL;
            read0 = RTCVAL;

            t->second = bcd_to_decimal(read0 & 0x00FF);
            t->minute = bcd_to_decimal((read0 & 0xFF00)>>8);
            t->hour = bcd_to_decimal(read1 & 0x00FF);
            t->weekday = bcd_to_decimal((read1 & 0xFF00)>>8);
            t->date = bcd_to_decimal(read2 & 0x00FF);
            t->month = bcd_to_decimal((read2 & 0xFF00)>>8);
            t->year = bcd_to_decimal(read3);
            break;
            
        default:
            break;
    }
}

uint8_t RTCC_get_time_parameter (STRUCT_RTCC *t, uint8_t type)
{
    switch (type)
    {
        case RTC_SECOND:
            return t->second;
            break;
            
        case RTC_MINUTE:
            return t->minute;
            break;
            
        case RTC_HOUR:
            return t->hour;
            break;
            
        case RTC_WEEKDAY:
            return t->weekday;
            break;
            
        case RTC_DATE:
            return t->date;
            break;
            
        case RTC_MONTH:
            return t->month;
            break;
            
        case RTC_YEAR:
            return t->year;
            break;
            
        default:
            return 0;
            break;
    }
}
