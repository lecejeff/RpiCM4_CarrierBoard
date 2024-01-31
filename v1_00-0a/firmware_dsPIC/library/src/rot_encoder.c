#include "rot_encoder.h"

STRUCT_ENCODER ENCODER_struct[ENCODER_QTY];

void ENCODER_init (STRUCT_ENCODER *enc, uint8_t channel, uint32_t fs_velocity)
{
    
    switch (channel)
    {
        case ENC_1:
            TRISAbits.TRISA14 = 1;  // Set rotary encoder A pulse pin to input
            TRISAbits.TRISA15 = 1;  // Set rotary encoder B pulse pin to input
            TRISAbits.TRISA5 = 1;   // Set rotary encoder SW pin to input
            CNENAbits.CNIEA15 = 1;  // Enable change notice interrupt on encoder B pulse pin  

            enc->channel = channel;
            enc->counter = 0;
            enc->direction = 0;   
            enc->position = 0;
            enc->velocity_rpm = 0;
            enc->velocity_counter = 0;
            enc->fs_velocity = fs_velocity;
            enc->switch_state = 0;
            enc->switch_debouncer = 1;
            enc->count_per_rev = 0;
            enc->channel_change = 0;
            enc->A_state = PORTAbits.RA14;
            enc->B_state = PORTAbits.RA15;
            enc->maximum_cpr = ENCODER_MAX_CPR;

            IEC1bits.CNIE = 1;      // Enable change notice interrupt
            IPC4bits.CNIP = 1;   
            break;
            
        default:
            break;
    }
}

uint8_t ENCODER_get_direction (STRUCT_ENCODER *enc)
{
    return enc->direction;
}

uint16_t ENCODER_get_position (STRUCT_ENCODER *enc)
{
    return enc->count_per_rev;
}

uint32_t ENCODER_get_fs (STRUCT_ENCODER *enc)
{
    return enc->fs_velocity;
}

uint16_t ENCODER_get_velocity (STRUCT_ENCODER *enc)
{
    if (enc->velocity_counter > 0)
    {
        enc->velocity_rpm = (uint16_t)((enc->velocity_counter * enc->fs_velocity)/(enc->maximum_cpr));
        enc->velocity_counter = 0;
        return enc->velocity_rpm;
    }
    else
        return 0;    
}

uint8_t ENCODER_get_switch_state (STRUCT_ENCODER *enc)
{
    // Negative edge interrupt fired and SW state is still 0
    if (PORTAbits.RA5 == 0)
    {
        if (++enc->switch_debouncer > 5)
        {
            enc->switch_debouncer = 5;
            return 0;
        }
        else
            return 1;
    }
    else
    {
        if (--enc->switch_debouncer < 1)
        {
            enc->switch_state = 1;
            enc->switch_debouncer = 1;
        }
        return 1;
    }      
}

void ENCODER_interrupt_handle (STRUCT_ENCODER *enc)
{
    // Backward
    if (enc->channel_change == 1)
    {
        if (PORTAbits.RA14 == 1)
        {
            enc->direction = 1;
        }   

        // Forward
        else 
        {
            enc->direction = 0;
        }  
    }
        
    enc->counter++;
    enc->velocity_counter++;
    enc->count_per_rev++;
    
    if (enc->count_per_rev >= enc->maximum_cpr)
    {
        enc->count_per_rev = 0;
        enc->tour_counter++;
    }         
}

// Encoder 1 interrupt handle
void __attribute__((__interrupt__, no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;   
    if ((PORTAbits.RA15 == 1) && (ENCODER_struct[ENC_1].B_state == 0))
    {
        ENCODER_struct[ENC_1].B_state = 1;
        ENCODER_struct[ENC_1].channel_change = 1;
        ENCODER_interrupt_handle(&ENCODER_struct[ENC_1]);   // Handle the interrupt 
    }
    else
        ENCODER_struct[ENC_1].B_state = PORTAbits.RA15;
}
