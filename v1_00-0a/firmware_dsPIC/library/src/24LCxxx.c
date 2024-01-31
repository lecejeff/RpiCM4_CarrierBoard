#include "24lcxxx.h"

uint8_t EEPROM_discover (STRUCT_EEPROM *mem, uint8_t base_adr, uint8_t end_adr)
{
    uint8_t i = base_adr;
    uint8_t adr_ack = 1;        // nack by default
    uint8_t i2c_buf[1];
    
    if (i <= end_adr)
    {
        while(i <= end_adr)
        {
            i2c_buf[0] = i;
            I2C_master_write(I2C_1, i2c_buf, 1);
            while(I2C_wait(I2C_1)==1);
            adr_ack = I2C_get_ack_state(I2C_1);
            if (adr_ack == 0)     // I2C slave acknowledged?
            {
                mem->address = i;
                mem->discovered = 1;
                return mem->address;
            }
            else
                i = i + 2;
                __delay_ms(1);
        }
        mem->address = 0;
        mem->discovered = 0;
        return 0xFF;
    }
    else
    {
        return 0xFF;
    }    
}

void EEPROM_write_byte (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t byte)
{
    
}

void EEPROM_write_buf (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t *buf, uint8_t length)
{
    
}

void EEPROM_write_page (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t *buf, uint8_t length)
{
    
}

uint8_t EEPROM_get_status (STRUCT_EEPROM *mem)
{
    return 0;
}

uint8_t EEPROM_discovered (STRUCT_EEPROM *mem)
{
    return mem->discovered;
}