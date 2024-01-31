//***************************************************************************//
// File      : ADC.h
//
// About     : dsPIC33E ADC module is a SAR (successive-approximation) type
//             The ADC1 module can be configured for either 10-bit @ 1.1Msps or
//             12-bit @ 500ksps, while the ADC2 module can only be configured
//             for 10-bit @ 1.1Msps operation             
//
// Functions : 
//
// Includes  : dspeak_generic.h
//           
// Purpose   : Driver for the dsPIC33EP ADC 10bits and 12bits module
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#ifndef __ADC_H_
#define	__ADC_H_

dspic_33ck_generic

#define ADC_SAMPLE_READY    1
#define ADC_SAMPLE_NOT_READY 0

#define ADC_PORT_1          0
#define ADC_PORT_2          1

// ADC states
#define ADC_STATE_INITIALIZED   1
#define ADC_STATE_ASSIGNED      2

// ADC resolution
#define ADC_RESOLUTION_10b  0
#define ADC_RESOLUTION_12b  1

// ADC states
#define ADC_INITIALIZED     0
#define ADC_CONV_IN_PROG    1
#define ADC_CONV_READY      2

// ADC output formats
#define ADC_FORMAT_SIGNED_FRACTIONAL    3
#define ADC_FORMAT_FRACTIONAL           2
#define ADC_FORMAT_SIGNED_INTEGER       1
#define ADC_FORMAT_UNSIGNED_INTEGER     0

// ADC channels
#define ADC_CHANNEL_AN0     0
#define ADC_CHANNEL_AN1     1
#define ADC_CHANNEL_AN2     2
#define ADC_CHANNEL_AN12    12
#define ADC_CHANNEL_AN13    13
#define ADC_CHANNEL_AN14    14
#define ADC_CHANNEL_AN15    15
#define ADC_QTY 7

// ADC sample source
// IF SSRCG = 0
#define ADC_AUTO_CONVERT        7
#define ADC_PWMSSET_CONVERT     5
#define ADC_TMR5_CONVERT        4
#define ADC_PWMPSET_CONVERT     3
#define ADC_TMR3_CONVERT        2
#define ADC_INT0_CONVERT        1
#define ADC_MANUAL_CONVERT      0
// IF SSRCG = 1
#define ADC_PWM7PTC_CONVERT     6
#define ADC_PWM6PTC_CONVERT     5
#define ADC_PWM5PTC_CONVERT     4
#define ADC_PWM4PTC_CONVERT     3
#define ADC_PWM3PTC_CONVERT     2
#define ADC_PWM2PTC_CONVERT     1
#define ADC_PWM1PTC_CONVERT     0

#define ADC_SSRCG_SET_0         0
#define ADC_SSRCG_SET_1         1

typedef struct
{
    uint16_t value;
    uint8_t state;
    uint8_t port;
    uint8_t channel;
    uint8_t resolution;
    uint8_t format;
    uint8_t sample_clk_src;
    uint8_t sample_clk_srcg;
    uint8_t sample_conv_clk;
}STRUCT_ADC;

void ADC_init_struct (STRUCT_ADC *adc, uint8_t port, uint8_t channel, 
                    uint8_t resolution, uint8_t format, uint8_t sample_clk_src,
                    uint8_t sample_clk_srcg, uint16_t sample_conv_clk);
void ADC_init (STRUCT_ADC *adc);
void ADC_start (uint8_t port);
void ADC_stop (uint8_t port);
uint8_t ADC_sample_status (STRUCT_ADC *adc);
uint16_t ADC_get_raw_channel (STRUCT_ADC *adc);
uint16_t ADC_get_eng_channel (STRUCT_ADC *adc);

#endif	

