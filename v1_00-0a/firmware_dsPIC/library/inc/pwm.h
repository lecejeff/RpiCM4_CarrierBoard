//****************************************************************************//
// File      : PWM.h
//
// Functions :  void PWM_init (void);
//              void PWM_change_duty (uint8_t channel, uint8_t duty);
//              uint8_t PWM_get_position (uint8_t channel);
//
// Includes  : dspic_33ck_generic.h
//
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#ifndef __PWM_H__
#define __PWM_H__

#include "dspic_33ck_generic.h"

#define PWM_1L  0   // Servo motor 1
#define PWM_1H  1   // Servo motor 2
#define PWM_2L  2   // APWM2L, Motor 1 L 
#define PWM_2H  3   // APWM2H, Motor 1 H
#define PWM_5L  4   // PWM5H, Motor 2L
#define PWM_5H  5   // PWM5L, Motor 2H
#define PWM_7L  6   // PWM7H, Motor 3L
#define PWM_7H  7   // PWM7L, Motor 3H
#define PWM_QTY 8

#define PWM_1L_PIN  TRISBbits.TRISB15
#define PWM_1H_PIN  TRISBbits.TRISB14
#define PWM_2L_PIN  TRISAbits.TRISA5
#define PWM_2H_PIN  TRISEbits.TRISE12
#define PWM_5L_PIN  TRISCbits.TRISC5
#define PWM_5H_PIN  TRISCbits.TRISC4
#define PWM_7L_PIN  TRISCbits.TRISC11
#define PWM_7H_PIN  TRISCbits.TRISC10

//#define PWM_CLOCK_DIVIDE_2      0
//#define PWM_CLOCK_DIVIDE_4      1
//#define PWM_CLOCK_DIVIDE_8      2
#define PWM_CLOCK_DIVIDE_16     3


#define PWM1H_PHASE         50L     // Phase in Hz
#define PWM1H_SERVO_BASE    0.045F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM1H_SERVO_END     0.105F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM1H_CLOCK_SCALE   10000   // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM1L_PHASE         50L     // Phase in Hz
#define PWM1L_SERVO_BASE    0.045F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM1L_SERVO_END     0.105F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM1L_CLOCK_SCALE   10000   // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM2H_PHASE         40000L
#define PWM2H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM2H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM2H_CLOCK_SCALE   31      // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM2L_PHASE         40000L
#define PWM2L_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM2L_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM2L_CLOCK_SCALE   31      // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM5H_PHASE         50L
#define PWM5H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM5H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM5H_CLOCK_SCALE   31      // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM5L_PHASE         50L
#define PWM5L_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM5L_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM5L_CLOCK_SCALE   31      // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM7H_PHASE         10000L
#define PWM7H_SERVO_BASE    0.05F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM7H_SERVO_END     0.1F    // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM7H_CLOCK_SCALE   31      // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM7L_PHASE         10000000L
#define PWM7L_SERVO_BASE    0.046F   // Ratio in % of phase, base duty cycle, irrelevant in load mode
#define PWM7L_SERVO_END     0.105F   // Ratio in % of phase, max duty cycle, irrelevant in load mode
#define PWM7L_CLOCK_SCALE   31      // Minimum divider for the period to fit Fosc / (phase * scaling) <= 2^16 - 1

#define PWM_TYPE_SERVO  0
#define PWM_TYPE_LOAD   1

typedef struct
{
    uint8_t PWM_channel;
    uint16_t range;
    uint16_t base_value;
    uint16_t end_value;
    uint8_t value_p;  
    uint16_t new_duty;
    uint8_t pwm_type;
}STRUCT_PWM;

int8_t PWM_init (STRUCT_PWM *pwm, uint8_t channel, uint8_t type);
void PWM_request_update (STRUCT_PWM *pwm);
int8_t PWM_enable (STRUCT_PWM *pwm);
int8_t PWM_disable (STRUCT_PWM *pwm);
void PWM_change_duty_perc (STRUCT_PWM *pwm, uint8_t duty);
uint8_t PWM_get_position (STRUCT_PWM *pwm);
#endif