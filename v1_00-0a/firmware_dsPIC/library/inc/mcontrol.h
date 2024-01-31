#ifndef __mcontrol_h__
#define __mcontrol_h__

dspic_33ck_generic
#include "pwm.h"
#include "qei.h"

typedef struct
{
    uint8_t pid_out;
    double pid_p_gain;
    double pid_i_gain;
    double pid_d_gain;
    uint16_t pid_fs;
    double pid_T;
    double p_calc_gain;
    double i_calc_gain;
    double d_calc_gain;
    double i_value;          
    double p_value; 
    double d_value;
    double d_input;
    double i_term;
    double pid_float;

    int32_t test_error;
    
    uint16_t pid_high_limit;
    double error_rpm;
    double last_error_rpm;
    uint16_t actual_rpm;
    uint16_t last_actual_rpm;    
    
    uint16_t max_rpm;
    uint16_t min_rpm;
    
    uint16_t speed_rpm;
    
    uint8_t speed_perc;
    uint8_t direction;
    uint8_t pwm_h_channel;
    uint8_t pwm_l_channel;
    uint8_t qei_channel;
    
    STRUCT_PWM *pwm_h_ref;        // Reference to a PWM struct
    STRUCT_PWM *pwm_l_ref;        // Reference to a PWM struct
}STRUCT_MCONTROL;

#define MOTOR_1             0
#define MOTOR_2             1
#define MOTOR_QTY           2

#define DIRECTION_FORWARD   0
#define DIRECTION_BACKWARD  1

void MOTOR_init (STRUCT_PWM *pwm_h, STRUCT_PWM *pwm_l, uint8_t channel, uint16_t speed_fs);
void MOTOR_drive_perc (uint8_t channel, uint8_t direction, uint8_t perc);
void MOTOR_set_rpm (uint8_t channel, uint16_t new_rpm);
void MOTOR_pid_calc_gains (uint8_t channel);
double MOTOR_get_error (uint8_t channel);
uint8_t MOTOR_get_direction (uint8_t channel);
uint16_t MOTOR_get_speed_rpm (uint8_t channel);
uint8_t MOTOR_get_speed_perc (uint8_t channel);
uint16_t MOTOR_get_setpoint_rpm (uint8_t channel);
void MOTOR_set_direction (uint8_t channel, uint8_t direction);
int32_t MOTOR_get_test_error (uint8_t channel);
//char MOTOR_drive_pid (uint8_t channel);
uint8_t MOTOR_drive_pid (uint8_t channel);
#endif