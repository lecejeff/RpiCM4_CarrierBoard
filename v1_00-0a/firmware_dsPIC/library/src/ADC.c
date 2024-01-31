//***************************************************************************//
// File      : ADC.c
//
// Functions :  
//
// Includes  :  ADC.h
//           
// Purpose   :  Driver for the dsPIC33EP ADC core
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#include "ADC.h"
unsigned char adc1_init = 0;
unsigned char adc2_init = 0;
unsigned char adc1_sample_ready = ADC_SAMPLE_NOT_READY;
unsigned char adc2_sample_ready = ADC_SAMPLE_NOT_READY;

void ADC_init_struct (STRUCT_ADC *adc, uint8_t port, 
                        uint8_t channel, uint8_t resolution, uint8_t format, 
                        uint8_t sample_clk_src, uint8_t sample_clk_srcg,
                        uint16_t sample_conv_clk)
{
    adc->state = ADC_STATE_INITIALIZED;
    adc->port = port;
    adc->channel = channel;
    adc->resolution = resolution;
    adc->format = format;
    adc->sample_clk_src = sample_clk_src;
    adc->sample_clk_srcg = sample_clk_srcg;
    adc->sample_conv_clk = (uint8_t)(sample_conv_clk - 1);  // Between 1 and 256
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
void ADC_init (STRUCT_ADC *adc)
{
    // Code executed on 1st initialization of ADC1 state machine
    if (adc->port == ADC_PORT_1)
    {
        ADC_stop(ADC_PORT_1);                              // Stop ADC1 module if it was in use
        AD1CON2bits.VCFG = 0;                       // Converter voltage reference set to AVDD / AVSS
        AD1CON2bits.CSCNA = 1;                      // Scans input for CH0+
        AD1CON1bits.AD12B = adc->resolution;        // Setup ADC1 module resolution to 12b
        AD1CON1bits.FORM = adc->format;             // Setup ADC1 output format
        AD1CON1bits.SSRC = adc->sample_clk_src;     // Setup ADC1 sample clock source
        AD1CON1bits.SSRCG = adc->sample_clk_srcg;   // Setup ADC1 sample clock source group
        AD1CON3bits.ADRC = 0;                       // ADC clock always derived from system clock
        AD1CON3bits.ADCS = adc->sample_conv_clk;    // Set conversion clock
        AD1CON3bits.SAMC = 0x1F;
        if (adc1_init == 0)
        {
            adc1_init = 1;
        }
        else
        {
            AD1CON2bits.SMPI++; 
        }
        // Auto sample configuration
        if ((AD1CON1bits.SSRC == ADC_AUTO_CONVERT) && (AD1CON1bits.SSRCG == 0))
        {
            AD1CON1bits.ASAM = 1;
            if (adc->channel <= 15)
            {
                AD1CSSL = AD1CSSL | (1 << adc->channel);
            }
            else
            {
                AD1CSSH = AD1CSSH | (1 << (adc->channel-16));
            }
        }
        
        // Manual sample configuration
        if ((AD1CON1bits.SSRC == ADC_MANUAL_CONVERT) && (AD1CON1bits.SSRCG == 0))
        {
            
        }
    } 

    // Code executed on 1st initialization of ADC2 state machine
    if (adc->port == ADC_PORT_2)
    {
        ADC_stop(ADC_PORT_2);                              // Stop ADC2 module if it was in use
        AD2CON2bits.VCFG = 0;                       // Converter voltage reference set to AVDD / AVSS
        AD2CON2bits.CSCNA = 1;                      // Scans input for CH0+
        //AD2CON1bits.AD12B = adc->resolution;        // Setup ADC2 module resolution to 12b
        AD2CON1bits.FORM = adc->format;             // Setup ADC2 output format
        AD2CON1bits.SSRC = adc->sample_clk_src;     // Setup ADC2 sample clock source
        AD2CON1bits.SSRCG = adc->sample_clk_srcg;   // Setup ADC2 sample clock source group
        AD2CON3bits.ADRC = 0;                       // ADC clock always derived from system clock
        AD2CON3bits.ADCS = adc->sample_conv_clk;    // Set conversion clock
        AD2CON3bits.SAMC = 0x1F;  
        
        if (adc2_init == 0)
        {
            adc2_init = 1;
        }
        else
        {
            AD2CON2bits.SMPI++; 
        }        
        // Auto sample configuration
        if ((AD2CON1bits.SSRC == ADC_AUTO_CONVERT) && (AD2CON1bits.SSRCG == 0))
        {
            AD2CON1bits.ASAM = 1;
            if (adc->channel <= 15)
            {
                AD2CSSL = AD2CSSL | (1 << adc->channel);
            }
//            else
//            {
//                AD2CSSH = AD2CSSH | (1 << adc->channel);
//            }
        }
        
        // Manual sample configuration
        if ((AD2CON1bits.SSRC == ADC_MANUAL_CONVERT) && (AD2CON1bits.SSRCG == 0))
        {
            
        }
    }
    
    // Initialize dsPIC pins to analog inputs
    switch (adc->channel)
    {
        case ADC_CHANNEL_AN0:
            // AIN_AN0 (±3.3V analog input) on RB0
            TRISBbits.TRISB0 = 1;   // Set ANI_AN0 pin to input
            ANSELBbits.ANSB0 = 1;   // Set ANI_AN0 pin to analog input mode
            break;

        case ADC_CHANNEL_AN1:
            // AIN_AN1 (±3.3V analog input) on RB1
            TRISBbits.TRISB1 = 1;   // Set ANI_AN1 pin to input
            ANSELBbits.ANSB1 = 1;   // Set ANI_AN1 pin to analog input mode  
            break;

        case ADC_CHANNEL_AN2:
            // AIN_AN2 (3.3V analog potentiometer) on RB2
            TRISBbits.TRISB2 = 1;   // Set ANI_AN2 pin to input
            ANSELBbits.ANSB2 = 1;   // Set ANI_AN2 pin to analog input mode
            break;

        case ADC_CHANNEL_AN12:
            // AIN_AN12 (±10V analog input) on RB12
            TRISBbits.TRISB12 = 1;   // Set ANI_AN12 pin to input
            ANSELBbits.ANSB12 = 1;   // Set ANI_AN12 pin to analog input mode  
            break;

        case ADC_CHANNEL_AN13:
            // AIN_AN13 (±10V analog input) on RB13
            TRISBbits.TRISB13 = 1;   // Set ANI_AN13 pin to input
            ANSELBbits.ANSB13 = 1;   // Set ANI_AN13 pin to analog input mode   
            break;

        case ADC_CHANNEL_AN14:
            // AIN_AN14 (±10V analog input) on RB14
            TRISBbits.TRISB14 = 1;   // Set ANI_AN14 pin to input
            ANSELBbits.ANSB14 = 1;   // Set ANI_AN14 pin to analog input mode 
            break;

        case ADC_CHANNEL_AN15:
            // AIN_AN15 (±10V analog input) on RB15
            TRISBbits.TRISB15 = 1;   // Set ANI_AN15 pin to input
            ANSELBbits.ANSB15 = 1;   // Set ANI_AN15 pin to analog input mode 
            break;
    } 
    adc->state = ADC_STATE_ASSIGNED;
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
void ADC_start (uint8_t port)
{
    switch (port)
    {
        case ADC_PORT_1:
            IFS0bits.AD1IF = 0;     // Clear flag value
            IEC0bits.AD1IE = 1;     // Enable ADC interrupt
            AD1CON1bits.ADON = 1;   // Start the ADC core            
            break;
            
        case ADC_PORT_2:
            IFS1bits.AD2IF = 0;     // Clear flag value
            IEC1bits.AD2IE = 1;     // Enable ADC interrupt
            AD2CON1bits.ADON = 1;   // Start the ADC core                  
            break;
    }
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
void ADC_stop (uint8_t port)
{
    switch (port)
    {
        case ADC_PORT_1:
            AD1CON1bits.ADON = 0;   // Stop the ADC core   
            IEC0bits.AD1IE = 0;     // Disable ADC interrupt
            IFS0bits.AD1IF = 0;     // Clear flag value                               
            break;
            
        case ADC_PORT_2:
            AD2CON1bits.ADON = 0;   // Stop the ADC core   
            IEC1bits.AD2IE = 0;     // Disable ADC interrupt
            IFS1bits.AD2IF = 0;     // Clear flag value                    
            break;
    }
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
    switch(adc->port)
    {
        case ADC_PORT_1:
            if (adc1_sample_ready == ADC_SAMPLE_READY)
            {
                adc1_sample_ready = ADC_SAMPLE_NOT_READY;   // Clear flag
                return ADC_SAMPLE_READY;                        // Sample ready to be read
            }
            else
                return ADC_SAMPLE_NOT_READY;
            break;
            
        case ADC_PORT_2:
            if (adc2_sample_ready == ADC_SAMPLE_READY)
            {
                adc2_sample_ready = ADC_SAMPLE_NOT_READY;   // Clear flag
                return ADC_SAMPLE_READY;                        // Sample ready to be read
            } 
            else
                return ADC_SAMPLE_NOT_READY;            
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
    switch (adc->port)
    {
        case ADC_PORT_1:
            switch(adc->channel)
            {     
                case ADC_CHANNEL_AN12:
                    return ADC1BUF0;
                    break;
                case ADC_CHANNEL_AN13:
                    return ADC1BUF1;
                    break;
                case ADC_CHANNEL_AN14:
                    return ADC1BUF2;
                    break;
                case ADC_CHANNEL_AN15:
                    return ADC1BUF3;
                    break;            
                default:
                    return 0;
                    break;
            }
            break;
            
        case ADC_PORT_2:
            switch(adc->channel)
            {     
                case ADC_CHANNEL_AN0:
                    return ADC2BUF0;
                    break;
                case ADC_CHANNEL_AN1:
                    return ADC2BUF1;
                    break;
                case ADC_CHANNEL_AN2:
                    return ADC2BUF2;
                    break;        
                default:
                    return 0;
                    break;
            }
            break;

        default:
            return 0;
            break;            
    }
}

uint16_t ADC_get_eng_channel (STRUCT_ADC *adc)
{
    switch (adc->port)
    {
        case ADC_PORT_1:
            switch(adc->channel)
            {     
                case ADC_CHANNEL_AN12:
                    return ADC1BUF0;
                    break;
                case ADC_CHANNEL_AN13:
                    return ADC1BUF1;
                    break;
                case ADC_CHANNEL_AN14:
                    return ADC1BUF2;
                    break;
                case ADC_CHANNEL_AN15:
                    return ADC1BUF3;
                    break;            
                default:
                    return 0;
                    break;
            }
            break;
            
        case ADC_PORT_2:
            switch(adc->channel)
            {     
                case ADC_CHANNEL_AN0:
                    return ADC2BUF0;
                    break;
                case ADC_CHANNEL_AN1:
                    return ADC2BUF1;
                    break;
                case ADC_CHANNEL_AN2:
                    return ADC2BUF2;
                    break;         
                default:
                    return 0;
                    break;
            }
            break;

        default:
            return 0;
            break;            
    }    
}


void __attribute__((__interrupt__, no_auto_psv))_AD1Interrupt(void)
{
    IFS0bits.AD1IF = 0; 
    adc1_sample_ready = ADC_SAMPLE_READY;
}

void __attribute__((__interrupt__, no_auto_psv))_AD2Interrupt(void)
{
    IFS1bits.AD2IF = 0;
    adc2_sample_ready = ADC_SAMPLE_READY;
}