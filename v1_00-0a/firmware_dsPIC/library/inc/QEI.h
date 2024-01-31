//****************************************************************************//
// File      :  QEI.c
//
// Functions :  void QEI_init (uint8_t channel);
//              uint32_t QEI_get_pulse (uint8_t channel);
//              void QEI_reset_pulse (uint8_t channel);
//              void QEI_set_gear_derate (uint8_t channel, uint16_t new_gear_derate);
//              uint16_t QEI_get_gear_derate (uint8_t channel);
//              void QEI_set_cpr (uint8_t channel, uint16_t new_cpr);
//              uint16_t QEI_get_cpr (uint8_t channel);
//              uint32_t QEI_get_tour (uint8_t channel);
//              void QEI_reset_tour (uint8_t channel);
//              uint8_t QEI_get_event (uint8_t channel);
//              uint8_t QEI_get_direction (uint8_t channel);
//              uint16_t QEI_get_velocity (uint8_t channel);
//              void QEI_calculate_velocity (uint8_t channel);
//              void QEI_interrupt_handle (uint8_t channel);
//
// Includes  :  dspeak_generic.h
//
// Purpose   :  QEI driver for the dsPIC33EP
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
#ifndef __QEI_H__
#define __QEI_H__

dspic_33ck_generic

typedef struct
{
    uint16_t refresh_freq;
    uint32_t pulse_for_tour;
    uint32_t pulse_cnter_dist;
    uint32_t pulse_cnter;
    uint32_t pulse_getter;
    uint32_t tour_cnter_dist;
    uint32_t tour_cnter;
    uint32_t tour_getter;
    uint8_t int_event;
    uint8_t direction;
    uint16_t motor_cpr;
    uint16_t motor_gear_derate;
    uint16_t motor_max_rpm;
    uint16_t speed_rpm;
    uint16_t speed_rps;
    uint32_t prev_pulse;
    uint32_t velocity;
    uint32_t pulse_diff;
    uint32_t pulse_per_tour;
}STRUCT_QEI;

#define QEI_1       0
#define QEI_2       1
#define QEI_ROT_ENC 2

#define QEI_QTY 3

#define QEI1B_PIN   PORTAbits.RA7
#define QEI1A_PIN   PORTAbits.RA6

#define QEI2B_PIN   PORTGbits.RG13
#define QEI2A_PIN   PORTGbits.RG12

#define ROT_ENCB_PIN PORTAbits.RA15
#define ROT_ENCA_PIN PORTAbits.RA14

#define QEI_DIR_BACKWARD 1
#define QEI_DIR_FORWARD 0

// Defines for 99:1 gearbox, 100RPM 12V motor
// Default values
#define QEI_MOT1_CPR 48                             // Encoder count per revolution
#define QEI_MOT1_GDR 99                             // Encoder gear derate
#define QEI_MOT1_MAX_RPM 130                        // Max RPM   
#define QEI_MOT1_PPT (QEI_MOT1_CPR * QEI_MOT1_GDR)  // Max pulse per rotation

#define QEI_MOT2_CPR 48                             // Encoder count per revolution
#define QEI_MOT2_GDR 99                             // Encoder gear derate
#define QEI_MOT2_MAX_RPM 130                        // Max RPM   
#define QEI_MOT2_PPT (QEI_MOT2_CPR * QEI_MOT2_GDR)  // Max pulse per rotation

#define QEI_ROT_ENC_CPR 48                             // Encoder count per revolution
#define QEI_ROT_ENC_GDR 47                             // Encoder gear derate
#define QEI_ROT_ENC_MAX_RPM 110                        // Max RPM   
#define QEI_ROT_ENC_PPR (QEI_MOT1_CPR * QEI_MOT1_GDR)  // Max pulse per rotation

#define MAX_PULSE_CNT 100000000
#define MAX_TOUR_CNT 100000000

void QEI_init (uint8_t channel);
void QEI_set_fs (uint8_t channel, uint16_t refresh_freq);
uint16_t QEI_get_fs (uint8_t channel);
uint32_t QEI_get_pulse (uint8_t channel);
void QEI_reset_pulse (uint8_t channel);
void QEI_reset_distance (uint8_t channel);
uint32_t QEI_get_distance (uint8_t channel);
void QEI_set_gear_derate (uint8_t channel, uint16_t new_gear_derate);
uint16_t QEI_get_gear_derate (uint8_t channel);
void QEI_set_cpr (uint8_t channel, uint16_t new_cpr);
uint16_t QEI_get_cpr (uint8_t channel);
uint32_t QEI_get_tour (uint8_t channel);
void QEI_reset_tour (uint8_t channel);
uint8_t QEI_get_event (uint8_t channel);
uint8_t QEI_get_direction (uint8_t channel);
uint16_t QEI_get_speed_rpm (uint8_t channel);
uint16_t QEI_get_speed_rps (uint8_t channel);
void QEI_calculate_velocity (uint8_t channel);
uint16_t QEI_get_velocity (uint8_t channel);
uint16_t QEI_get_max_rpm (uint8_t channel);
void QEI_set_max_rpm (uint8_t channel, uint16_t max_rpm);
void QEI_interrupt_handle (uint8_t channel);

#endif