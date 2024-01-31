#ifndef __ROT_ENCODER_H__
#define __ROT_ENCODER_H__

#include "dspic_33ck_generic.h"

#define ENC_1 0

#define ENCODER_QTY 1
#define ENCODER_MAX_CPR 24

typedef struct
{
    uint8_t channel;
    uint8_t direction;
    uint16_t counter;
    uint16_t velocity_counter;
    uint16_t count_per_rev;
    uint16_t tour_counter;
    uint16_t velocity_rpm;
    uint8_t switch_state;
    uint8_t switch_debouncer;
    uint16_t maximum_cpr;
    uint16_t position;
    uint8_t A_state;
    uint8_t B_state;
    uint8_t channel_change;
    uint32_t fs_velocity;
}STRUCT_ENCODER;

void ENCODER_init (STRUCT_ENCODER *enc, uint8_t channel, uint32_t fs_velocity);
uint8_t ENCODER_get_direction (STRUCT_ENCODER *enc);
uint16_t ENCODER_get_velocity (STRUCT_ENCODER *enc);
uint16_t ENCODER_get_position (STRUCT_ENCODER *enc);
uint32_t ENCODER_get_fs (STRUCT_ENCODER *enc);
uint8_t ENCODER_get_switch_state (STRUCT_ENCODER *enc);
uint16_t ENCODER_get_velocity (STRUCT_ENCODER *enc);
void ENCODER_interrupt_handle (STRUCT_ENCODER *enc);

#endif