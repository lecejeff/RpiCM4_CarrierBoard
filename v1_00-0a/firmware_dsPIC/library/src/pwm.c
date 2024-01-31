//****************************************************************************//
// File      : PWM.c
//
// Functions :  void PWM_init (void);
//              void PWM_change_duty (uint8_t channel, uint8_t duty);
//              uint8_t PWM_get_position (uint8_t channel);
//
// Includes  : pwm.h
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020   
//****************************************************************************//
#include "pwm.h"
STRUCT_PWM PWM_struct[PWM_QTY];

///**************************void PWM_init(void)****************************//
//Description : 
//
//Function prototype : void PWM_init(void)
//
//Enter params       : None
//
//Exit params        : None 
//
//Function call      : PWM_init;
//
//
///*****************************************************************************
int8_t PWM_init (STRUCT_PWM *pwm, uint8_t channel, uint8_t type)
{    
    if ((channel >= 0) && (channel < PWM_QTY))
    {
        pwm->PWM_channel = channel;  
        pwm->pwm_type = type;
    }
    else
        return -1;

#ifdef PWM_CLOCK_DIVIDE_2
    #define PWM_CLOCK_PRESCALE 2
    PCLKCONbits.DIVSEL = 0;
#elif PWM_CLOCK_DIVIDE_4
    #define PWM_CLOCK_PRESCALE 4
    PCLKCONbits.DIVSEL = 1;
#elif PWM_CLOCK_DIVIDE_8
    #define PWM_CLOCK_PRESCALE 8
    PCLKCONbits.DIVSEL = 2;
#elif PWM_CLOCK_DIVIDE_16
    #define PWM_CLOCK_PRESCALE 16
    PCLKCONbits.DIVSEL = 3;
#endif
    
    PCLKCONbits.MCLKSEL = 0;    // PWM Main CLK = Fosc
    APCLKCONbits.MCLKSEL = 0;   // APWM Main CLK = Fosc
    
    // For low-frequency servo-motors, scale-down the clock
    // Example for 50Hz servo, 20ms period
    // FOSC = 100MHz, 1x clock period = 10ns
    // Scale down by 500 -> 200kHz = 5us
    // Fscl = 1, Fsminper = 500, accumulator counts +1 500x before generating a clock pulse
    // F_Servo = (1/20) / (1/200000) = 4000
    FSCL = 1;
    FSMINPER = 500;
           
    switch (pwm->PWM_channel)
    {
        case PWM_1L:
            PG1CONLbits.ON = 0;     // Disable PWM generator
            PG1CONLbits.CLKSEL = 3; // PWM Main CLK derived from frequency scaling block
            PG1CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG1IOCONHbits.PENL = 1; // Enable PWM_1L
            PG1IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_1L_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            PG1PER = (uint16_t)(FOSC / (PWM1L_PHASE * FSMINPER));
            //PG1TRIGA = (uint16_t)(FOSC / (PWM1L_PHASE * PWM_CLOCK_PRESCALE));
            PG1TRIGB = (uint16_t)(FOSC / (PWM1L_PHASE * FSMINPER));
            
            // In load pwm->pwm_type, the PWM channel acts as a 0-100% variable duty cycle
            if (type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG1TRIGB;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG1TRIGB * PWM1L_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(PG1TRIGB * PWM1L_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_1H:
            PG1CONLbits.ON = 0;     // Disable PWM generator
            PG1CONLbits.CLKSEL = 3; // PWM Main CLK derived from frequency scaling block
            PG1CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG1IOCONHbits.PENH = 1; // Enable PWM_1H
            PG1IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_1H_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            PG1PER = (uint16_t)(FOSC / (PWM1H_PHASE * FSMINPER));
            //PG1PHASE = (uint16_t)(FOSC / (PWM1H_PHASE * PWM_CLOCK_PRESCALE));
            PG1DC = (uint16_t)(FOSC / (PWM1H_PHASE * FSMINPER));
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG1DC;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG1DC * PWM1H_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(PG1DC * PWM1H_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_2L:
            PG2CONLbits.ON = 0;     // Disable PWM generator 
            PG2CONLbits.CLKSEL = 2; // PWM Main CLK derived from clock divider circuit
            PG2CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG2IOCONHbits.PENL = 1; // Enable PWM_2L
            PG2IOCONHbits.PMOD = 1; // Operate in independent mode            
            
            PWM_2L_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            //PG2TRIGA = (uint16_t)(FOSC / (PWM2L_PHASE * PWM_CLOCK_PRESCALE));
            PG2PER = (uint16_t)(FOSC / (PWM2L_PHASE * PWM2L_CLOCK_SCALE));
            PG2TRIGB = (uint16_t)(FOSC / (PWM2L_PHASE * PWM2L_CLOCK_SCALE));
            PG2STATbits.UPDREQ = 1;
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG2TRIGB;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG2TRIGB * PWM2L_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(PG2TRIGB * PWM2L_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_2H:
            PG2CONLbits.ON = 0;     // Disable PWM generator  
            PG2CONLbits.CLKSEL = 2; // PWM Main CLK derived from clock divider circuit
            PG2CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG2IOCONHbits.PENH = 1; // Enable PWM_2H
            PG2IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_2H_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            //PG2PHASE = (uint16_t)(FOSC / (PWM2H_PHASE * PWM_CLOCK_PRESCALE));
            PG2PER = (uint16_t)(FOSC / (PWM2H_PHASE * PWM2H_CLOCK_SCALE));
            PG2DC = (uint16_t)(FOSC / (PWM2H_PHASE * PWM2H_CLOCK_SCALE));
            PG2STATbits.UPDREQ = 1;
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG2DC;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG2DC * PWM2H_SERVO_BASE);  // PWM base value is 1ms on a phase of 20ms                 
                pwm->end_value = (uint16_t)(PG2DC * PWM2H_SERVO_END);    // PWM end value is 2ms on a phase of 20ms                
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_5L:
            PG5CONLbits.ON = 0;     // Disable PWM generator
            PG5CONLbits.CLKSEL = 2; // PWM Main CLK derived from clock divider circuit
            PG5CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG5IOCONHbits.PENL = 1; // Enable PWM_5L
            PG5IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_5L_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            //PG5TRIGA = (uint16_t)(FOSC / (PWM5L_PHASE * PWM_CLOCK_PRESCALE));
            PG5PER = (uint16_t)(FOSC / (PWM5L_PHASE * PWM5L_CLOCK_SCALE));
            PG5TRIGB = (uint16_t)(FOSC / (PWM5L_PHASE * PWM5L_CLOCK_SCALE));
            PG5STATbits.UPDREQ = 1;
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG5TRIGB;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG5TRIGB * PWM5L_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PG5TRIGB * PWM5L_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }               
            break;
            
        case PWM_5H:
            PG5CONLbits.ON = 0;     // Disable PWM generator
            PG5CONLbits.CLKSEL = 2; // PWM Main CLK derived from clock divider circuit
            PG5CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG5IOCONHbits.PENH = 1; // Enable PWM_5H
            PG5IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_5H_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            //PG5PHASE = (uint16_t)(FOSC / (PWM5H_PHASE * PWM_CLOCK_PRESCALE));
            PG5PER = (uint16_t)(FOSC / (PWM5H_PHASE * PWM5H_CLOCK_SCALE));
            PG5DC = (uint16_t)(FOSC / (PWM5H_PHASE * PWM5H_CLOCK_SCALE));
            PG5STATbits.UPDREQ = 1;
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG5DC;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG5DC * PWM5H_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PG5DC * PWM5H_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }                     
            break;
            
        case PWM_7L:
            PG7CONLbits.ON = 0;     // Disable PWM generator
            PG7CONLbits.CLKSEL = 2; // PWM Main CLK derived from clock divider circuit
            PG7CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG7IOCONHbits.PENL = 1; // Enable PWM_7L
            PG7IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_7L_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            //PG7TRIGA = (uint16_t)(FOSC / (PWM7L_PHASE * PWM_CLOCK_PRESCALE));
            PG7PER = (uint16_t)(FOSC / (PWM7L_PHASE * PWM7L_CLOCK_SCALE));
            PG7TRIGB = (uint16_t)(FOSC / (PWM7L_PHASE * PWM7L_CLOCK_SCALE));
            PG7STATbits.UPDREQ = 1;
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG7TRIGB;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG7TRIGB * PWM7L_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PG7TRIGB * PWM7L_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }            
            break;
            
        case PWM_7H:
            PG7CONLbits.ON = 0;     // Disable PWM generator
            PG7CONLbits.CLKSEL = 2; // PWM Main CLK derived from clock divider circuit
            PG7CONLbits.MODSEL = 2; // Independent edge PWM mode, dual output
            PG7IOCONHbits.PENH = 1; // Enable PWM_7H
            PG7IOCONHbits.PMOD = 1; // Operate in independent mode
            
            PWM_7H_PIN = 0;         // Set associated PWM pin to output
            // The phase is defined in pwm.h for each PWM channel
            //PG7PHASE = (uint16_t)(FOSC / (PWM7H_PHASE * PWM_CLOCK_PRESCALE));
            PG7PER = (uint16_t)(FOSC / (PWM7H_PHASE * PWM7H_CLOCK_SCALE));
            PG7DC = (uint16_t)(FOSC / (PWM7H_PHASE * PWM7H_CLOCK_SCALE));
            PG7STATbits.UPDREQ = 1;
            
            // In load type, the PWM channel acts as a 0-100% variable duty cycle
            if (pwm->pwm_type == PWM_TYPE_LOAD)
            {
                pwm->base_value = 0;                 
                pwm->end_value = PG7DC;                
                pwm->range = pwm->end_value - pwm->base_value;   
            }
            // In servo type, the PWM channel duty cycle depends on the servo type
            else
            {
                pwm->base_value = (uint16_t)(PG7DC * PWM7H_SERVO_BASE);                
                pwm->end_value = (uint16_t)(PG7DC * PWM7H_SERVO_END);               
                pwm->range = pwm->end_value - pwm->base_value;                  
            }             
            break;
            
            default:
            break;
    } 

    return 0;
}

