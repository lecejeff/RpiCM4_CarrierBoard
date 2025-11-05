//***************************************************************************//
// File      : ADC.h
//
// About     : dsPIC33CK ADC module is a SAR (successive-approximation) type
//             There are up to 5 ADC modules and up to 27 conversion channels
//             The resolution is 12bits, up to 3.5Msps
//
// Functions : 
//
// Includes  : dspeak_generic.h
//           
// Purpose   : Driver for the dsPIC33CK ADC 12bits module
//
//Jean-Francois Bilodeau    MPLab X v6.05   24/06/2024
//****************************************************************************//
#ifndef __ADC_H_
#define	__ADC_H_

#include "dspic_33ck_generic.h"

#define ADC_CORE_MODE_SHARED    0
//#define ADC_CORE_MODE_DEDIC     1

#ifdef ADC_CORE_MODE_SHARED
// Defines for module used in shared ADC core
#define ADC_SHARED_FORM_SIGNED  0
#define ADC_SHARED_FORM_FRACT   1

#define ADC_SHARED_RES_6b       0
#define ADC_SHARED_RES_8b       1
#define ADC_SHARED_RES_10b      2
#define ADC_SHARED_RES_12b      3

#define ADC_SHARED_CORE_READY   1
#define ADC_SHARED_CORE_ENABLE  1
#endif

#ifdef ADC_CORE_MODE_DEDIC
// Defines for module used dedicated ADC code
#define ADC_DEDIC_SAMPLE_READY  1

#define ADC_DEDIC_RES_6b        0
#define ADC_DEDIC_RES_8b        1
#define ADC_DEDIC_RES_10b       2
#define ADC_DEDIC_RES_12b       3

#define ADC_DEDIC_CORE_READY    1
#define ADC_DEDIC_CORE_ENABLE   1
#endif

// ADC states
#define ADC_INSTANTIATED        0
#define ADC_INITIALIZED         1
#define ADC_CONV_IN_PROG        2
#define ADC_CONV_READY          3

// ADC output formats
#define ADC_FORMAT_INTEGER      0
#define ADC_FORMAT_FRACTIONAL   1

// ADC channels
#define ADC_CHANNEL_AN0     0       // rPICM4CB MKB2 AN input on RA0 -> AN0
#define ADC_CHANNEL_AN13    1       // rPICM4CB 1V8 CB AN input on RC1 -> Non remappeable
#define ADC_CHANNEL_AN14    2       // rPICM4CB 1V0 CB AN input on RC2 -> Non remappeable
#define ADC_CHANNEL_AN20    3       // rPICM4CB MKB1 AN input on RE0 -> ANC0
#define ADC_CHANNEL_AN23    4       // rPICM4CB 12V0 CB AN input on RE3 -> ANN3
#define ADC_CHANNEL_AN24    5       // rPICM4CB 5V0 CB AN input on RF6 -> Non remappeable
#define ADC_CHANNEL_AN25    6       // rPICM4CB 3V3 CB AN input on RF7 -> Non remappeable
#define ADC_QTY 7

typedef struct
{
    uint16_t value;
    uint8_t state;
    uint8_t channel;
    uint8_t resolution;
    uint8_t sample_ready;
}STRUCT_ADC;

int8_t ADC_init_struct (STRUCT_ADC *adc, uint8_t channel);
int8_t ADC_init (STRUCT_ADC *adc);
int8_t ADC_start (void);
int8_t ADC_stop (void);
int8_t ADC_set_channel (STRUCT_ADC *adc);
uint8_t ADC_sample_status (STRUCT_ADC *adc);
uint16_t ADC_get_raw_channel (STRUCT_ADC *adc);
uint16_t ADC_get_eng_channel (STRUCT_ADC *adc);

#endif	

