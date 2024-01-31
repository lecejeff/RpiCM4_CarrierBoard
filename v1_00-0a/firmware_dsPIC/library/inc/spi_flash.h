#ifndef __spi_flash_h__
#define __spi_flash_h__

dspic_33ck_generic
#include "spi.h"

typedef struct
{
    uint8_t state;
    uint8_t prev_state;
    uint8_t busy;
    uint16_t tx_buf_length;
    uint16_t rx_buf_length;
    uint32_t erase_length;
    uint8_t wp_state;
    uint8_t hold_state;
    
    // Reference to an SPI structure used to communicate with FLASH
    STRUCT_SPI *spi_ref;    
}STRUCT_FLASH;

#define FLASH_QTY               1

#define SPI_FLASH_STATE_INIT    0
#define SPI_FLASH_WRITE_ENABLE  1
#define SPI_FLASH_WRITE_DISABLE 2 
#define SPI_FLASH_HOLD          3
#define SPI_FLASH_WRITE         4
#define SPI_FLASH_READ          5
#define SPI_FLASH_ERASE         6
#define SPI_FLASH_SLEEP         7
#define SPI_FLASH_RESUME        8
#define SPI_FLASH_BUSY          9

#define ADESTO_MEM_32Mb
//#define ISSI_MEM_32Mb

#ifdef ADESTO_MEM_32Mb
// System commands
#define CMD_ENABLE_RESET        0x66
#define CMD_RESET_DEVICE        0x99
#define CMD_DEEP_PD             0xB9
#define CMD_RELEASE_PD          0xAB
// Read commands
#define CMD_NORMAL_READ         0x03
// Write commands
#define CMD_WRITE_ENABLE        0x06
#define CMD_VOL_SR_WRITE_ENABLE 0x50
#define CMD_WRITE_DISABLE       0x04
// Program commands
#define CMD_PAGE_PROGRAM        0x02
// Erase commands
#define CMD_BLOCK_ERASE_4k      0x20
#define CMD_BLOCK_ERASE_32k     0x52
#define CMD_BLOCK_ERASE_64k     0xD8
#define CMD_CHIP_ERASE          0xC7
// Suspend/Resume commands
#define CMG_PRG_ERASE_SUSPEND   0x75
#define CMG_PRG_ERASE_RESUME    0x7A
// Status register commands
#define CMD_READ_STATUS1        0x05
#define CMD_READ_STATUS2        0x35
#define CMD_READ_STATUS3        0x15
#define CMD_WRITE_STATUS1       0x01
#define CMD_WRITE_STATUS2       0x31
#define CMD_WRITE_STATUS3       0x11
#endif

#define FLASH_WP_PIN            LATJbits.LATJ15
#define FLASH_HOLD_PIN          LATAbits.LATA0

#define BLOCK_ERASE_4K          0xFFF
#define BLOCK_ERASE_32K         0x7FFF
#define BLOCK_ERASE_64K         0xFFFF

#define PAGE_PROGRAM            0xFF

void SPI_flash_init (STRUCT_FLASH *flash, STRUCT_SPI *spi, uint16_t tx_buf_length, uint16_t rx_buf_length,
                    uint8_t DMA_tx_channel, uint8_t DMA_rx_channel);
uint8_t SPI_flash_page_write (STRUCT_FLASH *flash, uint32_t adr, uint8_t *ptr);
uint8_t SPI_flash_read_page (STRUCT_FLASH *flash, uint32_t adr);
uint8_t SPI_flash_erase (STRUCT_FLASH *flash, uint8_t type, uint32_t adr);
uint8_t SPI_flash_write_enable(STRUCT_FLASH *flash);
uint8_t SPI_flash_write_disable(STRUCT_FLASH *flash);
uint8_t SPI_flash_busy (STRUCT_FLASH *flash);
uint8_t SPI_flash_busy_polling (STRUCT_FLASH *flash);
uint8_t SPI_flash_get_state (STRUCT_FLASH *flash);
uint8_t * SPI_flash_get_rx_buffer (STRUCT_FLASH *flash);

#endif