int8_t PWM_enable (STRUCT_PWM *pwm)
{
    switch (pwm->PWM_channel)
    {
        case PWM_1L:
            PG1CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_1H:
            PG1CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_2L:
            PG2CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_2H:
            PG2CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_5L:
            PG5CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_5H:
            PG5CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_7L:
            PG7CONLbits.ON = 1;     // Enable PWM generator
            break;
        case PWM_7H:
            PG7CONLbits.ON = 1;     // Enable PWM generator
            break;
        default:
            return -1;
            break;
    }
    return 0;
}

int8_t PWM_disable (STRUCT_PWM *pwm)
{
    switch (pwm->PWM_channel)
    {
        case PWM_1L:
            PG1CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_1H:
            PG1CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_2L:
            PG2CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_2H:
            PG2CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_5L:
            PG5CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_5H:
            PG5CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_7L:
            PG7CONLbits.ON = 0;     // Disable PWM generator
            break;
        case PWM_7H:
            PG7CONLbits.ON = 0;     // Disable PWM generator
            break;
        default:
            return -1;
            break;
    }
    return 0;
}
//**********uint8_t PWM_get_position (uint8_t channel)************//
//Description : Function returns actual duty cycle percentage of PWMx channel
//
//Function prototype : uint8_t PWM_get_position (uint8_t channel)
//
//Enter params       : uint8_t channel : PWMx channel
//
//Exit params        : uint8_t : actual position in percentage 
//
//Function call      : pwm_pos = PWM_get_position(PWM_1H);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
///*****************************************************************************
uint8_t PWM_get_position (STRUCT_PWM *pwm)
{
    return pwm->value_p;
}

