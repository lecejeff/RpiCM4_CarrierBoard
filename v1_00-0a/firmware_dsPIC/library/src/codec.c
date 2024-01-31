//***************************************************************************//
// File      : codec.c
//
// Functions : 
//
// Includes  : codec.h
//           
// Purpose   : Driver for the dsPeak SGTL5000 audio codec using DCI as audio
//             interface and SPI as configuration interface 
//
//Jean-Francois Bilodeau    MPLab X v5.45   13/01/2021
//****************************************************************************//
#include "codec.h"
STRUCT_CODEC CODEC_struct[CODEC_QTY]; 

#ifdef DCI0_DMA_ENABLE
__eds__ uint16_t codec_dma_tx_buf_A[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
__eds__ uint16_t codec_dma_rx_buf_A[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
__eds__ uint16_t codec_dma_tx_buf_B[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
__eds__ uint16_t codec_dma_rx_buf_B[CODEC_BLOCK_TRANSFER] __attribute__((eds,space(dma)));
#endif

// DCI operates in slave mode on dsPeak
void DCI_init (STRUCT_CODEC *codec, uint16_t tx_buf_length, uint16_t rx_buf_length,
                uint8_t DMA_tx_channel, uint8_t DMA_rx_channel)
{
    uint16_t i = 0;
    
    DCI_disable(codec);
   
    DCICON1bits.COFSM = 0x1;// Frame sync set to I2S frame sync mode
    DCICON1bits.CSCKD = 1;  // CSCK pin is an input when DCI module is enabled
    DCICON1bits.COFSD = 1;  // COFS pin is an input when DCI module is enabled
    DCICON1bits.CSCKE = 1;  // Sample incoming data on the rising edge of CSCK
    DCICON2bits.WS = 0xF;   // DCI data word size is 16 bits (standard)
    DCICON3bits.BCG = 0;    // Clear baud-rate generator

#ifndef DCI0_DMA_ENABLE
    // DCI with interrupt, without DMA
    DCICON2bits.COFSG = 1;  // Two data words transfered per I2S frame (Right(16) + Left(16))
    DCICON2bits.BLEN = 1;   // Enable interrupt after 2 data word transmitted    
    RSCONbits.RSE0 = 1;     // Enable receive time slot 0
    RSCONbits.RSE1 = 1;     // Enable receive time slot 1
    TSCONbits.TSE0 = 1;     // Enable transmit time slot 0
    TSCONbits.TSE1 = 1;     // Enable transmit time slot 1    
    for (; i < (2 * CODEC_BLOCK_TRANSFER); i++)
    {
        codec->DCI_receive_buffer[i] = 0;  // Initialize buffer
        codec->DCI_transmit_buffer[i] = 0; // Initialize buffer       
    }
#endif

    TRISDbits.TRISD1 = 1;   // RD1 configured as an input (I2S_COFS)
    TRISDbits.TRISD2 = 1;   // RD2 configured as an input (I2S_SCLK)
    TRISDbits.TRISD3 = 0;   // RD3 configured as an output (I2S_DOUT)
    TRISDbits.TRISD12 = 1;  // RD12 configured as an input (I2S_DIN)
    
    RPINR25bits.COFSR = 65; // RD1 (RP65) assigned to I2S_COFS    
    RPINR24bits.CSCKR = 66; // RD2 (RP66) assigned to I2C_SCLK
    RPOR1bits.RP67R = 0x0B; // RD3 (RP67) assigned to I2S_DOUT
    RPINR24bits.CSDIR = 76; // RD12 (RPI76) assigned to I2S_DIN

    codec->DCI_transmit_enable = DCI_TRANSMIT_DISABLE;
    codec->DCI_receive_enable = DCI_RECEIVE_DISABLE;
    codec->interrupt_flag = 0;
    codec->DCI_receive_counter = 0;   // Initialize counter to 0
    codec->DCI_transmit_counter = 0;  // Initialize counter to 0
    
    
    if (tx_buf_length > CODEC_BLOCK_TRANSFER){tx_buf_length = CODEC_BLOCK_TRANSFER;}    // Saturate tx buffer length
    codec->DCI_transmit_length = tx_buf_length;
    if (rx_buf_length > CODEC_BLOCK_TRANSFER){rx_buf_length = CODEC_BLOCK_TRANSFER;}    // Saturate rx buffer length
    codec->DCI_receive_length = rx_buf_length;
      
#ifdef DCI0_DMA_ENABLE
    // DCI with DMA
    for (i=0; i < codec->DCI_transmit_length; i++)
    {
        codec_dma_tx_buf_A[i] = 0;    // Initialize Tx buffer
        codec_dma_tx_buf_B[i] = 0;    // Initialize Tx buffer
    }
    
    for (i=0; i < codec->DCI_receive_length; i++)
    {
        codec_dma_rx_buf_A[i] = 0;    // Initialize Rx buffer       
        codec_dma_rx_buf_B[i] = 0;    // Initialize Rx buffer 
    }
    codec->DMA_tx_buf_pp = 0;
    codec->DMA_rx_buf_pp = 0;
    // If DMA is enabled, the DCI transfers only 1x frame / tx-rx slot
    DCICON2bits.COFSG = 1;  // Data frame has 2x words (left + right sample) -> 1x frame equals 32b
    DCICON2bits.BLEN = 0;   // Enable interrupt after 1 data word transfered
    RSCONbits.RSE0 = 1;     // Enable receive time slot 0
    RSCONbits.RSE1 = 1;     // Enable receive time slot 1
    TSCONbits.TSE0 = 1;     // Enable transmit time slot 0
    TSCONbits.TSE1 = 1;     // Enable transmit time slot 1
    
    codec->DMA_tx_channel = DMA_tx_channel;
    DMA_init(codec->DMA_tx_channel);
    DMA_set_control_register(codec->DMA_tx_channel, (DMA_SIZE_WORD | DMA_TXFER_WR_PER | DMA_CHMODE_CPPE));
    DMA_set_request_source(codec->DMA_tx_channel, DMAREQ_DCI);
    DMA_set_peripheral_address(codec->DMA_tx_channel, (volatile uint16_t)&TXBUF0);
    DMA_set_buffer_offset_pp(codec->DMA_tx_channel, 0, __builtin_dmaoffset(codec_dma_tx_buf_A), 0, __builtin_dmaoffset(codec_dma_tx_buf_B));
    //DMA7STAH = 0;
    //DMA7STAL = __builtin_dmaoffset(codec_dma_tx_buf_A);   
    //DMA7STBH = 0;
    //DMA7STBL = __builtin_dmaoffset(codec_dma_tx_buf_B);       
    
    codec->DMA_rx_channel = DMA_rx_channel;
    DMA_init(codec->DMA_rx_channel);
    DMA_set_control_register(codec->DMA_rx_channel, (DMA_SIZE_WORD | DMA_TXFER_RD_PER | DMA_CHMODE_CPPE));
    DMA_set_request_source(codec->DMA_rx_channel, DMAREQ_DCI);
    DMA_set_peripheral_address(codec->DMA_rx_channel, (volatile uint16_t)&RXBUF0);
    DMA_set_buffer_offset_pp(codec->DMA_rx_channel, 0, __builtin_dmaoffset(codec_dma_rx_buf_A), 0, __builtin_dmaoffset(codec_dma_rx_buf_B));
    //DMA8STAH = 0;
    //DMA8STAL = __builtin_dmaoffset(codec_dma_rx_buf_A); 
    //DMA8STBH = 0;
    //DMA8STBL = __builtin_dmaoffset(codec_dma_rx_buf_B);     
       
    DMA_set_txfer_length(codec->DMA_tx_channel, codec->DCI_transmit_length - 1);    // 0 = 1x transfer
    DMA_set_txfer_length(codec->DMA_rx_channel, codec->DCI_receive_length - 1);     // 0 = 1x transfer
    
    DMA_enable(codec->DMA_tx_channel);
    DMA_enable(codec->DMA_rx_channel);
#endif    
}

void CODEC_init (STRUCT_CODEC *codec, STRUCT_SPI *spi, uint8_t spi_channel, uint8_t sys_fs, 
                uint16_t tx_buf_length, uint16_t rx_buf_length, uint8_t DMA_tx_channel, uint8_t DMA_rx_channel)
{
    uint32_t pll_out_freq = 0;
    uint16_t pll_int_divisor = 0;
    uint16_t pll_frac_divisor = 0;
    
    DCI_init(codec, tx_buf_length, rx_buf_length, DMA_tx_channel, DMA_rx_channel);
    
    codec->SPI_channel = spi_channel;
    codec->spi_ref = spi; 
    
    // SPI3 port used for SGTL5000 max frequency is 9MHz
    // 70Mips / (1 * 8) = 8.75MHz
    SPI_init(codec->spi_ref, codec->SPI_channel, SPI_MODE0, PPRE_1_1, SPRE_8_1, 4, 4, 0xFF, 0xFF);  // DMA not used on CODEC   
    
    // Enable 1.8V output to CODEC
    TRISKbits.TRISK1 = 0;               // Set CODEC_1V8_EN to output
    LATKbits.LATK1 = 0;                 // Disable CODEC
    __delay_ms(250);                    // Power-off delay
    LATKbits.LATK1 = 1;                 // Enable 1.8V
    __delay_ms(250);                    // Power-on delay
    
    // Since SPI read is not supported, write default reset values to struct
    // registers to support the modify operation. Reset values taken from datasheet
    codec->CHIP_DIG_POWER = 0;                  // Every audio subsystem is disabled
    codec->CHIP_CLK_CTRL = 0x0008;              // SYS_FS = 48kHz               
    codec->CHIP_I2S_CTRL = 0x0010;              // DLEN = 24b
    codec->CHIP_SSS_CTRL = 0x0010;              // DAC_SELECT = I2S_IN
    codec->CHIP_ADCDAC_CTRL = 0x020C;           // VOL_RAMP ENABLED, DAC L/R MUTED
    codec->CHIP_DAC_VOL = 0x3C3C;               // DAC VOL L/R = 0dB
    codec->CHIP_PAD_STRENGTH = 0x015F;          // I2S PAD STRENGTH = 0x1 = 4.02mA, I2C PAD STRENGTH = 0x3 = 12.05mA
    codec->CHIP_ANA_ADC_CTRL = 0;               // No change in ADC range
    codec->CHIP_ANA_HP_CTRL = 0x1818;           // HP VOL L/R = 0dB
    codec->CHIP_ANA_CTRL = 0x0111;              // LineOut = Mute, HP = Mute, ADC = Mute
    codec->CHIP_LINREG_CTRL = 0;                //
    codec->CHIP_REF_CTRL = 0;                   //
    codec->CHIP_MIC_CTRL = 0;                   //
    codec->CHIP_LINE_OUT_CTRL = 0;              //
    codec->CHIP_LINE_OUT_VOL = 0x0404;          // LineOut output level, refer to equation in datasheet
    codec->CHIP_ANA_POWER = 0x7060;             // DAC Stereo, Power-up linreg, ADC stereo, Power-up ref bias
    codec->CHIP_PLL_CTRL = 0x5000;              // PLL INT and FRAC divisors, refer to equation in datasheet
    codec->CHIP_CLK_TOP_CTRL = 0;               //
    codec->CHIP_ANA_STATUS = 0;                 //
    codec->CHIP_ANA_TEST1 = 0x01C0;             // HP = ClassAB, VGND center for best antipop performance
    codec->CHIP_ANA_TEST2 = 0;                  //
    codec->CHIP_SHORT_CTRL = 0;                 //
    codec->DAP_CONTROL = 0;                     //
    codec->DAP_PEQ = 0;                         //
    codec->DAP_BASS_ENHANCE = 0x0040;           // Bass enhance cutoff frequency = 175Hz
    codec->DAP_BASS_ENHANCE_CTRL = 0x051F;    
    codec->DAP_AUDIO_EQ = 0;                    //
    codec->DAP_SGTL_SURROUND = 0x0040;          // Mid width perception change
    codec->DAP_FILTER_COEF_ACCESS = 0;          //
    codec->DAP_COEF_WR_B0_MSB = 0;              //
    codec->DAP_COEF_WR_B0_LSB = 0;              //
    codec->DAP_AUDIO_EQ_BASS_BAND0 = 0x002F;    // Bass/GEG Band 0 to 0dB
    codec->DAP_AUDIO_EQ_BAND1 = 0x002F;         // GEQ Band1 to 0dB
    codec->DAP_AUDIO_EQ_BAND2 = 0x002F;         // GEQ Band2 to 0dB
    codec->DAP_AUDIO_EQ_BAND3 = 0x002F;         // GEQ Band3 to 0dB
    codec->DAP_AUDIO_EQ_TREBLE_BAND4 = 0x002F;  // Tone / Treble GEQ4 to 0dB
    codec->DAP_MAIN_CHAN = 0x8000;              // DAP main channel volume = 100%
    codec->DAP_MIX_CHAN = 0;                    // DAP Mix channel volume = 100%
    codec->DAP_AVC_CTRL = 0x5100;               // AVC 6dB gain, LBI 25ms response
    codec->DAP_AVC_THRESHOLD = 0x1473;          // Threshold set to -12dB
    codec->DAP_AVC_ATTACK = 0x0028;             // Attack rate = 32dB/s
    codec->DAP_AVC_DECAY = 0x0050;              // Decay rate = 4dB/s
    codec->DAP_COEF_WR_B1_MSB = 0;              //
    codec->DAP_COEF_WR_B1_LSB = 0;              //
    codec->DAP_COEF_WR_B2_MSB = 0;              //
    codec->DAP_COEF_WR_B2_LSB = 0;              //
    codec->DAP_COEF_WR_A1_MSB = 0;              //
    codec->DAP_COEF_WR_A1_LSB = 0;              //
    codec->DAP_COEF_WR_A2_MSB = 0;              //
    codec->DAP_COEF_WR_A2_LSB = 0;              //
    codec->dap_enable = 0;                      // DAP disabled by default
    
    // Following the datasheet recommendations for initialization sequence   
    // CODEC power supply configuration, hardware implementation-dependent
    codec->CHIP_ANA_POWER = CODEC_spi_write(codec, CODEC_CHIP_ANA_POWER, 0x4060);        // Turnoff startup power supplies to save power
    __delay_ms(100);
    codec->CHIP_LINREG_CTRL = CODEC_spi_write(codec, CODEC_CHIP_LINREG_CTRL, 0x0060);    // Configure charge pump to use the VDDIO rail
    codec->CHIP_REF_CTRL = CODEC_spi_write(codec, CODEC_CHIP_REF_CTRL, 0x01F0);          // VAG -> 1.575V, BIAS Nominal, Normal VAG ramp
    codec->CHIP_LINE_OUT_CTRL = CODEC_spi_write(codec, CODEC_CHIP_LINE_OUT_CTRL, 0x031F);// LineOut bias -> 360uA, LineOut VAG -> 1.575V
    codec->CHIP_SHORT_CTRL = CODEC_spi_write(codec, CODEC_CHIP_SHORT_CTRL, 0x4446);      // Enable HP short detect, set trip to 125mA
    codec->CHIP_ANA_CTRL = CODEC_spi_write(codec, CODEC_CHIP_ANA_CTRL, 0x0137);          // Enable Zero-cross detection SEE IF USEFUL OR NOT *****
    codec->CHIP_ANA_POWER = CODEC_spi_write(codec, CODEC_CHIP_ANA_POWER, 0x40FF);        // Power up LineOUt, HP & capless mode, ADC, DAC
    codec->CHIP_DIG_POWER = CODEC_spi_write(codec, CODEC_CHIP_DIG_POWER, 0x0073);        // Power UP I2S, DAP, DAC and ADC
    
    // CODEC clock configuration, hardware implementation-dependent
    // PLL output frequency is based on the sample clock rate used
    // The on-board oscillator for the SGTL5000 has a frequency of 12MHz
    if (sys_fs == SYS_FS_44_1kHz)
    {
        pll_out_freq = 180633600;
    }    
    else
    {
        pll_out_freq = 196608000;
    }
    pll_int_divisor = (uint16_t)(pll_out_freq / CODEC_SYS_MCLK);
    pll_frac_divisor = (uint16_t)(((pll_out_freq / CODEC_SYS_MCLK)-pll_int_divisor)*2048);
    
    // CODEC PLL configuration    
    // Write PLL dividers to CODEC
    codec->CHIP_PLL_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_PLL_CTRL, codec->CHIP_PLL_CTRL, 0x07FF, pll_int_divisor<<11);
    codec->CHIP_PLL_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_PLL_CTRL, codec->CHIP_PLL_CTRL, 0xF800, pll_frac_divisor);
    
    // Power-up the PLL
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFBFF, 1<<10);
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFEFF, 1<<8);
    __delay_us(100);    // PLL power up delay
    
    // CODEC System MCLK and Sample Clock -> PLL must be powered before executing these calls
    switch (sys_fs)
    {
        case SYS_FS_8kHz:       // 32kHz / 4 = 8kHz
            // Set internal sample rate to 32kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 0<<2);
            // Divide the internal sample rate by 4
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 2<<4);
            break;
            
        case SYS_FS_11_025kHz:  // 44.100kHz / 4 = 11.025kHz
            // Set internal sample rate to 44.1kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 1<<2);  
            // Divide the internal sample rate by 4
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 2<<4);
            break;
            
        case SYS_FS_12kHz:      // 48kHz / 4 = 12kHz
            // Set internal sample rate to 48kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 2<<2);  
            // Divide the internal sample rate by 4
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 2<<4);
            break;
            
        case SYS_FS_16kHz:      // 32kHz / 2 = 16kHz
            // Set internal sample rate to 32kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 0<<2);  
            // Divide the internal sample rate by 2
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 1<<4);            
            break;
            
        case SYS_FS_22_05kHz:   // 44.1kHz / 2 = 22.05kHz
            // Set internal sample rate to 44.1kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 1<<2);  
            // Divide the internal sample rate by 2
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 1<<4);
            break;
            
        case SYS_FS_24kHz:      // 48kHz / 2 = 24kHz
            // Set internal sample rate to 48kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 2<<2);  
            // Divide the internal sample rate by 2
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 1<<4);
            break;
            
        case SYS_FS_32kHz:      // 32kHz = SYS_FS
            // Set internal sample rate to 32kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 0<<2);  
            // SYS_FS specifies the rate
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 0<<4);
            break;
            
        case SYS_FS_44_1kHz:    // 44.1kHz = SYS_FS 
            // Set internal sample rate to 44.1kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 1<<2);  
            // SYS_FS specifies the rate
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 0<<4);
            break;
            
        case SYS_FS_48kHz:      // 48kHz = SYS_FS  
            // Set internal sample rate to 48kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 2<<2);  
            // SYS_FS specifies the rate
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 0<<4);
            break;
            
        case SYS_FS_96kHz:      // 96kHz = SYS_FS  
            // Set internal sample rate to 96kHz
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFF3, 3<<2);  
            // SYS_FS specifies the rate
            codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFCF, 0<<4);
            break;
            
        default:
            // Will use the POR value for the SGTL5000 sample rate
            break;
    }
    // Identify the incoming SYS_MCLK frequency to use PLL (always)
    codec->CHIP_CLK_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_CLK_CTRL, codec->CHIP_CLK_CTRL, 0xFFFC, 3 << 0);
    
    // CODEC data channel configuration, hardware implementation-dependent
    // Set CODEC to I2S master, set data length to 16 bits, 32Fs
    codec->CHIP_I2S_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_I2S_CTRL, codec->CHIP_I2S_CTRL, 0xFEFF, 1 << 8);    // 32 Fs (DCI uses 16-bit per data)
    codec->CHIP_I2S_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_I2S_CTRL, codec->CHIP_I2S_CTRL, 0xFF7F, 1 << 7);    // CODEC is master
    codec->CHIP_I2S_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_I2S_CTRL, codec->CHIP_I2S_CTRL, 0xFFCF, 3 << 4);    // DLEN = 16 bits

    CODEC_set_adc_stereo(codec);
    CODEC_set_dac_stereo(codec);

    // CODEC default input / output route
    CODEC_set_audio_path(codec, CODEC_INPUT_MIC, CODEC_OUTPUT_I2S, CODEC_DAP_DISABLE);
    CODEC_set_audio_path(codec, CODEC_INPUT_I2S, CODEC_OUTPUT_HP_ADC, CODEC_DAP_DISABLE);
    
    CODEC_mic_config(codec, MIC_BIAS_RES_2k, MIC_BIAS_VOLT_2V00, MIC_GAIN_0dB);
    CODEC_set_hp_volume(codec, 0x18, 0x18);  
    
    // Enable DCI tx and RX 
    DCI_set_transmit_state(codec, DCI_TRANSMIT_ENABLE);
    DCI_set_receive_state(codec, DCI_RECEIVE_ENABLE);
    
