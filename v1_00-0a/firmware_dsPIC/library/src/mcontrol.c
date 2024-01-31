#include "mcontrol.h"
STRUCT_MCONTROL m_control[MOTOR_QTY];

void MOTOR_init (STRUCT_PWM *pwm_h, STRUCT_PWM *pwm_l, uint8_t channel, uint16_t speed_fs)
{
    m_control[channel].pwm_h_ref = pwm_h;
    m_control[channel].pwm_l_ref = pwm_l;
    switch (channel)
    {
        case MOTOR_1:           
            PWM_init(pwm_l, PWM_1L, PWM_TYPE_LOAD);    // Configure PWM_1L pin to output and enable PWM
            PWM_init(pwm_h, PWM_1H, PWM_TYPE_LOAD);    // Configure PWM_1H pin to output and enable PWM
            QEI_init(QEI_1);                    // Configure QEI_1 
            QEI_set_fs(QEI_1, speed_fs);        // Set QEI channel velocity refresh rate
            TRISGbits.TRISG14 = 1;              // RG14 configured as an input (nFAULT1)
            m_control[channel].direction = DIRECTION_FORWARD;
            m_control[channel].speed_perc = 0;    
            m_control[channel].speed_rpm = 0;    
            m_control[channel].pwm_h_channel = PWM_1H;
            m_control[channel].pwm_l_channel = PWM_1L;
            m_control[channel].qei_channel = QEI_1;
            
            m_control[channel].pid_fs = QEI_get_fs(QEI_1);
            m_control[channel].pid_p_gain = 1.1;
            m_control[channel].pid_i_gain = 4.2;
            m_control[channel].pid_d_gain = 0;
            m_control[channel].max_rpm = QEI_get_max_rpm(QEI_1);
            m_control[channel].min_rpm = 0;
            m_control[channel].pid_high_limit = 100;        // Max output is 100% ON duty cycle to PWM
            MOTOR_pid_calc_gains(MOTOR_1);
            break;
            
        case MOTOR_2:
            PWM_init(pwm_l, PWM_2L, PWM_TYPE_LOAD);    // Configure PWM_2L pin to output and enable PWM
            PWM_init(pwm_h, PWM_2H, PWM_TYPE_LOAD);    // Configure PWM_2H pin to output and enable PWM
            QEI_init(QEI_2);                    // Configure QEI_2
            QEI_set_fs(QEI_2, speed_fs);        // Set QEI channel velocity refresh rate
            TRISEbits.TRISE4 = 1;               // RE4 configured as an input (nFAULT2)
            
            m_control[channel].direction = DIRECTION_FORWARD;
            m_control[channel].speed_perc = 0;    
            m_control[channel].speed_rpm = 0;    
            m_control[channel].pwm_h_channel = PWM_2H;
            m_control[channel].pwm_l_channel = PWM_2L;
            m_control[channel].qei_channel = QEI_2;
            
            m_control[channel].pid_fs = QEI_get_fs(QEI_2);
            m_control[channel].pid_p_gain = 1.1;
            m_control[channel].pid_i_gain = 4.2;
            m_control[channel].pid_d_gain = 0;
            m_control[channel].max_rpm = QEI_get_max_rpm(QEI_2);
            m_control[channel].min_rpm = 0;
            m_control[channel].pid_high_limit = 100;        // Max output is 100% ON duty cycle to PWM
            MOTOR_pid_calc_gains(MOTOR_2);
            break;
    }
}

uint8_t MOTOR_get_direction (uint8_t channel)
{
    return m_control[channel].direction;
}

uint16_t MOTOR_get_speed_rpm (uint8_t channel)
{
    return m_control[channel].actual_rpm;
}

uint16_t MOTOR_get_setpoint_rpm (uint8_t channel)
{
    return m_control[channel].speed_rpm;
}

uint8_t MOTOR_get_speed_perc (uint8_t channel)
{
    return m_control[channel].speed_perc;
}

