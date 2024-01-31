#ifndef __ividac_driver_h__
#define __ividac_driver_h__

dspic_33ck_generic
#include "spi.h"

//------------------------ IVIDAC resolution selection -----------------------//
#define IVIDAC_12BIT_AD5621
#define IVIDAC_14BIT_AD5641
//----------------------------------------------------------------------------//

#ifdef IVIDAC_12BIT_AD5621
#define IVIDAC_RESOLUTION_12BIT 12
#define AD5621_OUTPUT_NORMAL    0
#define AD5621_OUTPUT_1KOHM     1
#define AD5621_OUTPUT_100KOHM   2
#define AD5621_OUTPUT_HIGHZ     3
#endif


#ifdef IVIDAC_14BIT_AD5641
#define IVIDAC_RESOLUTION_14BIT 14
#define AD5641_OUTPUT_NORMAL    0
#define AD5641_OUTPUT_1KOHM     1
#define AD5641_OUTPUT_100KOHM   2
#define AD5641_OUTPUT_HIGHZ     3
#endif

#define IVIDAC_OUTPUT_ENABLE    1
#define IVIDAC_OUTPUT_DISABLE   0

#define IVIDAC_ON_MIKROBUS1     0
#define IVIDAC_OE_MIKROBUS1_PIN LATHbits.LATH14  

#define IVIDAC_ON_MIKROBUS2     1
#define IVIDAC_OE_MIKROBUS2_PIN LATHbits.LATH12  

#define IVIDAC_ABSOLUTE_MINIMUM_CODE    0
#define IVIDAC_ABSOLUTE_MAXIMUM_CODE    0x3FFF
#define IVIDAC_REDUCED_CODE_LOW         256
#define IVIDAC_REDUCED_CODE_HIGH        16128

typedef struct
{
    int16_t voltage_output_offset;
    int16_t current_output_offset;
    
    uint16_t gain_offset_error;
    
    uint8_t resolution;
    uint8_t output_type;
    uint16_t spi_output;
    uint16_t dac_output;
    uint8_t output_enable;
}STRUCT_IVIDAC;

void IVIDAC_init (STRUCT_IVIDAC *ividac, uint8_t port, uint8_t resolution, uint8_t output_type, uint8_t output_enable);
void IVIDAC_set_output_raw (STRUCT_IVIDAC *ividac, uint8_t port, uint8_t output_type, uint16_t output);   // 
void IVIDAC_set_output_state (STRUCT_IVIDAC *ividac, uint8_t port, uint8_t value);
uint8_t IVIDAC_get_output_state (STRUCT_IVIDAC *ividac);
uint16_t IVIDAC_get_output_value (STRUCT_IVIDAC *ividac);

#endif