#ifndef __PMP_H_
#define	__PMP_H_

dspic_33ck_generic

#define PMP_WRITE           0
#define PMP_READ            1

#define PMP_READ_BUF_LENGTH 64

#define PMP_MODE_TFT        0
#define PMP_MODE_SRAM       1

#define PMP_CS1_TFT         0
#define PMP_CS2_SRAM        1

#define LCD_RESET_LATCH     LATKbits.LATK0
#define LCD_DAT_CMD_LATCH   LATJbits.LATJ0
#define PMP_CS1_STROBE      LATKbits.LATK11
#define PMP_CS2_STROBE      LATKbits.LATK12
#define PMP_PMBE_STROBE     LATKbits.LATK13
#define PMP_WR_STROBE       LATKbits.LATK14
#define PMP_RD_STROBE       LATKbits.LATK15

typedef struct
{
    uint8_t old_cmd;
    uint8_t new_cmd;
    uint16_t address;
    uint8_t state;
    uint8_t chip_select;
    uint16_t read_buffer[PMP_READ_BUF_LENGTH];
}STRUCT_PMP;

void PMP_init (uint8_t mode);
void PMP_write_single (uint8_t mode, uint16_t adr, uint16_t data);
void PMP_write_multiple (uint8_t mode, uint16_t adr, uint16_t *ptr, uint16_t length);
uint16_t PMP_read_single (uint8_t mode, uint16_t adr);
uint16_t * PMP_read_multiple (uint8_t mode, uint16_t adr, uint16_t length);
void PMP_pins_init (void);
void PMP_set_bus_output (void);
void PMP_set_bus_input (void);

#endif	