#ifdef DCI0_DMA_ENABLE   
    // Force first DMA transfers to TXBUF0 and TXBUF1
    DMA_force_txfer(codec->DMA_tx_channel);
    while(DMA_get_force_state(codec->DMA_tx_channel) == 1);
    DMA_force_txfer(codec->DMA_tx_channel);
    while(DMA_get_force_state(codec->DMA_tx_channel) == 1);   
#endif 
    
    DCI_enable(codec);
    // Unmute all channels
    CODEC_unmute(codec, ADC_MUTE);
    CODEC_unmute(codec, DAC_MUTE);
    CODEC_unmute(codec, HEADPHONE_MUTE);
    CODEC_unmute(codec, LINEOUT_MUTE);
}

uint16_t CODEC_spi_write (STRUCT_CODEC *codec, uint16_t adr, uint16_t data) 
{    
    // Blocking SPI call
    while (SPI_module_busy(codec->spi_ref) != SPI_MODULE_FREE);   
    uint8_t buf[4] = {((adr & 0xFF00)>>8), adr, ((data & 0xFF00)>>8), data};
    SPI_load_tx_buffer(codec->spi_ref, buf, 4);
    SPI_write(codec->spi_ref, AUDIO_CODEC_CS);
    // End of SPI transaction
    return data;
}