//*******void PWM_change_duty (uint8_t channel, uint8_t duty)*****//
//Description : Function changes the duty cycle of the selected PWM channel
//
//Function prototype : void PWM_change_duty (uint8_t channel, uint8_t duty)
//
//Enter params       : uint8_t channel : PWMx channel
//                     uint8_t duty : new duty cycle
//
//Exit params        : None 
//
//Function call      : PWM_change_duty(PWM_1H, 30);
//
//Jean-Francois Bilodeau    MPLab X v5.45    30/01/2021  
///*****************************************************************************
void PWM_change_duty_perc (STRUCT_PWM *pwm, uint8_t duty)
{
    if (duty > 100){duty = 100;}

    if (pwm->pwm_type == PWM_TYPE_LOAD)
    {
        pwm->new_duty = (uint16_t)((pwm->range * (float)duty/100.0));                   // Change duty percentage;
    }
    else
    {
        pwm->new_duty = (uint16_t)((pwm->range * ((float)duty/100.0)) + pwm->base_value);// Change duty percentage                     
    }
    pwm->value_p = duty;     // Update struct variable with new percentage
    switch (pwm->PWM_channel)
    {        
        case PWM_1L:              
            PG1TRIGB = pwm->new_duty;   // Update duty cycle
            break;  

        case PWM_1H:
            PG1DC = pwm->new_duty;      // Update duty cycle
            break;                

        case PWM_2L:               
            PG2TRIGB = pwm->new_duty;   // Update duty cycle
            break;  

        case PWM_2H:
            PG2DC = pwm->new_duty;      // Update duty cycle
            break;                

        case PWM_5L:
            PG5TRIGB = pwm->new_duty;   // Update duty cycle
            break;

        case PWM_5H:
            PG5DC = pwm->new_duty;      // Update duty cycle
            break;

        case PWM_7L:
            PG7TRIGB = pwm->new_duty;   // Update duty cycle
            break;             

        case PWM_7H:
            PG7DC = pwm->new_duty;      // Update duty cycle
            break;

        default:    
            break;
    }
}

void PWM_request_update (STRUCT_PWM *pwm)
{
    if ((pwm->PWM_channel == PWM_1L) || (pwm->PWM_channel == PWM_1H))
    {
        PG1STATbits.UPDREQ = 1;
    }
    if ((pwm->PWM_channel == PWM_2L) || (pwm->PWM_channel == PWM_2H))
    {
        PG2STATbits.UPDREQ = 1;
    }
    if ((pwm->PWM_channel == PWM_5L) || (pwm->PWM_channel == PWM_5H))
    {
        PG5STATbits.UPDREQ = 1;
    }
    if ((pwm->PWM_channel == PWM_7L) || (pwm->PWM_channel == PWM_7H))
    {
        PG7STATbits.UPDREQ = 1;
    }
}