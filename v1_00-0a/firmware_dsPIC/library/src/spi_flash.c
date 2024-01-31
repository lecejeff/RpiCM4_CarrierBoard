#include "spi_flash.h"
#include "uart.h"
#include "timer.h"

STRUCT_FLASH FLASH_struct[FLASH_QTY];

void SPI_flash_init (STRUCT_FLASH *flash, STRUCT_SPI *spi, uint16_t tx_buf_length, uint16_t rx_buf_length,
                    uint8_t DMA_tx_channel, uint8_t DMA_rx_channel)
{
    TRISJbits.TRISJ15 = 0;              // Set flash WP pin to output
    TRISAbits.TRISA0 = 0;               // Set flash HOLD pin to output
    FLASH_WP_PIN = 1;                   // Write protect is inactive
    FLASH_HOLD_PIN = 1;                 // HOLD is inactive    
    
    flash->state = SPI_FLASH_STATE_INIT;  
    flash->prev_state = SPI_FLASH_STATE_INIT; 
    flash->tx_buf_length = tx_buf_length;
    flash->rx_buf_length = rx_buf_length;
    flash->spi_ref = spi;
    flash->spi_ref->chip = FLASH_MEMORY_CS;
    flash->hold_state = FLASH_HOLD_PIN;
    flash->wp_state = FLASH_WP_PIN;
    
    SPI_init(flash->spi_ref, SPI_2, SPI_MODE0, PPRE_1_1, SPRE_7_1, tx_buf_length, rx_buf_length, DMA_tx_channel, DMA_rx_channel);   // Set SPI2 to 10MHz (70MIPS / 7)
}

// Page write = 256 bytes of data + 4 bytes for the flash command
uint8_t SPI_flash_page_write (STRUCT_FLASH *flash, uint32_t adr, uint8_t *ptr)
{
    uint8_t buf[260] = {0};
    uint16_t i = 0;
    
    if (flash->state != SPI_FLASH_WRITE_ENABLE)
    {
        SPI_flash_write_enable(flash);
        return 0;
    }
    else
    {  
        flash->prev_state = flash->state;
        flash->state = SPI_FLASH_WRITE;   
        // This here is redundant. We write the array 2x times in memory, 
        // once to fill the local buffer and once again to fill the DMA buffer.
        buf[0] = CMD_PAGE_PROGRAM;
        buf[1] = ((adr & 0xFF0000)>>16);
        buf[2] = ((adr & 0x00FF00)>>8);
        buf[3] = adr&0x0000FF;
        for (i=4; i < 260; i++)
        {
            buf[i] = *ptr++;
        }
        
        if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 260) == 0)
        {
            return 0;
        }       
        if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
        {
            return 0;
        }
        return 1;
    }
}

// Page read = 256 bytes of data + 4 bytes for the flash command
uint8_t SPI_flash_read_page (STRUCT_FLASH *flash, uint32_t adr)
{
    uint8_t buf[260];

    flash->prev_state = flash->state;
    flash->state = SPI_FLASH_READ;  
    
    buf[0] = CMD_NORMAL_READ;
    buf[1] = ((adr & 0xFF0000)>>16);
    buf[2] = ((adr & 0x00FF00)>>8);
    buf[3] = adr;

    if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 260) == 0)
    {
        return 0;
    } 
    if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
    {
        return 0;
    }
    return 1;
}

// Return 0 if function had to call SPI_flash_write_enable
// Return 1 if function proceeded with flash erase
uint8_t SPI_flash_erase (STRUCT_FLASH *flash, uint8_t type, uint32_t adr)
{   
    // In order to erase a memory location, the WEL bit must be set
    if (flash->state != SPI_FLASH_WRITE_ENABLE)
    {
        SPI_flash_write_enable(flash);
        return 0;                   
    }
    else
    {
        flash->prev_state = flash->state;
        flash->state = SPI_FLASH_ERASE;  

        if (type == CMD_CHIP_ERASE)
        {
            uint8_t buf[1] = {type}; 
            if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 1) == 0)
            {
                // Failed or resource busy, process it here
                return 0;                
            }
            if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
            {
                // Failed or resource busy, process it here
                return 0;
            }
        }
        else
        {
            uint8_t buf[4] = {type, ((adr & 0xFF0000)>>16), ((adr & 0x00FF00)>>8), adr};
            if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 4) == 0)
            {
                // Failed or resource busy, process it here
                return 0;
            }
            if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
            {
                // Failed or resource busy, process it here
                return 0;
            }
        }
        return 1;
    }
}

// Blocking function call
uint8_t SPI_flash_busy_polling (STRUCT_FLASH *flash)
{
    uint8_t buf[2] = {CMD_READ_STATUS1, 0};
    // Blocking SPI call
    SPI_load_tx_buffer(flash->spi_ref, buf, 2);
    SPI_write(flash->spi_ref, FLASH_MEMORY_CS); 
    while(SPI_get_txfer_state(flash->spi_ref)!= SPI_TX_COMPLETE);
    return SPI_get_rx_buffer_index(flash->spi_ref, 1);
}

// Function reads the FLASH busy flag. If flag == 0, device is ready for operation
// If flag == 1, device is busy with internal operation
uint8_t SPI_flash_busy (STRUCT_FLASH *flash)
{
    uint8_t buf[2] = {CMD_READ_STATUS1, 0};
    if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 2) == 0)
    {
        // Failed or resource busy, process it here
        return 0;
    }
    if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
    {
        // Failed or resource busy, process it here
        return 0;
    }
    return 1;
}

uint8_t SPI_flash_write_enable(STRUCT_FLASH *flash)
{
    uint8_t buf[1] = {CMD_WRITE_ENABLE};
    FLASH_WP_PIN = 1;   
    flash->prev_state = flash->state;
    flash->state = SPI_FLASH_WRITE_ENABLE;   
    if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 1) == 0)
    {
        // Failed or resource busy, process it here
        return 0;
    }
    if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
    {
        // Failed or resource busy, process it here
        return 0;
    }
    return 1;
}

uint8_t SPI_flash_write_disable(STRUCT_FLASH *flash)
{
    uint8_t buf[1] = {CMD_WRITE_DISABLE};
    flash->prev_state = flash->state;
    flash->state = SPI_FLASH_WRITE_DISABLE;   
    if (SPI_load_dma_tx_buffer(flash->spi_ref, buf, 1) == 0)
    {
        // Failed or resource busy, process it here
        return 0;
    }
    
    if (SPI_write_dma(flash->spi_ref, FLASH_MEMORY_CS) == 0)
    {
        // Failed or resource busy, process it here
        return 0;
    }   
    FLASH_WP_PIN = 0;
    return 1;
}

uint8_t SPI_flash_get_state (STRUCT_FLASH *flash)
{
    return flash->state;
}

uint8_t * SPI_flash_get_rx_buffer (STRUCT_FLASH *flash)
{
#ifdef SPI2_DMA_ENABLE
    if (SPI_unload_dma_rx_buffer(flash->spi_ref) == 0) 
    {
        return 0;
        // Resource busy or error, process it here
    }
    else
    {
        return SPI_get_rx_buffer(flash->spi_ref);
    }
#endif
}