uint16_t CODEC_spi_modify_write (STRUCT_CODEC *codec, uint16_t adr, uint16_t reg, uint16_t mask, uint16_t data)
{
    reg &= mask;
    reg |= data;
    uint8_t buf[4] = {((adr & 0xFF00)>>8), (adr&0x00FF), ((reg & 0xFF00)>>8), (reg&0x00FF)};
    
    // Blocking SPI call
    while (SPI_module_busy(codec->spi_ref) != SPI_MODULE_FREE);
    SPI_load_tx_buffer(codec->spi_ref, buf, 4);
    SPI_write(codec->spi_ref, AUDIO_CODEC_CS); 
    // End of SPI transaction
    return reg;
}

void CODEC_mic_config (STRUCT_CODEC *codec, uint8_t bias_res, uint8_t bias_volt, uint8_t gain)
{
    // Set microphone bias impedance
    codec->CHIP_MIC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_MIC_CTRL, codec->CHIP_MIC_CTRL, 0xFCFF, bias_res << 8);
    // Set microphone bias voltage
    codec->CHIP_MIC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_MIC_CTRL, codec->CHIP_MIC_CTRL, 0xFF8F, bias_volt << 4);
    // Set microphone gain
    CODEC_set_mic_gain(codec, gain);  
}

void CODEC_set_dac_mono (STRUCT_CODEC *codec)
{
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xBFFF, 0<<14);  // Set DAC to MONO     
}

