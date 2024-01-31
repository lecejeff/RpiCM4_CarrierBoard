#include "rc_dac.h"

STRUCT_RCDAC RCDAC_struct[RCDAC_QTY];

uint8_t RCDAC_init (STRUCT_RCDAC *rcdac, STRUCT_PWM *pwm, STRUCT_I2C *i2c, uint8_t port, uint16_t freq)
{
    uint8_t buf[3] = {0};
    
    switch (port)
    {
        case RCDAC_1:
            rcdac->RCDAC_channel = RCDAC_1;
            rcdac->PWM_channel = PWM_6L;
            buf[0] = RCDAC_1_ADR;
            buf[1] = 0x40;          // Default POR -> reset wiper to midscale
            buf[2] = 0;
            break;
            
        case RCDAC_2:
            rcdac->RCDAC_channel = RCDAC_2;
            rcdac->PWM_channel = PWM_6H;
            buf[0] = RCDAC_2_ADR;
            buf[1] = 0x40;          // Default POR -> reset wiper to midscale
            buf[2] = 0;
            break;
            
        default:
            return 0;
            break;           
    }
    
    rcdac->pwm_ref = pwm;
    rcdac->i2c_ref = i2c;
    rcdac->PWM_frequency = freq;
    PWM_init(rcdac->pwm_ref, rcdac->PWM_channel, PWM_TYPE_LOAD);
    
    I2C_master_write(rcdac->i2c_ref, buf, 3);
    return 1;
}

void RCDAC_update_wiper (STRUCT_RCDAC *rcdac, uint8_t wiper)
{
    uint8_t buf[3] = {0};
    switch(rcdac->RCDAC_channel)
    {
        case RCDAC_1:
            buf[0] = RCDAC_1_ADR;
            buf[1] = 0;
            buf[2] = wiper;
            break;
            
        case RCDAC_2:
            buf[0] = RCDAC_2_ADR;
            buf[1] = 0;
            buf[2] = wiper;
            break;
    }
    I2C_master_write(rcdac->i2c_ref, buf, 3);
}