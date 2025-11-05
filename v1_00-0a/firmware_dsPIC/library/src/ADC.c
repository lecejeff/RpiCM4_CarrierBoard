//***************************************************************************//
// File      : ADC.c
//
// Functions :  
//
// Includes  :  ADC.h
//           
// Purpose   :  Driver for the dsPIC33CK ADC core
//
//Jean-Francois Bilodeau    MPLab X v6.05    24/06/2024  
//****************************************************************************//
#include "ADC.h"
STRUCT_ADC ADC_struct[ADC_QTY];

int8_t ADC_init_struct (STRUCT_ADC *adc, uint8_t channel)
{
    switch (channel)
    {
        case ADC_CHANNEL_AN0:       // rPICM4CB MKB2 AN input on RA0
            adc->channel = ADC_CHANNEL_AN0;
            break;

        case ADC_CHANNEL_AN13:      // rPICM4CB 1V8 CB AN input on RC1
            adc->channel = ADC_CHANNEL_AN13;
            break;

        case ADC_CHANNEL_AN14:      // rPICM4CB 1V0 CB AN input on RC2
            adc->channel = ADC_CHANNEL_AN14;
            break;

        case ADC_CHANNEL_AN20:      // rPICM4CB MKB1 AN input on RE0
            adc->channel = ADC_CHANNEL_AN20;
            break;

        case ADC_CHANNEL_AN23:      // rPICM4CB 12V0 CB AN input on RE3
            adc->channel = ADC_CHANNEL_AN23;
            break;

        case ADC_CHANNEL_AN24:      // rPICM4CB 5V0 CB AN input on RF6
            adc->channel = ADC_CHANNEL_AN24;
            break;

        case ADC_CHANNEL_AN25:      // rPICM4CB 3V3 CB AN input on RF7
            adc->channel = ADC_CHANNEL_AN25;
            break;
            
        default:
            return -1;
            break;
    } 
    
    adc->state = ADC_INSTANTIATED;
    return 0;
}

//*************************void ADC_init (void)*******************************//
//Description : Function initializes the dsPIC ADC core
//
//Function prototype : void ADC_init (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_init();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
int8_t ADC_init (STRUCT_ADC *adc)
{
    // Initialize dsPIC pins to analog inputs
    switch (adc->channel)
    {
        case ADC_CHANNEL_AN0:       // rPICM4CB MKB2 AN input on RA0
            ADMOD0Lbits.SIGN0 = 0;  // unsigned output data
            ADMOD0Lbits.DIFF0 = 0;  // channel is single-ended
            TRISAbits.TRISA0 = 1;
            ANSELAbits.ANSELA0 = 1;
            
            ADIELbits.IE0 = 1;
            IEC5bits.ADCAN0IE = 1;
            IFS5bits.ADCAN0IF = 0;
            break;

        case ADC_CHANNEL_AN13:      // rPICM4CB 1V8 CB AN input on RC1
            ADMOD0Hbits.SIGN13 = 0; // unsigned output data
            ADMOD0Hbits.DIFF13 = 0; // channel is single-ended
            TRISCbits.TRISC1 = 1;
            ANSELCbits.ANSELC1 = 1;
            
            ADIELbits.IE13 = 1;
            IEC6bits.ADCAN13IE = 1;
            IFS6bits.ADCAN13IF = 0;
            break;

        case ADC_CHANNEL_AN14:      // rPICM4CB 1V0 CB AN input on RC2
            ADMOD0Hbits.SIGN14 = 0; // unsigned output data
            ADMOD0Hbits.DIFF14 = 0; // channel is single-ended
            TRISCbits.TRISC2 = 1;
            ANSELCbits.ANSELC2 = 1;
            
            ADIELbits.IE14 = 1;
            IEC6bits.ADCAN14IE = 1;
            IFS6bits.ADCAN14IF = 0;
            break;

        case ADC_CHANNEL_AN20:      // rPICM4CB MKB1 AN input on RE0
            ADMOD1Lbits.SIGN20 = 0; // unsigned output data
            ADMOD1Lbits.DIFF20 = 0; // channel is single-ended
            TRISEbits.TRISE0 = 1;
            ANSELEbits.ANSELE0 = 1;
            
            ADIEHbits.IE20 = 1;
            IEC6bits.ADCAN20IE = 1;
            IFS6bits.ADCAN20IF = 0;
            break;

        case ADC_CHANNEL_AN23:      // rPICM4CB 12V0 CB AN input on RE3
            ADMOD1Lbits.SIGN23 = 0; // unsigned output data
            ADMOD1Lbits.DIFF23 = 0; // channel is single-ended
            TRISEbits.TRISE3 = 1; 
            ANSELEbits.ANSELE3 = 1; 
            
            ADIEHbits.IE23 = 1;
            IEC7bits.ADCAN23IE = 1;
            IFS7bits.ADCAN23IF = 0;
            break;

        case ADC_CHANNEL_AN24:      // rPICM4CB 5V0 CB AN input on RF6
            ADMOD1Hbits.SIGN24 = 0; // unsigned output data
            ADMOD1Hbits.DIFF24 = 0; // channel is single-ended
            TRISFbits.TRISF6 = 1; 
            ANSELFbits.ANSELF6 = 1;
            
            ADIEHbits.IE24 = 1;
            IEC12bits.ADCAN24IE = 1;
            IFS12bits.ADCAN24IF = 0;
            break;

        case ADC_CHANNEL_AN25:      // rPICM4CB 3V3 CB AN input on RF7
            ADMOD1Hbits.SIGN25 = 0; // unsigned output data
            ADMOD1Hbits.DIFF25 = 0; // channel is single-ended
            TRISFbits.TRISF7 = 1; 
            ANSELFbits.ANSELF7 = 1;
            
            ADIEHbits.IE25 = 1;
            IEC12bits.ADCAN25IE = 1;
            IFS12bits.ADCAN25IF = 0;
            break;
            
        default:
            return -1;
            break;
    } 
    
    ADCON1Hbits.FORM = 0;   // Unsigned for all channels
    ADCON1Hbits.SHRRES = 3; // 12b for all channels
    adc->state = ADC_INITIALIZED;
    return 0;
}