void CODEC_set_dac_stereo (STRUCT_CODEC *codec)
{
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xBFFF, 1<<14);  // Set DAC to STEREO    
}

void CODEC_set_adc_mono (STRUCT_CODEC *codec)
{
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFFBF, 0<<6);   // Set ADC to MONO      
}

void CODEC_set_adc_stereo (STRUCT_CODEC *codec)
{
    codec->CHIP_ANA_POWER = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_POWER, codec->CHIP_ANA_POWER, 0xFFBF, 1<<6);   // Set ADC to STEREO          
}

void CODEC_set_audio_path (STRUCT_CODEC *codec, uint8_t in_channel, uint8_t out_channel, uint8_t dap_enable)
{    
    codec->dap_enable = dap_enable;
    
    // Input / output routes are defined in SGTL5000 datasheet
    if (in_channel == CODEC_INPUT_LINE)
    {    
        if (out_channel == CODEC_OUTPUT_HP_BYP)
        {
            // Direct bypass of CODEC, LineIn -> HP out
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFBF, 1 << 6);   // LineIn feeds HP out
        }
        
        if (out_channel == CODEC_OUTPUT_HP_ADC)
        {
            // LineIn feeds the ADC
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 1 << 2); 
            
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1) 
            {
                // ADC feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 0 << 6);
                // DAP feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 3 << 4);
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // ADC feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   
            }
            // DAC feeds the HP out             
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFBF, 0 << 6);          
        }        
        
        if (out_channel == CODEC_OUTPUT_I2S)
        {
            // LineIn feeds the ADC
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 1 << 2);   
            
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1)
            {
                // ADC feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 0 << 6);
                // DAP feeds I2S_OUT
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 3 << 0);                
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // ADC feeds I2S_OUT
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 0 << 0);   
            }
        }
        
        if (out_channel == CODEC_OUTPUT_LINE)
        {
            // LineIn feeds the ADC
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 1 << 2); 
            
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1) 
            {
                // ADC feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 0 << 6);
                // DAP feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 3 << 4);
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // ADC feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   
            }             
        }
    }
    
    if (in_channel == CODEC_INPUT_MIC)
    {    
        if (out_channel == CODEC_OUTPUT_HP_ADC)
        { 
            // MicIn feeds the ADC
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 0 << 2);  
            
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1) 
            {
                // ADC feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 0 << 6);
                // DAP feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 3 << 4);
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // ADC feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   
            }
            // DAC feeds the HP out             
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFBF, 0 << 6);        
        }
        
        if (out_channel == CODEC_OUTPUT_I2S)
        {  
            // MicIn feeds the ADC
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 0 << 2);              
            
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1)
            {
                // ADC feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 0 << 6);
                // DAP feeds I2S_OUT
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 3 << 0);                
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // ADC feeds I2S_OUT
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 0 << 0);   
            }
        } 
        
        if (out_channel == CODEC_OUTPUT_LINE)
        {
            // MicIn feeds the ADC
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFB, 0 << 2);  
            
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1) 
            {
                // ADC feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 0 << 6);
                // DAP feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 3 << 4);
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // ADC feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 0 << 4);   
            }             
        }        
    }
    
    if (in_channel == CODEC_INPUT_I2S)
    {
        if (out_channel == CODEC_OUTPUT_HP_ADC)
        {           
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1) 
            {
                // I2S_IN feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 1 << 6);
                // DAP feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 3 << 4);
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // I2S_IN feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 1 << 4);  
            }
            // DAC feeds the HP out             
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFBF, 0 << 6);                   
        }
        
        if (out_channel == CODEC_OUTPUT_I2S)
        {              
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1)
            {
                // I2S_IN feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 1 << 6);
                // DAP feeds I2S_OUT
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 3 << 0);                
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // I2S_IN feeds I2S_OUT
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFFC, 1 << 0);   
            }
        } 
        
        if (out_channel == CODEC_OUTPUT_LINE)
        {
            // SGTL5000 digital audio processor submodule is enabled
            if (codec->dap_enable == 1) 
            {
                // I2S_IN feeds DAP
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFF3F, 1 << 6);
                // DAP feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 3 << 4);  
            }
            
            // SGTL5000 digital audio processor submodule is disabled
            else
            {
                // I2S_IN feeds DAC
                codec->CHIP_SSS_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_SSS_CTRL, codec->CHIP_SSS_CTRL, 0xFFCF, 1 << 4);   
            }             
        }         
    }
}

