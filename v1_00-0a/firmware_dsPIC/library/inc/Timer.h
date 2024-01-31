//****************************************************************************//
// File      :  timer.h
//
// Functions :  
//
// Includes  :  dspic_33ck_generic.h
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
#ifndef __TIMER_H_
#define	__TIMER_H_

#include "dspic_33ck_generic.h"

#define TIMER_1             0

#define TIMER_QTY           1

#define TIMER_RUN_STATE     0
#define TIMER_INT_STATE     1

#define TIMER_MODE_16B      0
#define TIMER_MODE_32B      1
#define TIMER_COUNTER       2

#define TIMER_PRESCALER_1   0
#define TIMER_PRESCALER_8   1
#define TIMER_PRESCALER_64  2
#define TIMER_PRESCALER_256 3

#define ALL_TIMER_PRESCALER 8

typedef struct
{
    uint8_t TIMER_channel;
    uint8_t int_state;
    uint32_t freq;
    uint8_t prescaler;
    uint8_t running;
    uint8_t mode;
}STRUCT_TIMER;

typedef struct
{
    uint8_t COUNTER_channel;
    uint8_t int_state;
    uint32_t freq;
    uint8_t prescaler;
    uint8_t running;
    uint8_t type;
}STRUCT_COUNTER;

uint8_t TIMER_init (STRUCT_TIMER *timer, uint8_t channel, uint8_t mode, uint8_t prescaler, uint32_t freq);
uint8_t TIMER_update_freq (STRUCT_TIMER *timer, uint8_t prescaler, uint32_t new_freq);
uint32_t TIMER_get_freq (STRUCT_TIMER *timer);
uint8_t TIMER_start (STRUCT_TIMER *timer);
uint8_t TIMER_stop (STRUCT_TIMER *timer);
uint8_t TIMER_get_state (STRUCT_TIMER *timer, uint8_t type);

uint8_t COUNTER_init (STRUCT_TIMER *timer, uint8_t channel, uint8_t mode, uint8_t prescaler, uint32_t freq);
#endif	/* TIMER_H */

