//***************************************************************************//
// File      : spi.c
//
// Functions : void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre); 
//              void SPI_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip);
//              uint8_t SPI_txfer_state (uint8_t channel);
//              uint8_t * SPI_get_rx_buffer (uint8_t channel);
//              uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index);
//              void SPI_deassert_cs (uint8_t chip);
//              void SPI_assert_cs (uint8_t chip);
//              void SPI_flush_txbuffer (uint8_t channel);
//              void SPI_flush_rxbuffer (uint8_t channel);
//              uint8_t SPI_module_busy (uint8_t channel); 
//
// Includes  :  spi.h
//           
// Purpose   :  Driver for the dsPIC33EP SPI peripheral
//              4x seperate SPI channels on dsPeak :
//              SPI_1 : Riverdi EVE embedded video engine
//              SPI_2 : Flash / uSD Card 
//              SPI_3 : Audio CODEC
//              SPI_4 : MikroBus 1 and 2
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#include "spi.h"
STRUCT_SPI SPI_struct[SPI_QTY];

#ifdef SPI1_DMA_ENABLE
    __eds__ uint8_t spi1_dma_tx_buf[EVE_SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
    __eds__ uint8_t spi1_dma_rx_buf[EVE_SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
#endif

#ifdef SPI2_DMA_ENABLE
    __eds__ uint8_t spi2_dma_tx_buf[SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
    __eds__ uint8_t spi2_dma_rx_buf[SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
#endif
    
#ifdef SPI3_DMA_ENABLE
    __eds__ uint8_t spi3_dma_tx_buf[SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
    __eds__ uint8_t spi3_dma_rx_buf[SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
#endif

#ifdef SPI4_DMA_ENABLE
    __eds__ uint8_t spi4_dma_tx_buf[SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
    __eds__ uint8_t spi4_dma_rx_buf[SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
#endif

//void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre)//
//Description : Function initializes SPI module on selected MODE with Prim and
//              secondary prescalers to scale down the clock.
//
//Function prototype :  void SPI_init (uint8_t channel, uint8_t mode, uint8_t ppre, uint8_t spre) 
//
//Enter params       :  uint8_t channel : SPI channel
//                      uint8_t mode : SPI MODE (0...3)
//                      uint8_t ppre : Primary clock prescaler
//                      uint8_t spre : Secondary clock prescaler
//                      
//
//Return params      : None
//
//Function call      : SPI_init(SPI_2, SPI_MODE0, 3, 0);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void SPI_init (STRUCT_SPI *spi, uint8_t spi_channel, uint8_t spi_mode, uint8_t ppre, 
                uint8_t spre, uint16_t tx_buf_length, uint16_t rx_buf_length,
                uint8_t DMA_tx_channel, uint8_t DMA_rx_channel)
{    
    switch (spi_channel)
    {
        // SPI1 is FTDI EVE interface
        case SPI_1:            
            IEC0bits.SPI1IE = 0;            // Disable SPI module interrupt
            SPI1STATbits.SPIEN = 0;         // Disable SPI module 
            IFS0bits.SPI1EIF = 0;           // Reset SPI error int flag 
            IFS0bits.SPI1IF = 0;            // Reset SPI int flag 
            SPI1CON1bits.DISSCK = 0;        // Internal serial clock is enabled 
            SPI1CON1bits.DISSDO = 0;        // SDOx pin is controlled by the module 
            SPI1CON1bits.MODE16 = 0;        // Communication is byte wide (8bit)   
            SPI1CON1bits.MSTEN = 1;         // SPI master mode enabled 
            SPI1CON2bits.SPIBEN = 1;        // Enhanced buffer mode enabled
            SPI1STATbits.SISEL = 5;         // Interrupt when the last bit is shifted out of SPIxSR and the transmit is complete             
            SPI1CON2bits.FRMEN = 0;         // Frame mode disabled 
            SPI1CON1bits.SSEN = 0;          // GPIO controls SPI /CS
            
            // SPI1 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module 
            // (see MCU datasheet, electrical caracteristics section)
            SPI1CON1bits.PPRE = ppre;
            SPI1CON1bits.SPRE = spre;
            switch (spi_mode)
            {
                case SPI_MODE0:
                    SPI1CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI1CON1bits.CKP = 0;   // 
                    SPI1CON1bits.SMP = 0;   // Sample data at beginning valid point                     
                    break;

                case SPI_MODE1:
                    SPI1CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI1CON1bits.CKP = 0;   //  
                    SPI1CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI1CON1bits.CKE = 1;   // Mode 2 configurations  
                    SPI1CON1bits.CKP = 1;   //  
                    SPI1CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI1CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI1CON1bits.CKP = 1;   //  
                    SPI1CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }
            
#ifdef SPI1_DMA_ENABLE
            spi->DMA_tx_channel = DMA_tx_channel;           
            DMA_init(spi->DMA_tx_channel);
            DMA_set_control_register(spi->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_tx_channel, DMAREQ_SPI1);
            DMA_set_peripheral_address(spi->DMA_tx_channel, (volatile uint16_t)&SPI1BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_tx_channel, __builtin_dmapage(spi1_dma_tx_buf), __builtin_dmaoffset(spi1_dma_tx_buf)); 

            spi->DMA_rx_channel = DMA_rx_channel;           
            DMA_init(spi->DMA_rx_channel);
            DMA_set_control_register(spi->DMA_rx_channel, (DMA_SIZE_BYTE | DMA_TXFER_RD_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_rx_channel, DMAREQ_SPI1);
            DMA_set_peripheral_address(spi->DMA_rx_channel, (volatile uint16_t)&SPI1BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_rx_channel, __builtin_dmapage(spi1_dma_rx_buf), __builtin_dmaoffset(spi1_dma_rx_buf));    
#endif

            // SPI1 input/output pin mapping  
            TRISFbits.TRISF13 = 0;          // RF13 configured as an output (SPI1_MOSI) 
            TRISAbits.TRISA1 = 1;           // RA1 configured as an input (SPI1_MISO) 
            TRISFbits.TRISF12 = 0;          // RF12 configured as an output (SPI1_SCLK) 
            TRISBbits.TRISB11 = 0;          // RB11 configured as an output (SPI1_nCS)
            FT8XX_EVE_CS_PIN = 1;           // Set #CS to high
            
            // SPI1 peripheral pin assignment       
            RPOR12bits.RP109R = 0x05;       // RF13 (RP109) assigned to SPI1_MOSI 
            RPINR20bits.SDI1R = 17;         // RA1 (RPI17) assigned to SPI1_MISO
            RPINR20bits.SCK1R = 108;        // RF12 (RP108) assigned to SPI1_SCLK input
            RPOR11bits.RP108R = 0x06;       // RF12 (RP108) assigned to SPI1_SCLK output         
            
            IPC2bits.SPI1IP = 7;            // SPI int priority is 7 (highest) 
            IEC0bits.SPI1EIE = 1;           // Enable SPI error interrupt detection
            IFS0bits.SPI1IF = 0;            // Clear SPI int flag   
            SPI1STATbits.SPIEN = 1;         // Enable SPI module               
            break;
            
        // SPI2 is SD / FLASH interface  
        case SPI_2:
            IEC2bits.SPI2IE = 0;            // Disable SPI module interrupt
            SPI2STATbits.SPIEN = 0;         // Disable SPI module 
            IFS2bits.SPI2EIF = 0;           // Reset SPI error int flag 
            IFS2bits.SPI2IF = 0;            // Reset SPI int flag 
            SPI2CON1bits.DISSCK = 0;        // Internal serial clock is enabled 
            SPI2CON1bits.DISSDO = 0;        // SDOx pin is controlled by the module 
            SPI2CON1bits.MODE16 = 0;        // Communication is byte wide (8bit)   
            SPI2CON1bits.MSTEN = 1;         // SPI master mode enabled             
            
            //SPI2CON2bits.SPIBEN = 1;        // Enhanced buffer mode enable
            //SPI2STATbits.SISEL = 5;         // Interrupt when the last bit is shifted out of SPIxSR and the transmit is complete              
            SPI2CON2bits.FRMEN = 0;         // Frame mode disabled 
            SPI2CON1bits.SSEN = 0;          // GPIO controls SPI /CS      
            // SPI2 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI2CON1bits.PPRE = ppre;
            SPI2CON1bits.SPRE = spre;
            switch (spi_mode)
            {
                case SPI_MODE0:
                    SPI2CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI2CON1bits.CKP = 0;   //
                    SPI2CON1bits.SMP = 0;   // Sample data at beginning valid point                    
                    break;

                case SPI_MODE1:
                    SPI2CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI2CON1bits.CKP = 0;   //  
                    SPI2CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI2CON1bits.CKE = 1;   // Mode 2 configurations
                    SPI2CON1bits.CKP = 1;   // 
                    SPI2CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI2CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI2CON1bits.CKP = 1; 
                    SPI2CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI2 input/output pin mapping 
            TRISGbits.TRISG6 = 0;           // RG6 configured as an output (SPI2_SCLK)
            TRISGbits.TRISG7 = 1;           // RG7 configured as an input (SPI2_MISO)
            TRISGbits.TRISG8 = 0;           // RGB configured as an output (SPI2_MOSI)
            TRISGbits.TRISG9 = 0;           // RB9 configured as an output (SD_nCS)
            TRISGbits.TRISG0 = 0;           // RG0 configured as an output (FLASH_nCS)
            SD_CARD_CS_PIN = 1;
            FLASH_MEMORY_CS_PIN = 1;

            IPC8bits.SPI2IP = 4;            // SPI int priority is 7  
            IEC2bits.SPI2EIE = 1;           // Enable SPI error interrupt detection
            IFS2bits.SPI2IF = 0;            // Clear SPI int flag                           

#ifdef SPI2_DMA_ENABLE
            spi->DMA_tx_channel = DMA_tx_channel;           
            DMA_init(spi->DMA_tx_channel);
            DMA_set_control_register(spi->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_tx_channel, DMAREQ_SPI2);
            DMA_set_peripheral_address(spi->DMA_tx_channel, (volatile uint16_t)&SPI2BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_tx_channel, __builtin_dmapage(spi2_dma_tx_buf), __builtin_dmaoffset(spi2_dma_tx_buf)); 

            spi->DMA_rx_channel = DMA_rx_channel;           
            DMA_init(spi->DMA_rx_channel);
            DMA_set_control_register(spi->DMA_rx_channel, (DMA_SIZE_BYTE | DMA_TXFER_RD_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_rx_channel, DMAREQ_SPI2);
            DMA_set_peripheral_address(spi->DMA_rx_channel, (volatile uint16_t)&SPI2BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_rx_channel, __builtin_dmapage(spi2_dma_rx_buf), __builtin_dmaoffset(spi2_dma_rx_buf));          
#endif
            SPI2STATbits.SPIEN = 1;         // Enable SPI module   
            break;  
            
        // SPI3 is the AUDIO CODEC configuration interface    
        case SPI_3:
            IEC5bits.SPI3IE = 0;            // Disable SPI module interrupt
            SPI3STATbits.SPIEN = 0;         // Disable SPI module 
            IFS5bits.SPI3EIF = 0;           // Reset SPI error int flag 
            IFS5bits.SPI3IF = 0;            // Reset SPI int flag 
            SPI3CON1bits.DISSCK = 0;        // Internal serial clock is enabled 
            SPI3CON1bits.DISSDO = 0;        // SDOx pin is controlled by the module 
            SPI3CON1bits.MODE16 = 0;        // Communication is byte wide (8bit)   
            SPI3CON1bits.MSTEN = 1;         // SPI master mode enabled 
            SPI3CON2bits.SPIBEN = 1;        // Enhanced buffer mode enable
            SPI3STATbits.SISEL = 5;         // Interrupt when the last bit is shifted out of SPIxSR and the transmit is complete
            SPI3CON2bits.FRMEN = 0;         // Frame mode disabled 
            SPI3CON1bits.SSEN = 0;          // GPIO controls SPI /CS      
            // SPI3 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI3CON1bits.PPRE = ppre;
            SPI3CON1bits.SPRE = spre;
            switch (spi_mode)
            {
                case SPI_MODE0:
                    SPI3CON1bits.CKE = 1;   // Mode 0 configurations
                    SPI3CON1bits.CKP = 0;   //
                    SPI3CON1bits.SMP = 0;   // Sample data at beginning valid point                
                    break;

                case SPI_MODE1:
                    SPI3CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI3CON1bits.CKP = 0;   //  
                    SPI3CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI3CON1bits.CKE = 1;   // Mode 2 configurations
                    SPI3CON1bits.CKP = 1;   // 
                    SPI3CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI3CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI3CON1bits.CKP = 1; 
                    SPI3CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI3 input/output pin mapping 
            TRISDbits.TRISD5 = 0;           // RD5 configured as an output (SPI3_SCLK)
            TRISDbits.TRISD4 = 0;           // RD4 configured as an output (SPI3_MOSI)
            TRISDbits.TRISD6 = 0;           // RD6 configured as an output (SPI3_nCS) 
            AUDIO_CODEC_CS_PIN = 1;

            // SPI3 peripheral pin assignment       
            RPOR2bits.RP68R = 0x1F;         // RD4 (RP68) assigned to SPI3_MOSI 
            RPINR29bits.SCK3R = 69;         // RD5 (RP69) assigned to SPI3_SCLK input
            RPOR2bits.RP69R = 0x20;         // RD5 (RP69) assigned to SPI3_SCLK output             
            
            IPC22bits.SPI3IP = 4;           // SPI int priority is 4  
            IEC5bits.SPI3EIE = 1;           // Enable SPI error interrupt detection
            IFS5bits.SPI3IF = 0;            // Clear SPI int flag  
            
#ifdef SPI3_DMA_ENABLE
            spi->DMA_tx_channel = DMA_tx_channel;           
            DMA_init(spi->DMA_tx_channel);
            DMA_set_control_register(spi->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_tx_channel, DMAREQ_SPI3);
            DMA_set_peripheral_address(spi->DMA_tx_channel, (volatile uint16_t)&SPI3BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_tx_channel, __builtin_dmapage(spi3_dma_tx_buf), __builtin_dmaoffset(spi3_dma_tx_buf)); 

            spi->DMA_rx_channel = DMA_rx_channel;           
            DMA_init(spi->DMA_rx_channel);
            DMA_set_control_register(spi->DMA_rx_channel, (DMA_SIZE_BYTE | DMA_TXFER_RD_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_rx_channel, DMAREQ_SPI3);
            DMA_set_peripheral_address(spi->DMA_rx_channel, (volatile uint16_t)&SPI3BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_rx_channel, __builtin_dmapage(spi3_dma_rx_buf), __builtin_dmaoffset(spi3_dma_rx_buf));    
#endif
            
            SPI3STATbits.SPIEN = 1;         // Enable SPI module                   
            break;
          
        // SPI4 is the MikroBus interface   
        case SPI_4:
            IEC7bits.SPI4IE = 0;            // Disable SPI module interrupt
            SPI4STATbits.SPIEN = 0;         // Disable SPI module 
            IFS7bits.SPI4EIF = 0;           // Reset SPI error int flag 
            IFS7bits.SPI4IF = 0;            // Reset SPI int flag 
            SPI4CON1bits.DISSCK = 0;        // Internal serial clock is enabled 
            SPI4CON1bits.DISSDO = 0;        // SDOx pin is controlled by the module 
            SPI4CON1bits.MODE16 = 0;        // Communication is byte wide (8bit)   
            SPI4CON1bits.MSTEN = 1;         // SPI master mode enabled 
            SPI4CON2bits.SPIBEN = 1;        // Enhanced buffer mode enable
            SPI4STATbits.SISEL = 5;         // Interrupt when the last bit is shifted out of SPIxSR and the transmit is complete       
            SPI4CON2bits.FRMEN = 0;         // Frame mode disabled 
            SPI4CON1bits.SSEN = 0;          // GPIO controls SPI /CS      
            // SPI4 clock control, Fsck = FCY / (ppre * spre)
            // Make sure the clock rate is supported by the spi module
            SPI4CON1bits.PPRE = ppre;
            SPI4CON1bits.SPRE = spre;
            switch (spi_mode)
            {
                case SPI_MODE0:
                    SPI4CON1bits.CKE = 1;   // Mode 0 configurations  
                    SPI4CON1bits.CKP = 0;   //
                    SPI4CON1bits.SMP = 0;   // Sample data at beginning valid point             
                    break;

                case SPI_MODE1:
                    SPI4CON1bits.CKE = 0;   // Mode 1 configurations  
                    SPI4CON1bits.CKP = 0;   //  
                    SPI4CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;

                case SPI_MODE2:
                    SPI4CON1bits.CKE = 1;   // Mode 2 configurations
                    SPI4CON1bits.CKP = 1;   // 
                    SPI4CON1bits.SMP = 0;   // Sample data at beginning valid point 
                    break;

                case SPI_MODE3:
                    SPI4CON1bits.CKE = 0;   // Mode 3 configurations  
                    SPI4CON1bits.CKP = 1; 
                    SPI4CON1bits.SMP = 1;   // Sample data at mid valid point 
                    break;
            }

            // SPI4 input/output pin mapping 
            TRISFbits.TRISF0 = 0;           // RF0 configured as an output (SPI4_MOSI)
            TRISDbits.TRISD13 = 1;          // RD13 configured as an input (SPI4_MISO)            
            TRISDbits.TRISD7 = 0;           // RD7 configured as an output (SPI4_SCLK)
            TRISHbits.TRISH15 = 0;          // RH15 configured as an output (MikroBus1_nCS)    
            TRISHbits.TRISH13 = 0;          // RH13 configured as an output (MikroBus2_nCS) 
            MIKROBUS1_CS_PIN = 1;
            MIKROBUS2_CS_PIN = 1;

            // SPI4 peripheral pin assignment       
            RPOR7bits.RP96R = 0x22;         // RF0 (RP96) assigned to SPI4_MOSI 
            RPINR31bits.SDI4R = 77;         // RD13 (RPI77) assigned to SPI4_MISO
            RPINR31bits.SCK4R = 71;         // RD7 (RP71) assigned to SPI4_SCLK input
            RPOR3bits.RP71R = 0x23;         // RD7 (RP71) assigned to SPI4_SCLK output             
            
            IPC30bits.SPI4IP = 4;           // SPI int priority is 4  
            IEC7bits.SPI4EIE = 1;           // Enable SPI error interrupt detection
            IFS7bits.SPI4IF = 0;            // Clear SPI int flag     
            
#ifdef SPI4_DMA_ENABLE
            spi->DMA_tx_channel = DMA_tx_channel;           
            DMA_init(spi->DMA_tx_channel);
            DMA_set_control_register(spi->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TXFER_WR_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_tx_channel, DMAREQ_SPI4);
            DMA_set_peripheral_address(spi->DMA_tx_channel, (volatile uint16_t)&SPI4BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_tx_channel, __builtin_dmapage(spi4_dma_tx_buf), __builtin_dmaoffset(spi4_dma_tx_buf)); 

            spi->DMA_rx_channel = DMA_rx_channel;           
            DMA_init(spi->DMA_rx_channel);
            DMA_set_control_register(spi->DMA_rx_channel, (DMA_SIZE_BYTE | DMA_TXFER_RD_PER | DMA_CHMODE_OPPD));
            DMA_set_request_source(spi->DMA_rx_channel, DMAREQ_SPI4);
            DMA_set_peripheral_address(spi->DMA_rx_channel, (volatile uint16_t)&SPI4BUF);
            DMA_set_buffer_offset_sgl(spi->DMA_rx_channel, __builtin_dmapage(spi4_dma_rx_buf), __builtin_dmaoffset(spi4_dma_rx_buf));    
#endif
            
            SPI4STATbits.SPIEN = 1;         // Enable SPI module              
            break;
    }
    
    spi->SPI_channel = spi_channel;
    spi->mode = spi_mode;
    spi->ppre = ppre;
    spi->spre = spre;
    spi->txfer_state = SPI_TX_IDLE;         // Ready for a transmission
    spi->tx_buf_length = tx_buf_length;
    spi->rx_buf_length = rx_buf_length;
    spi->tx_cnt = 0;
    spi->rx_cnt = 0;
    spi->tx_length = 0;
    spi->last_tx_length = 0;
    spi->tx_remaining = 0;
}

//void SPI_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip)//
//Description : Function write specified array of data, of specified length, to
//              specified chip (define it in SPI.h)
//              This function is to be used with a MASTER SPI only
//              This is a blocking function
//
//Function prototype : void SPI_write (uint8_t channel, uint8_t *data, uint8_t length, uint8_t chip)
//
//Enter params       :  uint8_t channel : SPI_x channel
//                      uint8_t *data   : Array of data to write
//                      uint8_t length  : Array length
//                      uint8_t chip    : spi chip to select
//                      
//
//Exit params        : None
//
//Function call      : SPI_write(SPI_2, buf, (length+4), FLASH_MEMORY_CS); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          12-02-2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t SPI_write (STRUCT_SPI *spi, uint8_t chip)
{
    uint16_t i = 0;   
    // *** Data should be loaded in SPI_load_tx_buffer function ***
    // Check if previous transaction was completed
    while (SPI_module_busy(spi) != SPI_MODULE_FREE); 
    // Reset state machine variables
    spi->rx_cnt = 0;                // Set receive counter to 0 
    spi->tx_cnt = 0;                // Set transmit counter to 0
    spi->chip = chip;               // Set SPI module chip to struct 
    spi->last_tx_length = 0;        
    spi->tx_remaining = 0;
    switch (spi->SPI_channel)
    {
        case SPI_1:
            IEC0bits.SPI1IE = 1;                                        // Enable SPI module interrupt 
            SPI_assert_cs(spi);                                  // Assert /CS from specified SPI chip  
            // Enhanced buffer mode
            // If <= 8 bytes transfer, fill FIFO once
            if (spi->tx_length <= 8)
            {
                for (i=0; i<spi->tx_length; i++)
                {
                    SPI1BUF = spi->tx_data[i];                   
                }
                spi->tx_cnt = spi->tx_length;
                spi->last_tx_length = spi->tx_cnt;
            }
            // If > 8 bytes transfer, fill FIFO once, increm tx cnt
            else
            {
                for (i=0; i<8; i++)
                {
                    SPI1BUF = spi->tx_data[i];
                }
                spi->tx_cnt = 8;  
                spi->last_tx_length = spi->tx_cnt;
            }            
            break;

        case SPI_2:                                     
            IEC2bits.SPI2IE = 1; 
            SPI_assert_cs(spi);
            if (spi->tx_length <= 8)
            {
                for (i=0; i<spi->tx_length; i++)
                {
                    SPI2BUF = spi->tx_data[i];                  
                }
                spi->tx_cnt = spi->tx_length;
                spi->last_tx_length = spi->tx_cnt;
            }
            else
            {
                for (i=0; i<8; i++)
                {
                    SPI2BUF = spi->tx_data[i];
                }
                spi->tx_cnt = 8;  
                spi->last_tx_length = spi->tx_cnt;
            }                      
            break;

        case SPI_3:
            IEC5bits.SPI3IE = 1;                     
            SPI_assert_cs(spi);
            if (spi->tx_length <= 8)
            {
                for (i=0; i<spi->tx_length; i++)
                {
                    SPI3BUF = spi->tx_data[i];                 
                }
                spi->tx_cnt = spi->tx_length;
                spi->last_tx_length = spi->tx_cnt;
            }
            else
            {
                for (i=0; i<8; i++)
                {
                    SPI3BUF = spi->tx_data[i];
                }
                spi->tx_cnt = 8;  
                spi->last_tx_length = spi->tx_cnt;
            } 
            break;

        case SPI_4:                               
            IEC7bits.SPI4IE = 1;
            SPI_assert_cs(spi);
            if (spi->tx_length <= 8)
            {
                for (i=0; i<spi->tx_length; i++)
                {
                    SPI4BUF = spi->tx_data[i];                
                }
                spi->tx_cnt = spi->tx_length;
                spi->last_tx_length = spi->tx_cnt;
            }
            else
            {
                for (i=0; i<8; i++)
                {
                    SPI4BUF = spi->tx_data[i];
                }
                spi->tx_cnt = 8;  
                spi->last_tx_length = spi->tx_cnt;
            }
            break;

        default:
            return 0;
            break;               
    }        
    spi->txfer_state = SPI_TX_IN_PROGRESS;
    return 1;
}

// Before using SPI_write, the SPI_load_dma_tx_buffer() function should have been used to fill the DMA tx buffer
uint8_t SPI_write_dma (STRUCT_SPI *spi, uint8_t chip)
{
    // *** Data should be loaded in SPI_load_dma_tx_buffer function ***    
    spi->rx_cnt = 0;               // Set receive counter to 0 
    spi->tx_cnt = 0;               // Set transmit counter to 0
    spi->chip = chip;              // Set SPI module chip to struct 
    spi->last_tx_length = 0;
    spi->tx_remaining = 0;
    spi->txfer_state = SPI_TX_IN_PROGRESS; // Set SPI module state to transmit idle                  
        
    DMA_set_txfer_length(spi->DMA_tx_channel, spi->tx_length - 1);
    DMA_set_txfer_length(spi->DMA_rx_channel, spi->tx_length - 1);  // RX = TX in length
    DMA_enable(spi->DMA_tx_channel); 
    DMA_enable(spi->DMA_rx_channel);   
    SPI_assert_cs(spi);
    DMA_force_txfer(spi->DMA_tx_channel);
    SPI_set_interrupt_enable(spi);
    return 1;
}

// Blocking call, loads SPI tx data to SPI struct tx buffer
uint8_t SPI_load_tx_buffer (STRUCT_SPI *spi, uint8_t *data, uint16_t length)
{
    uint16_t i=0;
    while (SPI_module_busy(spi) != SPI_MODULE_FREE);
    // Saturate length
    if (length > spi->tx_buf_length)
    {
        length = spi->tx_buf_length;
    }

    for (i=0; i<length; i++)                       
    {
       spi->tx_data[i] = data[i];
    }
    spi->tx_length = length;
    spi->txfer_state = SPI_TX_LOADED;
    return 1;
}

uint8_t SPI_load_dma_tx_buffer (STRUCT_SPI *spi, uint8_t *data, uint16_t length)
{
    uint16_t i=0;
    // Saturate length
    if (length > spi->tx_buf_length)
    {
        length = spi->tx_buf_length;
    }
    
    if (spi->SPI_channel == SPI_1)
    {
        for (i=0; i<length; i++)  
        {
#ifdef SPI1_DMA_ENABLE
            spi1_dma_tx_buf[i] = data[i];
#endif
        }
    }
    
    else if (spi->SPI_channel == SPI_2)
    {
        for (i=0; i<length; i++)  
        {
#ifdef SPI2_DMA_ENABLE
            spi2_dma_tx_buf[i] = data[i];
#endif
        }
    }  
    
    else if (spi->SPI_channel == SPI_3)
    {
        for (i=0; i<length; i++)  
        {
#ifdef SPI3_DMA_ENABLE
            spi3_dma_tx_buf[i] = data[i];
#endif
        }
    }
    
    else if (spi->SPI_channel == SPI_4)
    {
        for (i=0; i<length; i++)  
        {
#ifdef SPI4_DMA_ENABLE
            spi4_dma_tx_buf[i] = data[i];
#endif
        }
    }  
    else
        return 0;
    
    spi->tx_length = length;
    spi->txfer_state = SPI_TX_LOADED;
    return 1;
}

uint8_t SPI_set_interrupt_enable (STRUCT_SPI *spi)
{
    switch(spi->SPI_channel)
    {
        case SPI_1:
            IEC0bits.SPI1IE = 1;
            break;
            
        case SPI_2:
            IEC2bits.SPI2IE = 1;
            break;

        case SPI_3:
            IEC5bits.SPI3IE = 1;
            break;
            
        case SPI_4:
            IEC7bits.SPI4IE = 1;
            break; 
            
        default:
            return 0;
            break;
    }
    return 1;
}

uint8_t SPI_release_port (STRUCT_SPI *spi)
{
    SPI_deassert_cs(spi);
    spi->txfer_state = SPI_TX_COMPLETE;  
    switch (spi->SPI_channel)
    {
        case SPI_1:    
            IEC0bits.SPI1IE = 0;      
            break;
            
        case SPI_2: 
            IEC2bits.SPI2IE = 0;      
            break;
            
        case SPI_3:     
            IEC5bits.SPI3IE = 0;      
            break;
            
        case SPI_4:
            IEC7bits.SPI4IE = 0;      
            break; 
            
        default:
            return 0;
            break;
    }
    return 1;
}

uint8_t SPI_get_dma_rx_buffer_at_index (STRUCT_SPI *spi, uint16_t index)
{
    if (index > SPI_BUF_LENGTH)
    {
        index = SPI_BUF_LENGTH;
    }
    if (spi->SPI_channel == SPI_1)
    {
#ifdef SPI1_DMA_ENABLE
        return spi1_dma_rx_buf[index];
#endif
#ifndef SPI1_DMA_ENABLE
        return 0;
#endif
    }
    
    else if (spi->SPI_channel == SPI_2)
    {
#ifdef SPI2_DMA_ENABLE
        return spi2_dma_rx_buf[index];
#endif
#ifndef SPI2_DMA_ENABLE
        return 0;
#endif
    }
    
    else if (spi->SPI_channel == SPI_3)
    {
#ifdef SPI3_DMA_ENABLE
        return spi3_dma_rx_buf[index];
#endif
#ifndef SPI3_DMA_ENABLE
        return 0;
#endif
    }
    
    else if (spi->SPI_channel == SPI_4)
    {
#ifdef SPI4_DMA_ENABLE
        return spi4_dma_rx_buf[index];
#endif
#ifndef SPI4_DMA_ENABLE
        return 0;
#endif
    }
    else      
        return 0;
}

uint8_t * SPI_unload_dma_rx_buffer (STRUCT_SPI *spi)
{
    uint16_t i;

    if (spi->SPI_channel == SPI_1)
    {
#ifdef SPI1_DMA_ENABLE
        for (i=0; i<spi->tx_length; i++)
        {
            spi->rx_data[i] = spi1_dma_rx_buf[i];
        }
#endif
    }
    
    else if (spi->SPI_channel == SPI_2)
    {
#ifdef SPI2_DMA_ENABLE
        for (i=0; i<spi->tx_length; i++)
        {
            spi->rx_data[i] = spi2_dma_rx_buf[i];
        }
#endif
    }
    
    else if (spi->SPI_channel == SPI_3)
    {
#ifdef SPI3_DMA_ENABLE
        for (i=0; i<spi->tx_length; i++)
        {
            spi->rx_data[i] = spi3_dma_rx_buf[i];
        }
#endif
    }
    
    else if (spi->SPI_channel == SPI_4)
    {
#ifdef SPI4_DMA_ENABLE
        for (i=0; i<spi->tx_length; i++)
        {
            spi->rx_data[i] = spi4_dma_rx_buf[i];
        }
#endif
    }
    
    else
        return 0;
    return &spi->rx_data[0];
}

//*************void SPI_deassert_cs (uint8_t chip)*********************//
//Description : Function deassert spi /CS of specified chip
//              If using I/Os whose primary function was not an SPI nCS, define
//              the case in spi.h and define the LATx bit associated with the pin
//              Make sure to set the TRISx bit to 0 prior to an SPI transaction
//
//Function prototype : void SPI_deassert_cs (uint8_t chip)
//
//Enter params       : uint8_t chip : chip to deassert /CS
//
//Exit params        : None
//
//Function call      : SPI_deassert_cs(FT8XX_EVE_CS); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t SPI_deassert_cs (STRUCT_SPI *spi)
{
    switch (spi->chip)
    {
        case FT8XX_EVE_CS:
            FT8XX_EVE_CS_PIN = 1;
            break;  
           
        case SD_CARD_CS:
            SD_CARD_CS_PIN = 1;
            break;

        case FLASH_MEMORY_CS:
            FLASH_MEMORY_CS_PIN = 1;
            break;

        case AUDIO_CODEC_CS:
            AUDIO_CODEC_CS_PIN = 1;
            break;

        case MIKROBUS1_CS:
            MIKROBUS1_CS_PIN = 1;
            break;

        case MIKROBUS2_CS:
            MIKROBUS2_CS_PIN = 1;
            break;

        default:
            return 0;
            break;
    }
    return 1;    
}

//*****************void SPI_assert_cs (uint8_t chip)*******************//
//Description : Function assert spi /CS to specified chip
//              If using I/Os whose primary function was not an SPI nCS, define
//              the case in spi.h and define the LATx bit associated with the pin
//              Make sure to set the TRISx bit to 0 prior to an SPI transaction
//
//Function prototype : void SPI_assert_cs (uint8_t chip)
//
//Enter params       : uint8_t chip : chip to assert /CS
//
//Exit params        : None
//
//Function call      : SPI_assert_cs(FT8XX_EVE_CS); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t SPI_assert_cs (STRUCT_SPI *spi)
{
    switch (spi->chip)
    {
        case FT8XX_EVE_CS:
            FT8XX_EVE_CS_PIN = 0;
            break;  
           
        case SD_CARD_CS:
            SD_CARD_CS_PIN = 0;
            break;

        case FLASH_MEMORY_CS:
            FLASH_MEMORY_CS_PIN = 0;
            break;

        case AUDIO_CODEC_CS:
            AUDIO_CODEC_CS_PIN = 0;
            break;

        case MIKROBUS1_CS:
            MIKROBUS1_CS_PIN = 0;
            break;

        case MIKROBUS2_CS:
            MIKROBUS2_CS_PIN = 0;
            break;
           
        default:
            return 0;
            break;           
    }
    return 1;
}

//************uint8_t * SPI_get_rx_buffer (uint8_t channel)******************//
//Description : Function returns a pointer to the 1st element of the specified
//              channel spi rx buffer
//
//Function prototype : uint8_t * SPI_get_rx_buffer (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel                   
//
//Exit params        : uint8_t * : pointer to 1st element location
//
//Function call      : uint8_t * = SPI_get_rx_buffer(SPI_2); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t * SPI_get_rx_buffer (STRUCT_SPI *spi)
{
    return(&spi->rx_data[0]);// Return array pointer on 1st element
}

//*****uint8_t SPI_get_rx_buffer_index (uint8_t channel, uint8_t index)*******//
//Description : Function returns a byte from the specified spi channel index
//
//Function prototype : uint8_t * SPI_get_rx_buffer (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel                   
//
//Exit params        : uint8_t * : pointer to 1st element location
//
//Function call      : uint8_t * = SPI_get_rx_buffer(SPI_2); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t SPI_get_rx_buffer_index (STRUCT_SPI *spi, uint16_t index)
{
    return spi->rx_data[index];
}

//***************uint8_t SPI_get_txfer_state (uint8_t channel)*********************//
//Description : Function checks wether a transfer is in progress on the specified
//              SPI channel or if the bus is free.
//
//Function prototype : uint8_t SPI_txfer_state (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel
//
//Exit params        : uint8_t       0 : Bus is BUSY -> SPI_TX_BUSY
//                                   1 : Bus is FREE -> SPI_TX_COMPLETE
//
//Function call      : uint8_t = SPI_get_txfer_state(SPI_1); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t SPI_get_txfer_state (STRUCT_SPI *spi)
{
    if (spi->txfer_state == SPI_TX_IDLE)
    {
        return SPI_TX_IDLE;
    }
    else if (spi->txfer_state == SPI_TX_LOADED)
    {
        return SPI_TX_LOADED;
    }
    else if (spi->txfer_state == SPI_TX_IN_PROGRESS)
    {
        return SPI_TX_IN_PROGRESS;
    }
    else if (spi->txfer_state == SPI_TX_COMPLETE)
    {
        return SPI_TX_COMPLETE;
    }
    else return SPI_TX_IDLE;
}

void SPI_set_txfer_state (STRUCT_SPI *spi, uint8_t state)
{
    spi->txfer_state = state;
}

//***************uint8_t SPI_module_busy (uint8_t channel)********************//
//Description : Function checks wether the spi channel module interrupt is set
//              or if it is free. Polling this bit indicates wether a transfer or receive
//              operation is in progress or if the module is free.
//
//Function prototype : uint8_t SPI_module_busy (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel
//
//Exit params        : uint8_t       0 : Bus is FREE -> SPIxIE = 0
//                                   1 : Bus is BUSY -> SPIxIE = 1
//
//Function call      : uint8_t = SPI_module_busy(SPI_1); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t SPI_module_busy (STRUCT_SPI *spi)
{
    switch (spi->SPI_channel)
    {
        case SPI_1:
            return IEC0bits.SPI1IE;
            break;
            
        case SPI_2:
            return IEC2bits.SPI2IE;
            break;
            
        case SPI_3:
            return IEC5bits.SPI3IE;
            break;
            
        case SPI_4:
            return IEC7bits.SPI4IE;
            break;
            
        default:
            return SPI_MODULE_BUSY;
            break;
    }
}

//*****************void SPI_flush_txbuffer (uint8_t channel)******************//
//Description : Function flushes the TX buffer of the specified SPI channel
//              and reset it's value to all 0
//
//Function prototype : void SPI_flush_txbuffer (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel
//
//Exit params        : None
//
//Function call      : SPI_flush_txbuffer(SPI_1); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void SPI_flush_txbuffer (STRUCT_SPI *spi)
{
    uint16_t i = 0;
    for (; i < spi->tx_buf_length; i++)
    {
        spi->tx_data[i] = 0;
    }
}

//*****************void SPI_flush_rxbuffer (uint8_t channel)******************//
//Description : Function flushes the RX buffer of the specified SPI channel
//              and reset it's value to all 0
//
//Function prototype : void SPI_flush_rxbuffer (uint8_t channel)
//
//Enter params       : uint8_t channel : SPI_x channel
//
//Exit params        : None
//
//Function call      : SPI_flush_rxbuffer(SPI_1); 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void SPI_flush_rxbuffer (STRUCT_SPI *spi)
{
    uint16_t i = 0;
    for (; i < spi->rx_buf_length; i++)
    {
        spi->rx_data[i] = 0;
    }    
}

//**************************SPI1 interrupt function***************************//
//Description : SPI interrupt with enhanced buffer.
//
//Function prototype : _SPI1Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI1Interrupt(void)
{                
    uint16_t i=0;  
    uint8_t temp;
    // Based on last transfer length, read SPI RXFIFO and put value in struct rx buffer
    for (i=0; i<SPI_struct[SPI_1].last_tx_length; i++)
    {
        SPI_struct[SPI_1].rx_data[SPI_struct[SPI_1].rx_cnt++] = SPI1BUF;       
    }
    
    // Was there more data received than expected? Error, flush RX buffer
    while (SPI1STATbits.SRXMPT == 0)
    {
        temp = SPI1BUF;
    }
    SPI1STATbits.SPIROV = 0;
    
    // Is there more data to transmit?
    if (SPI_struct[SPI_1].tx_cnt < SPI_struct[SPI_1].tx_length)
    {     
        SPI_struct[SPI_1].tx_remaining = SPI_struct[SPI_1].tx_length - SPI_struct[SPI_1].tx_cnt;
        // Is there at least 8+ bytes left to transmit?
        if (SPI_struct[SPI_1].tx_remaining  >= 8)
        {
            for (i=0; i<8; i++)
            {
                SPI1BUF = SPI_struct[SPI_1].tx_data[SPI_struct[SPI_1].tx_cnt++];
            }
            SPI_struct[SPI_1].last_tx_length = 8;
        }
        else
        {
            for (i=0; i<SPI_struct[SPI_1].tx_remaining ; i++)
            {
                SPI1BUF = SPI_struct[SPI_1].tx_data[SPI_struct[SPI_1].tx_cnt++];
            } 
            SPI_struct[SPI_1].last_tx_length = SPI_struct[SPI_1].tx_remaining;
        }
    }
    else
    {
        if (SPI1STATbits.SRXMPT == 1)
        {  
            SPI_deassert_cs(&SPI_struct[SPI_1]);
            SPI_struct[SPI_1].txfer_state = SPI_TX_COMPLETE;       
            IEC0bits.SPI1IE = 0;             
        }
    } 
    IFS0bits.SPI1IF = 0;
}

//**************************SPI2interrupt function***************************//
//Description : SPI interrupt with enhanced buffer.
//
//Function prototype : _SPI2Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          04/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI2Interrupt(void)
{  
#ifndef SPI2_DMA_ENABLE
    uint16_t i=0;  
    uint8_t temp;
    // Based on last transfer length, read SPI RXFIFO and put value in struct rx buffer
    for (i=0; i<SPI_struct[SPI_2].last_tx_length; i++)
    {
        SPI_struct[SPI_2].rx_data[SPI_struct[SPI_2].rx_cnt++] = SPI2BUF;
    }
    
    // Was there more data received than expected? Error, flush RX buffer
    while (SPI2STATbits.SRXMPT == 0)
    {
        temp = SPI2BUF;
    }
    SPI2STATbits.SPIROV = 0;
    
    // Is there more data to transmit?
    if (SPI_struct[SPI_2].tx_cnt < SPI_struct[SPI_2].tx_length)
    {     
        SPI_struct[SPI_2].tx_remaining = SPI_struct[SPI_2].tx_length - SPI_struct[SPI_2].tx_cnt;
        // Is there at least 8+ bytes left to transmit?
        if (SPI_struct[SPI_2].tx_remaining  >= 8)
        {
            for (i=0; i<8; i++)
            {
                SPI2BUF = SPI_struct[SPI_2].tx_data[SPI_struct[SPI_2].tx_cnt++];
            }
            SPI_struct[SPI_2].last_tx_length = 8;
        }
        else
        {
            for (i=0; i<SPI_struct[SPI_2].tx_remaining ; i++)
            {
                SPI2BUF = SPI_struct[SPI_2].tx_data[SPI_struct[SPI_2].tx_cnt++];
            } 
            SPI_struct[SPI_2].last_tx_length = SPI_struct[SPI_2].tx_remaining;
        }
    }
    else
    {
        if (SPI2STATbits.SRXMPT == 1)
        {  
            SPI_deassert_cs(&SPI_struct[SPI_2]);
            SPI_struct[SPI_2].txfer_state = SPI_TX_COMPLETE;       
            IEC2bits.SPI2IE = 0;             
        }
    } 
#endif
    IFS2bits.SPI2IF = 0;
}

//**************************SPI3 interrupt function***************************//
//Description : SPI interrupt with enhanced buffer.
//
//Function prototype : _SPI3Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          13/01/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI3Interrupt(void)
{
    uint16_t i=0;
    uint8_t temp;
    // Based on last transfer length, read SPI RXFIFO and put value in struct rx buffer
    for (i=0; i<SPI_struct[SPI_3].last_tx_length; i++)
    {
        SPI_struct[SPI_3].rx_data[SPI_struct[SPI_3].rx_cnt++] = SPI3BUF;        
    }
    
    // Was there more data received than expected? Error, flush RX buffer
    while (SPI3STATbits.SRXMPT == 0)
    {
        temp = SPI3BUF;
    }
    SPI3STATbits.SPIROV = 0;
    
    // Is there more data to transmit?
    if (SPI_struct[SPI_3].tx_cnt < SPI_struct[SPI_3].tx_length)
    {     
        SPI_struct[SPI_3].tx_remaining = SPI_struct[SPI_3].tx_length - SPI_struct[SPI_3].tx_cnt;
        // Is there at least 8+ bytes left to transmit?
        if (SPI_struct[SPI_3].tx_remaining  >= 8)
        {
            for (i=0; i<8; i++)
            {
                SPI3BUF = SPI_struct[SPI_3].tx_data[SPI_struct[SPI_3].tx_cnt++];
            }
            SPI_struct[SPI_3].last_tx_length = 8;
        }
        else
        {
            for (i=0; i<SPI_struct[SPI_3].tx_remaining ; i++)
            {
                SPI3BUF = SPI_struct[SPI_3].tx_data[SPI_struct[SPI_3].tx_cnt++];
            } 
            SPI_struct[SPI_3].last_tx_length = SPI_struct[SPI_3].tx_remaining;
        }
    }
    else
    {
        if (SPI3STATbits.SRXMPT == 1)
        {  
            SPI_deassert_cs(&SPI_struct[SPI_3]);
            SPI_struct[SPI_3].txfer_state = SPI_TX_COMPLETE;       
            IEC5bits.SPI3IE = 0;             
        }
    } 
    IFS5bits.SPI3IF = 0; 
}

//**************************SPI4 interrupt function***************************//
//Description : SPI interrupt with enhanced buffer.
//
//Function prototype : _SPI4Interrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          13/01/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, no_auto_psv)) _SPI4Interrupt(void)
{
    uint8_t temp; 
    uint16_t i=0;
    // Based on last transfer length, read SPI RXFIFO and put value in struct rx buffer
    for (i=0; i<SPI_struct[SPI_4].last_tx_length; i++)
    {
        SPI_struct[SPI_4].rx_data[SPI_struct[SPI_4].rx_cnt++] = SPI4BUF;       
    }
    
    // Was there more data received than expected? Error, flush RX buffer
    while (SPI4STATbits.SRXMPT == 0)
    {
        temp = SPI4BUF;
    }
    SPI4STATbits.SPIROV = 0;

    // Is there more data to transmit?
    if (SPI_struct[SPI_4].tx_cnt < SPI_struct[SPI_4].tx_length)
    {     
        SPI_struct[SPI_4].tx_remaining = SPI_struct[SPI_4].tx_length - SPI_struct[SPI_4].tx_cnt;
        // Is there at least 8+ bytes left to transmit?
        if (SPI_struct[SPI_4].tx_remaining  >= 8)
        {
            for (i=0; i<8; i++)
            {
                SPI4BUF = SPI_struct[SPI_4].tx_data[SPI_struct[SPI_4].tx_cnt++];
            }
            SPI_struct[SPI_4].last_tx_length = 8;
        }
        else
        {
            for (i=0; i<SPI_struct[SPI_4].tx_remaining ; i++)
            {
                SPI4BUF = SPI_struct[SPI_4].tx_data[SPI_struct[SPI_4].tx_cnt++];
            } 
            SPI_struct[SPI_4].last_tx_length = SPI_struct[SPI_4].tx_remaining;
        }
    }
    else
    {
        if (SPI4STATbits.SRXMPT == 1)
        {  
            SPI_deassert_cs(&SPI_struct[SPI_4]);
            SPI_struct[SPI_4].txfer_state = SPI_TX_COMPLETE;       
            IEC7bits.SPI4IE = 0;             
        }                      
    }
    IFS7bits.SPI4IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI1ErrInterrupt(void)
{
    SPI1STATbits.SPIROV = 0;
    IFS0bits.SPI1EIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI2ErrInterrupt(void)
{
    SPI2STATbits.SPIROV = 0;
    IFS2bits.SPI2EIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI3ErrInterrupt(void)
{
    SPI3STATbits.SPIROV = 0;
    IFS5bits.SPI3EIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI4ErrInterrupt(void)
{
    SPI4STATbits.SPIROV = 0;
    IFS7bits.SPI4EIF = 0;
}