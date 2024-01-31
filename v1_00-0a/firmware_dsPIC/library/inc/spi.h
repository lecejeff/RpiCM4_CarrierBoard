//***************************************************************************//
// File      : spi.h
//
// Functions :  void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre); 
//              void SPI_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip);
//              uint8_t SPI_txfer_done (uint8_t channel);
//              uint8_t * SPI_get_rx_buffer (uint8_t channel);
//              uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index);
//              void SPI_deassert_cs (uint8_t chip);
//              void SPI_assert_cs (uint8_t chip);
//              void SPI_flush_txbuffer (uint8_t channel);
//              void SPI_flush_rxbuffer (uint8_t channel);
//              uint8_t SPI_module_busy (uint8_t channel);
//
// Includes  :  dspeak_generic.h
//           
// Purpose   :  Driver for the dsPIC33E SPI peripheral
//              4x seperate SPI channels on dsPeak :
//              SPI_1 : Riverdi EVE embedded video engine
//              SPI_2 : Flash / uSD Card 
//              SPI_3 : Audio CODEC
//              SPI_4 : MikroBus 1 and 2
//
// Intellitrol                   MPLab X v5.45                        13/01/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#ifndef __spi_h_
#define __spi_h_

dspic_33ck_generic
#include "dma.h"

//******************************************************************************
// Definition of SPI modules
#define SPI_1 0
#define SPI_2 1
#define SPI_3 2
#define SPI_4 3
#define SPI_QTY 4

// SPI clock dividers define
#define SPRE_1_1            7
#define SPRE_2_1            6
#define SPRE_3_1            5
#define SPRE_4_1            4
#define SPRE_5_1            3
#define SPRE_6_1            2
#define SPRE_7_1            1
#define SPRE_8_1            0

#define PPRE_1_1            3
#define PPRE_4_1            2
#define PPRE_16_1           1
#define PPRE_64_1           0

//******************************************************************************
// Various SPI chip select defines
#define FT8XX_EVE_CS        1
#define SD_CARD_CS          2
#define FLASH_MEMORY_CS     3
#define AUDIO_CODEC_CS      4
#define MIKROBUS1_CS        5
#define MIKROBUS2_CS        6

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define SPI_BUF_LENGTH 260

#ifdef EVE_SCREEN_ENABLE
    #define EVE_SPI_BUF_LENGTH  SPI_BUF_LENGTH
#endif

#define SPI_TX_IDLE         0
#define SPI_TX_COMPLETE     1

// SPI with DMA state machine
#define SPI_IS_INITIALIZED  0
#define SPI_IS_READY        1
#define SPI_TX_LOADED       2
#define SPI_TX_IN_PROGRESS  3
#define SPI_TX_IS_DONE      4  

//#define SPI1_DMA_ENABLE
#define SPI2_DMA_ENABLE
//#define SPI3_DMA_ENABLE
//#define SPI4_DMA_ENABLE

#define SPI_MODULE_FREE     0
#define SPI_MODULE_BUSY     1
//******************************************************************************
// SPI CS pin assignation for assert / deassert functions
#define FT8XX_EVE_CS_PIN    LATBbits.LATB11
#define SD_CARD_CS_PIN      LATGbits.LATG9
#define FLASH_MEMORY_CS_PIN LATGbits.LATG0
#define AUDIO_CODEC_CS_PIN  LATDbits.LATD6
#define MIKROBUS1_CS_PIN    LATHbits.LATH15
#define MIKROBUS2_CS_PIN    LATHbits.LATH13

typedef struct
{
    uint8_t SPI_channel;
    uint8_t DMA_tx_channel;
    uint8_t DMA_rx_channel;
    uint8_t mode;
    uint8_t ppre;
    uint8_t spre;
    uint8_t chip;
    uint8_t tx_data[SPI_BUF_LENGTH];
    uint8_t rx_data[SPI_BUF_LENGTH];
    uint16_t tx_buf_length;
    uint16_t rx_buf_length;
    uint16_t tx_length;
    uint16_t last_tx_length;
    uint16_t tx_remaining;
    uint8_t txfer_state;
    uint16_t rx_cnt;
    uint16_t tx_cnt;
}STRUCT_SPI;

void SPI_init (STRUCT_SPI *spi, uint8_t spi_channel, uint8_t spi_mode, uint8_t ppre, 
                uint8_t spre, uint16_t tx_buf_length, uint16_t rx_buf_length,
                uint8_t DMA_tx_channel, uint8_t DMA_rx_channel); 
uint8_t SPI_write (STRUCT_SPI *spi, uint8_t chip);
uint8_t SPI_write_dma (STRUCT_SPI *spi, uint8_t chip);
uint8_t SPI_release_port (STRUCT_SPI *spi);
uint8_t SPI_set_interrupt_enable (STRUCT_SPI *spi);
uint8_t SPI_get_txfer_state (STRUCT_SPI *spi);
void SPI_set_txfer_state (STRUCT_SPI *spi, uint8_t state);
uint8_t * SPI_get_rx_buffer (STRUCT_SPI *spi);
uint8_t SPI_get_dma_rx_buffer_at_index (STRUCT_SPI *spi, uint16_t index);
uint8_t SPI_get_rx_buffer_index (STRUCT_SPI *spi, uint16_t index);
uint8_t SPI_load_tx_buffer (STRUCT_SPI *spi, uint8_t *data, uint16_t length);
uint8_t SPI_load_dma_tx_buffer (STRUCT_SPI *spi, uint8_t *data, uint16_t length);
uint8_t * SPI_unload_dma_rx_buffer (STRUCT_SPI *spi);
uint8_t SPI_deassert_cs (STRUCT_SPI *spi);
uint8_t SPI_assert_cs (STRUCT_SPI *spi);
void SPI_flush_txbuffer (STRUCT_SPI *spi);
void SPI_flush_rxbuffer (STRUCT_SPI *spi);
uint8_t SPI_module_busy (STRUCT_SPI *spi);
#endif

