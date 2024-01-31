//****************************************************************************//
// File      :  timer.c
//
// Functions :  
//
// Includes  :  timer.h
//
// Purpose   :  Driver for the dsPIC33CK TIMER core
//              1x dedicated timer on dsPIC33CK
//              Timer 1
//              
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#include "Timer.h"

STRUCT_TIMER TIMER_struct[TIMER_QTY];

//uint8_t TIMER_init (STRUCT_TIMER *timer, uint8_t channel, uint8_t mode, uint8_t prescaler, uint32_t freq)//
//Description           :   Initializes timer structure    
//
//Function prototype    :   uint8_t TIMER_init (STRUCT_TIMER *timer, uint8_t channel, uint8_t mode, uint8_t prescaler, uint32_t freq)
//
//Enter params          :   STRUCT_TIMER *timer
//                          uint8_t channel
//                          uint8_t mode
//                          uint8_t prescaler
//                          uint32_t freq
//
//Exit params           :   uint8_t
//
//Function call         :   
//
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
uint8_t TIMER_init (STRUCT_TIMER *timer, uint8_t channel, uint8_t mode, uint8_t prescaler, uint32_t freq)
{
    if (freq < 1){freq = 1;}    // Cannot divide by 0, otherwise will cause math error trap
    
    // With 1x prescaler, Timer clock = Fcy = 100MHz
    // 16b mode, minimum timer frequency : 100000000 / freq, 1 <= freq <= 65535 = 1069Hz
    // 32b mode, minimum timer frequency : 100000000 / freq, 1 <= freq <= (2^32)-1 = 0.016Hz
    // Maximum timer frequency : 100MHz
    
    // With 8x prescaler, Timer clock is Fcy / 8 -> 12.5MHz
    // 16b mode, minimum timer frequency : 12.5MHz / freq, 1 <= freq <= 65535 = 134Hz
    // 32b mode, minimum timer frequency : 12.5MHz / freq, 1 <= freq <= (2^32)-1 = 0.002Hz
    // Maximum timer frequency : 12.5MHz
    
    // With 64x prescaler, Timer clock is Fcy / 64 -> 1.5625MHz
    // 16b mode, minimum timer frequency : 1.5625MHz / freq, 1 <= freq <= 65535 = 17Hz
    // 32b mode, minimum timer frequency : 1.5625MHz / freq, 1 <= freq <= (2^32)-1 = 0.000254Hz
    // Maximum timer frequency : 1.5625MHz
    
    // with 256x prescaler, Timer clock is Fcy / 256 -> 390625Hz
    // 16b mode, minimum timer frequency : 390625Hz / freq, 1 <= freq <= 65535 = 5.96Hz
    // 32b mode, minimum timer frequency : 390625Hz / freq, 1 <= freq <= (2^32)-1 = 0.0000633Hz
    // Maximum timer frequency : 390625Hz
    switch(channel)
    {
        case TIMER_1:  
            if (mode == TIMER_MODE_32B)
            {
                return 0;   // Error, Timer1 is the Type A timer and cannot be set to 32B mode
            }
            
            else if (mode == TIMER_MODE_16B)
            {
                T1CON = 0;                  // Clear Timer1 control register
                T1CONbits.TON = 0;          // Stop Timer
                T1CONbits.TCS = 0;          // Select internal instruction cycle clock
                T1CONbits.TGATE = 0;        // Disable Gated Timer mode
                T1CONbits.TCKPS = prescaler;// Apply prescaler
                TMR1 = 0;                   // Clear timer register
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR1 = (FCY / freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR1 = ((FCY / 8) / freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR1 = ((FCY / 64) / freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR1 = ((FCY / 256) / freq);
                        break;   
                        
                    default:
                        return 0;
                        break;
                }
            }
            
            else
                return 0;
            break;      
                                           
        default: 
            return 0;
            break;
    }
    
    timer->TIMER_channel = channel;
    timer->running = 0;
    timer->freq = freq;
    timer->prescaler = prescaler;
    timer->int_state = 0; 
    timer->mode = mode;
    return 1;
}

uint32_t TIMER_get_freq (STRUCT_TIMER *timer)
{
    return timer->freq;
}

//**********************void TIMER_start (uint8_t timer)**********************//
//Description : 
//
//Function prototype : 
//
//Enter params       : 
//
//Exit params        : 
//
//Function call      : 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_update_freq (STRUCT_TIMER *timer, uint8_t prescaler, uint32_t new_freq)
{
    switch (timer->TIMER_channel)
    {
        case TIMER_1:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;       // Timer1 cannot be used as a 32b timer
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TIMER_stop(timer);
                TMR1 = 0;
                T1CONbits.TCKPS = prescaler;
                timer->prescaler = prescaler;
                timer->freq = new_freq;
                switch (prescaler)
                {
                    case TIMER_PRESCALER_1:
                        PR1 = (FCY / new_freq); 
                        break;

                    case TIMER_PRESCALER_8:
                        PR1 = ((FCY / 8) / new_freq);
                        break;

                    case TIMER_PRESCALER_64:
                        PR1 = ((FCY / 64) / new_freq);
                        break;

                    case TIMER_PRESCALER_256:
                        PR1 = ((FCY / 256) / new_freq);
                        break; 
                        
                    default:
                        return 0;
                        break;
                }            
                TIMER_start(timer);
            }
            else
                return 0;
            break;
            
        default:
            return 0;
            break;
    }
    return 1;
}

//**********************void TIMER_start (uint8_t timer)**********************//
//Description : Function starts timer module
//
//Function prototype : void TIMER_start (uint8_t timer)
//
//Enter params       : uint8_t timer : TIMER_x module
//
//Exit params        : None
//
//Function call      : TIMER_start(TIMER_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_start (STRUCT_TIMER *timer)
{
    switch (timer->TIMER_channel)
    {
        case TIMER_1:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                TMR1 = 0;
                IEC0bits.T1IE = 1;      // Enable timer interrupt
                IFS0bits.T1IF = 0;      // Clear timer flag
                T1CONbits.TON = 1;      // Start timer      
            }
            else
                return 0;
            break;

        default:
            return 0;
            break;
    }    
    timer->running = 1;
    return 1;
}