int8_t ADC_set_channel (STRUCT_ADC *adc)
{
    switch (adc->channel)
    {
        case ADC_CHANNEL_AN0:       // rPICM4CB MKB2 AN input on RA0
            ADCON3Lbits.CNVCHSEL = 0;   
            break;

        case ADC_CHANNEL_AN13:      // rPICM4CB 1V8 CB AN input on RC1
            ADCON3Lbits.CNVCHSEL = 13;  
            break;

        case ADC_CHANNEL_AN14:      // rPICM4CB 1V0 CB AN input on RC2
            ADCON3Lbits.CNVCHSEL = 14; 
            break;

        case ADC_CHANNEL_AN20:      // rPICM4CB MKB1 AN input on RE0
            ADCON3Lbits.CNVCHSEL = 20; 
            break;

        case ADC_CHANNEL_AN23:      // rPICM4CB 12V0 CB AN input on RE3
            ADCON3Lbits.CNVCHSEL = 23; 
            break;

        case ADC_CHANNEL_AN24:      // rPICM4CB 5V0 CB AN input on RF6
            ADCON3Lbits.CNVCHSEL = 24; 
            break;

        case ADC_CHANNEL_AN25:      // rPICM4CB 3V3 CB AN input on RF7
            ADCON3Lbits.CNVCHSEL = 25; 
            break;
            
        default:
            return -1;
            break;
    }
    return 0;    
}

//************************void ADC_start (void)*******************************//
//Description : Function starts the dsPIC ADC core and enables interrupt
//
//Function prototype : void ADC_start (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_start();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
int8_t ADC_start (void)
{
    ADCON5Hbits.WARMTIME = 15;
    ADCON1Lbits.ADON = 1; 
    ADCON5Lbits.SHRPWR = 1;
    while (ADCON5Lbits.SHRRDY == 0);
    ADCON3Hbits.SHREN = 1; 
    return 0;
}

//*************************void ADC_stop (void)*******************************//
//Description : Function stops the dsPIC ADC core and disables interrupt
//
//Function prototype : void ADC_stop (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_stop();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
int8_t ADC_stop (void)
{
    ADCON1Lbits.ADON = 0;       // Stop the global ADC core   
    ADCON3Hbits.SHREN = 0;
    ADCON5Lbits.SHRPWR = 0;    
    return 0;
}

