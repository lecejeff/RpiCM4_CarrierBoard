#include "ividac_driver.h"

STRUCT_SPI *IVIDAC_spi;

void IVIDAC_init (STRUCT_IVIDAC *ividac, uint8_t port, uint8_t resolution, uint8_t output_type, uint8_t output_enable)
{  
    ividac->resolution = resolution;
    ividac->output_type = output_type;
    ividac->output_enable = output_enable;  
    
    if (port == IVIDAC_ON_MIKROBUS1)
    {
        TRISHbits.TRISH14 = 0;
        IVIDAC_OE_MIKROBUS1_PIN = IVIDAC_OUTPUT_DISABLE;
        if (ividac->output_enable == IVIDAC_OUTPUT_DISABLE)
        {
            IVIDAC_OE_MIKROBUS1_PIN = IVIDAC_OUTPUT_DISABLE;
        }
        else
        {
            IVIDAC_OE_MIKROBUS1_PIN = IVIDAC_OUTPUT_ENABLE;
        }
    }
    
    if (port == IVIDAC_ON_MIKROBUS2)
    {
        TRISHbits.TRISH12 = 0;
        IVIDAC_OE_MIKROBUS2_PIN = IVIDAC_OUTPUT_DISABLE;
        if (ividac->output_enable == IVIDAC_OUTPUT_DISABLE)
        {
            IVIDAC_OE_MIKROBUS2_PIN = IVIDAC_OUTPUT_DISABLE;
        }
        else
        {
            IVIDAC_OE_MIKROBUS2_PIN = IVIDAC_OUTPUT_ENABLE;
        }        
    }
    
}

void IVIDAC_set_output_raw (STRUCT_IVIDAC *ividac, uint8_t port, uint8_t output_type, uint16_t output) 
{
    uint16_t spi_word = 0;
    uint8_t dac_write_array[2] = {0};
    
    if (ividac->resolution == IVIDAC_RESOLUTION_12BIT)
    {  
        if (output > 0xFFF)
        {
            output = 0xFFF;           
        }
        spi_word = (uint16_t)(((output_type << 14) | (output << 2)) & 0xFFFC);
        ividac->dac_output = output; 
        ividac->spi_output = spi_word;      
    }

    if (ividac->resolution == IVIDAC_RESOLUTION_14BIT)
    {  
        if (output > 0x3FFF)
        {
            output = 0x3FFF;
        }
        spi_word = (uint16_t)((output_type << 14) | output);
        ividac->dac_output = output; 
        ividac->spi_output = spi_word;
    }    
    dac_write_array[0] = ((ividac->spi_output & 0xFF00)>>8);
    dac_write_array[1] = ividac->spi_output & 0x00FF;
    
    if (port == IVIDAC_ON_MIKROBUS1)
    {
        SPI_load_tx_buffer(IVIDAC_spi, dac_write_array, 2);
        SPI_write(IVIDAC_spi, MIKROBUS1_CS); 
    }
    if (port == IVIDAC_ON_MIKROBUS2)
    {
        SPI_load_tx_buffer(IVIDAC_spi, dac_write_array, 2);
        SPI_write(IVIDAC_spi, MIKROBUS2_CS); 
    }
}

void IVIDAC_set_output_state (STRUCT_IVIDAC *ividac, uint8_t port, uint8_t value)
{
    if (port == IVIDAC_ON_MIKROBUS1)
    {
        if (value == IVIDAC_OUTPUT_ENABLE)
        {
            IVIDAC_OE_MIKROBUS1_PIN = IVIDAC_OUTPUT_ENABLE;
            ividac->output_enable = IVIDAC_OUTPUT_ENABLE;
        }
        
        if (value == IVIDAC_OUTPUT_DISABLE)
        {
            IVIDAC_OE_MIKROBUS1_PIN = IVIDAC_OUTPUT_DISABLE;
            ividac->output_enable = IVIDAC_OUTPUT_DISABLE;            
        }
    }
    
    if (port == IVIDAC_ON_MIKROBUS2)
    {
        if (value == IVIDAC_OUTPUT_ENABLE)
        {
            IVIDAC_OE_MIKROBUS2_PIN = IVIDAC_OUTPUT_ENABLE;
            ividac->output_enable = IVIDAC_OUTPUT_ENABLE;
        }
        
        if (value == IVIDAC_OUTPUT_DISABLE)
        {
            IVIDAC_OE_MIKROBUS2_PIN = IVIDAC_OUTPUT_DISABLE;
            ividac->output_enable = IVIDAC_OUTPUT_DISABLE;            
        }
    }
}

uint8_t IVIDAC_get_output_state (STRUCT_IVIDAC *ividac)
{
    return ividac->output_enable;
}

uint16_t IVIDAC_get_output_value (STRUCT_IVIDAC *ividac)
{
    return ividac->dac_output;
}