//***********************void TIMER_stop (uint8_t timer)**********************//
//Description : Function stops timer module
//
//Function prototype : void TIMER_stop (uint8_t timer)
//
//Enter params       : uint8_t timer : TIMER_x module
//
//Exit params        : None
//
//Function call      : TIMER_stop(TIMER_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_stop (STRUCT_TIMER *timer)
{
    switch (timer->TIMER_channel)
    {
        case TIMER_1:
            if (timer->mode == TIMER_MODE_32B)
            {
                return 0;           // Invalid, Timer1 is 16b only
            }
            else if (timer->mode == TIMER_MODE_16B)
            {
                T1CONbits.TON = 0;      // Stop timer               
                IEC0bits.T1IE = 0;      // Disable timer interrupt
                IFS0bits.T1IF = 0;      // Clear timer flag       
            }
            else
                return 0;
            break;         
            
        default:
            return 0;
            break;
    }
    timer->running = 0; // Timer is stopped
    return 1;
}

//***********uint8_t TIMER_get_state (uint8_t timer, uint8_t type)************//
//Description : Function returns timer module state
//
//Function prototype : uint8_t TIMER_get_state (uint8_t timer)
//
//Enter params       : uint8_t timer : TIMER_x module
//
//Exit params        : uint8_t : state (0 stopped, 1 active)
//
//Function call      : uint8_t = TIMER_get_state(TIMER_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t TIMER_get_state (STRUCT_TIMER *timer, uint8_t type)
{
    switch (type)
    {
        case TIMER_RUN_STATE:
            return timer->running;
            break;
            
        case TIMER_INT_STATE:
            if (timer->int_state)
            {
                timer->int_state = 0;
                return 1;
            }
            else return 0;
            break;
            
        default:
            return 0;
            break;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    TIMER_struct[TIMER_1].int_state = 1;
}