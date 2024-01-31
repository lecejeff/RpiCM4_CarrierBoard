#ifndef __24LCxxx_h__
#define __24LCxxx_h__

#include "i2c.h"
dspic_33ck_generic

#define EEPROM_24LC512

#ifdef EEPROM_24LC512
    #define PAGE_WRITE_LENGTH   128                         // Length of page write in bytes
    #define EEPROM_BIT_LENGTH   524288                      // Bit length
    #define EEPROM_BYTE_LENGTH (EEPROM_BIT_LENGTH / 8)      // Byte length
    #define EEPROM_BASE_ADDRESS 0                           // Start address
    #define EEPROM_END_ADDRESS (EEPROM_BYTE_LENGTH - 1)     // End address
#endif

typedef struct
{
    uint8_t wr_buf[PAGE_WRITE_LENGTH];
    uint8_t rd_buf[PAGE_WRITE_LENGTH];
    uint8_t address;
    uint8_t discovered;
    uint8_t busy_flag;
}STRUCT_EEPROM;

uint8_t EEPROM_init (STRUCT_EEPROM *mem, uint8_t i2c_address);
void EEPROM_write_byte (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t byte);
void EEPROM_write_buf (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t *buf, uint8_t length);
void EEPROM_write_page (STRUCT_EEPROM *mem, uint8_t base_address, uint8_t *buf, uint8_t length);
uint8_t EEPROM_get_status (STRUCT_EEPROM *mem);
uint8_t EEPROM_discovered (STRUCT_EEPROM *mem);
uint8_t EEPROM_discover (STRUCT_EEPROM *mem, uint8_t base_adr, uint8_t end_adr);

#endif