//***************uint8_t ADC_sample_status (void)***********************//
//Description : Function returns 1 when a new ADC sample is ready to be read
//
//Function prototype : uint8_t ADC_sample_status (void)
//
//Enter params       : None
//
//Exit params        : uint8_t : sample status (1 = ready)
//
//Function call      : uint8_t = ADC_sample_status();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
uint8_t ADC_sample_status (STRUCT_ADC *adc)
{
    switch(adc->channel)
    {
        case ADC_CHANNEL_AN0:       // rPICM4CB MKB2 AN input on RA0
            return ADSTATLbits.AN0RDY;  
            break;

        case ADC_CHANNEL_AN13:      // rPICM4CB 1V8 CB AN input on RC1
            return ADSTATLbits.AN13RDY;   
            break;

        case ADC_CHANNEL_AN14:      // rPICM4CB 1V0 CB AN input on RC2
            return ADSTATLbits.AN14RDY; 
            break;

        case ADC_CHANNEL_AN20:      // rPICM4CB MKB1 AN input on RE0
            return ADSTATHbits.AN20RDY; 
            break;

        case ADC_CHANNEL_AN23:      // rPICM4CB 12V0 CB AN input on RE3
            return ADSTATHbits.AN23RDY; 
            break;

        case ADC_CHANNEL_AN24:      // rPICM4CB 5V0 CB AN input on RF6
            return ADSTATHbits.AN24RDY;  
            break;

        case ADC_CHANNEL_AN25:      // rPICM4CB 3V3 CB AN input on RF7
            return ADSTATHbits.AN25RDY; 
            break;
        
        default:
            return 0;
            break;
    }
}

//**********uint16_t ADC_get_channel (uint8_t channel)**************//
//Description : Function returns the value of the selected ADC channel
//
//Function prototype : uint16_t ADC_get_channel (uint8_t channel)
//
//Enter params       : uint8_t channel : ADC channel to read (see ADC.h)
//
//Exit params        : uint16_t : sample value between 0 and 2^12 - 1
//
//Function call      : uint16_t = ADC_get_channel(ADC_MOTOR_1_CHANNEL);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
uint16_t ADC_get_raw_channel (STRUCT_ADC *adc)
{
    switch(adc->channel)
    {     
        case ADC_CHANNEL_AN0:       // rPICM4CB MKB2 AN input on RA0
            adc->sample_ready = 0;
            return adc->value;  
            break;

        case ADC_CHANNEL_AN13:      // rPICM4CB 1V8 CB AN input on RC1
            adc->sample_ready = 0;
            return adc->value; 
            break;

        case ADC_CHANNEL_AN14:      // rPICM4CB 1V0 CB AN input on RC2
            adc->sample_ready = 0;
            return adc->value; 
            break;

        case ADC_CHANNEL_AN20:      // rPICM4CB MKB1 AN input on RE0
            adc->sample_ready = 0;
            return adc->value; 
            break;

        case ADC_CHANNEL_AN23:      // rPICM4CB 12V0 CB AN input on RE3
            adc->sample_ready = 0;
            return adc->value; 
            break;

        case ADC_CHANNEL_AN24:      // rPICM4CB 5V0 CB AN input on RF6
            adc->sample_ready = 0;
            return adc->value; 
            break;

        case ADC_CHANNEL_AN25:      // rPICM4CB 3V3 CB AN input on RF7
            adc->sample_ready = 0;
            return adc->value; 
            break;
        
        default:
            return 0;
            break;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN0Interrupt(void)
{
    IFS5bits.ADCAN0IF = 0;
    ADC_struct[ADC_CHANNEL_AN0].value = ADCBUF0;
    ADC_struct[ADC_CHANNEL_AN0].sample_ready = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN13Interrupt(void)
{
    IFS6bits.ADCAN13IF = 0;
    ADC_struct[ADC_CHANNEL_AN13].value = ADCBUF13;
    ADC_struct[ADC_CHANNEL_AN13].sample_ready = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN14Interrupt(void)
{
    IFS6bits.ADCAN14IF = 0;
    ADC_struct[ADC_CHANNEL_AN14].value = ADCBUF14;
    ADC_struct[ADC_CHANNEL_AN14].sample_ready = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN20Interrupt(void)
{
    IFS6bits.ADCAN20IF = 0;
    ADC_struct[ADC_CHANNEL_AN20].value = ADCBUF20;
    ADC_struct[ADC_CHANNEL_AN20].sample_ready = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN23Interrupt(void)
{
    IFS7bits.ADCAN23IF = 0;
    ADC_struct[ADC_CHANNEL_AN23].value = ADCBUF23;
    ADC_struct[ADC_CHANNEL_AN23].sample_ready = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN24Interrupt(void)
{
    IFS12bits.ADCAN24IF = 0;
    ADC_struct[ADC_CHANNEL_AN24].value = ADCBUF24;
    ADC_struct[ADC_CHANNEL_AN24].sample_ready = 1;
}

void __attribute__((__interrupt__, no_auto_psv))_ADCAN25Interrupt(void)
{
    IFS12bits.ADCAN25IF = 0;
    ADC_struct[ADC_CHANNEL_AN25].value = ADCBUF25;
    ADC_struct[ADC_CHANNEL_AN25].sample_ready = 1;
}