void MOTOR_drive_perc (uint8_t channel, uint8_t direction, uint8_t perc)
{
    m_control[channel].direction = direction;
    m_control[channel].speed_perc = perc;        
    if (m_control[channel].direction == DIRECTION_FORWARD)
    {
        PWM_change_duty_perc(m_control[channel].pwm_h_ref, m_control[channel].speed_perc);
        PWM_change_duty_perc(m_control[channel].pwm_l_ref, 0);        
    }
    if (m_control[channel].direction == DIRECTION_BACKWARD)
    {
        PWM_change_duty_perc(m_control[channel].pwm_h_ref,  0);
        PWM_change_duty_perc(m_control[channel].pwm_l_ref,  m_control[channel].speed_perc);        
    }
}

void MOTOR_set_rpm (uint8_t channel, uint16_t new_rpm)
{
    m_control[channel].speed_rpm = new_rpm;
}

void MOTOR_pid_calc_gains (uint8_t channel)
{
    m_control[channel].pid_T = 1.0 / (double)m_control[channel].pid_fs;
    m_control[channel].p_calc_gain = m_control[channel].pid_p_gain;
    m_control[channel].i_calc_gain = m_control[channel].pid_i_gain * m_control[channel].pid_T;
    m_control[channel].d_calc_gain = m_control[channel].pid_d_gain / m_control[channel].pid_T;
}

double MOTOR_get_error (uint8_t channel)
{
    return m_control[channel].error_rpm;
}

void MOTOR_set_direction (uint8_t channel, uint8_t direction)
{
    m_control[channel].direction = direction & 0x01;
}

int32_t MOTOR_get_test_error (uint8_t channel)
{
    return m_control[channel].test_error;
}

uint8_t MOTOR_drive_pid (uint8_t channel)
{
    if (m_control[channel].speed_rpm > 0)
    {           
        m_control[channel].actual_rpm = QEI_get_speed_rpm(channel);  
        // Get velocity
        if (m_control[channel].direction == DIRECTION_BACKWARD)
        {
            
        }
                  
        if (m_control[channel].actual_rpm > m_control[channel].max_rpm)
        {
            m_control[channel].actual_rpm = m_control[channel].max_rpm;
        }
        
        // Error = setpoint - input
        m_control[channel].test_error = (int32_t)m_control[channel].speed_rpm - m_control[channel].actual_rpm;  
                
        //m_control[channel].error_rpm = (double)m_control[channel].speed_rpm - (double)m_control[channel].actual_rpm;
        m_control[channel].error_rpm = (double)m_control[channel].test_error;
        
        // Limit the error to maximum motor error, between -MAX_RPM and MAX_RPM specified in datasheet
//        if (m_control[channel].error_rpm > (double)m_control[channel].max_rpm)
//        {
//            m_control[channel].error_rpm = (double)m_control[channel].max_rpm;
//        }
//        else if (m_control[channel].error_rpm < (double)-m_control[channel].max_rpm)
//        {
//            m_control[channel].error_rpm = (double)-m_control[channel].max_rpm;
//        }
        // Calculate proportional term
        m_control[channel].p_value = m_control[channel].p_calc_gain * m_control[channel].error_rpm;
        
        // Calculate integral term
        m_control[channel].i_term += (m_control[channel].i_calc_gain * m_control[channel].error_rpm);
        // Limit the cumulative error to controls system limit, in this case PWM duty cycle = 100
        if (m_control[channel].i_term > (double)m_control[channel].pid_high_limit)
        {
            m_control[channel].i_term = (double)m_control[channel].pid_high_limit;
        }
        
        // Calculate derivative term
        m_control[channel].d_input = (double)(m_control[channel].actual_rpm - m_control[channel].last_actual_rpm);
        m_control[channel].d_value = m_control[channel].d_calc_gain * m_control[channel].d_input;

        // Compute PID output
        m_control[channel].pid_out = (uint8_t)(m_control[channel].p_value + m_control[channel].i_term - m_control[channel].d_value);   
           
        if (m_control[channel].pid_out > m_control[channel].pid_high_limit)
        {
            m_control[channel].pid_out = m_control[channel].pid_high_limit;
        }
        else if (m_control[channel].pid_out < 0)
        {
            m_control[channel].pid_out = 0;
        }
//            
        // Remember some variables
        m_control[channel].last_actual_rpm = m_control[channel].actual_rpm; 
        m_control[channel].last_error_rpm = m_control[channel].error_rpm;  
        return m_control[channel].pid_out;
    }
    else
    {
        return 0;
    }
}

