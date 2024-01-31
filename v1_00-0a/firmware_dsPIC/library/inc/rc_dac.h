#ifndef __rc_dac_h__
#define __rc_dac_h__

#include "dspic_33ck_generic.h"
#include "i2c.h"
#include "pwm.h"

#define RCDAC_1     0
#define RCDAC_2     1

#define RCDAC_QTY   2

#define RCDAC_1_ADR 0x5A
#define RCDAC_2_ADR 0x58

typedef struct
{
    uint8_t RCDAC_channel;
    uint8_t PWM_channel;
    uint16_t PWM_frequency;
    uint16_t PWM_duty_cycle;
    uint8_t wiper;
    
    STRUCT_PWM *pwm_ref;
    STRUCT_I2C *i2c_ref;
}STRUCT_RCDAC;

uint8_t RCDAC_init (STRUCT_RCDAC *rcdac, STRUCT_PWM *pwm, STRUCT_I2C *i2c, uint8_t port, uint16_t freq);
void RCDAC_enable_output (STRUCT_RCDAC *rcdac);
void RCDAC_disable_output (STRUCT_RCDAC *rcdac);
void RCDAC_update_wiper (STRUCT_RCDAC *rcdac, uint8_t wiper);
uint8_t RCDAC_get_wiper (STRUCT_RCDAC *rcdac);

#endif