// CODEC control on-chip DAC volume. DAC can only be attenuated, not amplified
// Min is 0dB, max is -90dB, in 0.5dB step
// 0 <= dac_vol_L/R <= 180, where 180 * 0.5 = -90dB 
// Minimum value written to register is 0x3C/3C. Codes lower than 0x3C/3C are reserved
// Maximum value written to register is 0xF0/F0. Codes higher than 0xF0/F0 = channel mute
void CODEC_set_dac_volume (STRUCT_CODEC *codec, uint8_t dac_vol_right, uint8_t dac_vol_left)
{
    if (dac_vol_right > 180){dac_vol_right = 180;}  // Saturate right channel
    if (dac_vol_left > 180){dac_vol_left = 180;}    // Saturate left channel
    codec->dac_vol_right = dac_vol_right + 0x3C;    // 0x3C is 0dB base offset in SGTL5000       
    codec->dac_vol_left = dac_vol_left + 0x3C;      // 0x3C is 0dB base offset in SGTL5000            
    codec->CHIP_DAC_VOL = CODEC_spi_write(codec, CODEC_CHIP_DAC_VOL, ((codec->dac_vol_right << 8) | codec->dac_vol_left)); 
}

// CODEC control on-chip HP volume. HP can be attenuated and amplified
// Min is -51.5dB, max is +12dB, in 0.5dB step
// 0 <= hp_vol_L/R <= 127, where 0 = +12dB and 127 = -51.5dB 
// Maximum value written to register is 0x7F/7F. Codes higher than 0x7F/7F = reserved
void CODEC_set_hp_volume (STRUCT_CODEC *codec, uint8_t hp_vol_right, uint8_t hp_vol_left)
{
    if (hp_vol_right > 0x7F){hp_vol_right = 0x7F;}  // Saturate right channel
    if (hp_vol_left > 0x7F){hp_vol_left = 0x7F;}    // Saturate left channel
    
    codec->hp_vol_right = hp_vol_right;            
    codec->hp_vol_left = hp_vol_left;       
    codec->CHIP_ANA_HP_CTRL = CODEC_spi_write(codec, CODEC_CHIP_ANA_HP_CTRL, ((codec->hp_vol_right << 8) | codec->hp_vol_left));     
}

// CODEC control on-chip LineOut volume. LineOut can be attenuated and amplified
// Min is -7.5dB, max is +7.5dB, in 0.5dB step
// 0 <= lo_vol_right/R <= 31, where 0 = +7.5dB and 31 = -7.5dB
// Maximum value written to register is 0x1F/1F. Codes higher than 0x1F/1F = reserved
// Based on hardware implementation, VDDA = 3V3, VAG_VAL = 1.575, VDDIO = 3.3, LO_VAGCNTRL = 1.575
// Nominal value is 40*log((VAG_VAL)/(LO_VAGCNTRL))+15 = 0xF (L + R channel)
// Maximum value is 0x1F (32). Codes higher than nominal have more attenuation
// Each additional step is either +-0.5dB attenuation or gain
// For more information see SGTL5000 datasheet p.41
void CODEC_set_lo_volume (STRUCT_CODEC *codec, uint8_t lo_vol_right, uint8_t lo_vol_left)
{
    if (lo_vol_right > 0x1F){lo_vol_right = 0x1F;}  // Saturate right channel
    if (lo_vol_left > 0x1F){lo_vol_left = 0x1F;}    // Saturate left channel
    codec->lo_vol_right = lo_vol_right;
    codec->lo_vol_left = lo_vol_left;
    codec->CHIP_LINE_OUT_VOL = CODEC_spi_write(codec, CODEC_CHIP_LINE_OUT_VOL, ((codec->lo_vol_right << 8) | codec->lo_vol_left));     
}

// CODEC control on-chip ADC volume. ADC can be attenuated and / or amplified
// Output gain and / or attenuation is controlled by the range bit
// With range = 0, no change in ADC range
// Min is 0dB, max is +22.5dB, in 1.5dB step
// 0 <= adc_vol_L/R <= 15, where 0 = +0dB and 15 = +22.5dB 
//
// With range = 1, ADC range reduced by 6dB
// Min is -6dB, max is +16.5dB, in 1.5dB step
// 0 <= adc_vol_L/R <= 15, where 0 = -6dB and 15 = +16.5dB 
// Maximum value written to register is 0x. Codes higher than 0x7F/7F = reserved
void CODEC_set_adc_volume (STRUCT_CODEC *codec, uint8_t range, uint8_t adc_vol_right, uint8_t adc_vol_left)
{
    // If range == 0, gain range from 0 to 22.5dB in step of 1.5dB, from 0x0 to 0xF
    // If range == 1, gain range from -6 to 16.5dB in step of 1.5dB, from 0x0 to 0xF
    if (adc_vol_right > 0x0F){adc_vol_right = 0x0F;}
    if (adc_vol_left > 0x0F){adc_vol_left = 0x0F;}
    codec->adc_vol_right = adc_vol_right;
    codec->lo_vol_left = adc_vol_left;
    codec->CHIP_ANA_ADC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_ADC_CTRL, codec->CHIP_ANA_ADC_CTRL, (0x00FF | (range << 8)), ((codec->adc_vol_right << 4) | codec->lo_vol_left));
}

void CODEC_set_mic_gain (STRUCT_CODEC *codec, uint8_t gain)
{
    // Set microphone gain
    codec->CHIP_MIC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_MIC_CTRL, codec->CHIP_MIC_CTRL, 0xFFFC, gain << 0);     
}

void CODEC_mute (STRUCT_CODEC *codec, uint8_t channel)
{
    switch(channel)
    {
        case ADC_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFE, 1 << 0);
            break;
            
        case HEADPHONE_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFEF, 1 << 4);
            break;
            
        case LINEOUT_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFEFF, 1 << 8);
            break;
             
        case DAC_MUTE:
             codec->CHIP_ADCDAC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ADCDAC_CTRL, codec->CHIP_ADCDAC_CTRL, 0xFFF3, 3 << 2);
            break;
            
        default:
            break;
    }
}

void CODEC_unmute (STRUCT_CODEC *codec, uint8_t channel)
{
    switch(channel)
    {
        case ADC_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFFE, 0 << 0);
            break;
            
        case HEADPHONE_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFFEF, 0 << 4);
            break;
            
        case LINEOUT_MUTE:
            codec->CHIP_ANA_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ANA_CTRL, codec->CHIP_ANA_CTRL, 0xFEFF, 0 << 8);
            break;

        case DAC_MUTE:
             codec->CHIP_ADCDAC_CTRL = CODEC_spi_modify_write(codec, CODEC_CHIP_ADCDAC_CTRL, codec->CHIP_ADCDAC_CTRL, 0xFFF3, 0 << 2);
            break;
            
        default:
            break;
    }   
}

uint8_t DCI_fill_dma_tx_buf (STRUCT_CODEC *codec, uint16_t *buf, uint16_t length)
{
#ifdef DCI0_DMA_ENABLE
    uint16_t i=0;
    if (length > codec->DCI_transmit_length){length = codec->DCI_transmit_length;}
    
    for (; i<length; i++)
    {        
        if (codec->DMA_tx_buf_pp == 1)
        {
            codec_dma_tx_buf_B[i] = *buf++;
        }
        else
        {
            codec_dma_tx_buf_A[i] = *buf++;
        }
    }
    return 1;
#endif
#ifndef DCI0_DMA_ENABLE
    return 0;
#endif
}

uint16_t * DCI_unload_dma_rx_buf (STRUCT_CODEC *codec, uint16_t length)
{
#ifdef DCI0_DMA_ENABLE
    uint16_t i = 0;
    if (length > codec->DCI_receive_length){length = codec->DCI_receive_length;}

    if (codec->DMA_rx_buf_pp == 1) 
    {
        for (; i<length; i++)
        {
            codec->DCI_receive_buffer[i] = codec_dma_rx_buf_A[i];
        }
    }
    else
    {
        for (; i<length; i++)
        {
            codec->DCI_receive_buffer[i] = codec_dma_rx_buf_B[i];
        }        
    }  
    return &codec->DCI_receive_buffer[0];
#endif
#ifndef DCI0_DMA_ENABLE
    return 0;
#endif
}

void DCI_set_transmit_state (STRUCT_CODEC *codec, uint8_t state)
{
    codec->DCI_transmit_enable = state;
}

void DCI_set_receive_state (STRUCT_CODEC *codec, uint8_t state)
{
    codec->DCI_receive_enable = state;
}

uint8_t DCI_get_pp_buffer_state (STRUCT_CODEC *codec, uint8_t tx_rx)
{
    if (tx_rx == DCI_DMA_TX)
    {
        return codec->DMA_tx_buf_pp;
    }
    else
    {
        return codec->DMA_rx_buf_pp;
    }
}

void DCI_enable (STRUCT_CODEC *codec)
{
    codec->DCI_enable_state = 1;
#ifndef DCI0_DMA_ENABLE
    IEC3bits.DCIIE = 1;     // Enable DCI interrupt (only when DMA not enabled)
    IPC15bits.DCIIP = 4;    // Make the DCI interrupt higher priority than nominal      
#endif
    DCICON1bits.DCIEN = 1;  // Enable DCI module    
}

void DCI_disable (STRUCT_CODEC *codec)
{
    codec->DCI_enable_state = 0;
#ifndef DCI0_DMA_ENABLE
    IEC3bits.DCIIE = 0;     // Disable DCI interrupt  
    IFS3bits.DCIIF = 0;     // Clear DCI interrupt flag       
#endif
    DCICON1bits.DCIEN = 0;  // Disable DCI module  
}

uint8_t DCI_get_interrupt_state (STRUCT_CODEC *codec, uint8_t tx_rx)
{
#ifndef DCI0_DMA_ENABLE
    if (codec->interrupt_flag == 1)
    {
        codec->interrupt_flag = 0;
        return 1;
    }
    else 
        return 0;
#endif
#ifdef DCI0_DMA_ENABLE
    if (tx_rx == DCI_DMA_RX)
    {
        if (codec->DCI_receive_enable == DCI_RECEIVE_ENABLE)
        {
            if (DMA_get_txfer_state(codec->DMA_rx_channel) == DMA_TXFER_DONE)
            {                               
                return 1;
            }
            else
                return 0;
        }
        else 
            return 0;
    }
    
    else if (tx_rx == DCI_DMA_TX)
    {
        if(codec->DCI_transmit_enable == DCI_TRANSMIT_ENABLE)
        {
            if (DMA_get_txfer_state(codec->DMA_tx_channel) == DMA_TXFER_DONE)
            {
                return 1;
            }
            else
                return 0;
        }
        else
            return 0;
    }
    else
        return 0;
#endif
}

void __attribute__((__interrupt__, no_auto_psv)) _DCIInterrupt(void)
{
    IFS3bits.DCIIF = 0;      // clear DCI interrupt flag
    // Without DMA
#ifndef DCI0_DMA_ENABLE
    // I2S direct loopback (I2Sin -> I2Sout)
    TXBUF0 = RXBUF0;
    TXBUF1 = RXBUF1;     
#endif
    CODEC_struct[DCI_0].interrupt_flag = 1;
}