//***************************************************************************//
// File      : codec.h
//
// About     : dsPeak features an on-board audio in/out codec based on NXPs
//             SGTL5000 integrated circuit. It has an audio in line, a micro in,
//             an audio out and a speaker out. In addition, this codec has an
//             integrated equalizer / bass-tone enhancer and many other features
//             such as programmable mic gain / volume control, filtering and
//             automatic volume control.
//
// Functions : 
//
// Includes  : dspeak_generic.h
//           
// Purpose   : Driver for dsPeak's SGTL5000 audio codec using DCI as audio
//             interface and SPI as configuration interface 
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
//****************************************************************************//

#ifndef __CODEC_H_
#define	__CODEC_H_

dspic_33ck_generic
#include "spi.h"

#define CODEC_QTY           1
#define DCI_0               0

// To use DCI module with DMA transfers (both transmit and receive), uncomment
#define DCI0_DMA_ENABLE

// SGTL5000 CODEC registers definition -----------------------------------------
#define CODEC_CHIP_ID                   0x0000
#define CODEC_CHIP_DIG_POWER            0x0002
#define CODEC_CHIP_CLK_CTRL             0x0004
#define CODEC_CHIP_I2S_CTRL             0x0006
#define CODEC_CHIP_SSS_CTRL             0x000A
#define CODEC_CHIP_ADCDAC_CTRL          0x000E
#define CODEC_CHIP_DAC_VOL              0x0010
#define CODEC_CHIP_PAD_STRENGTH         0x0014
#define CODEC_CHIP_ANA_ADC_CTRL         0x0020
#define CODEC_CHIP_ANA_HP_CTRL          0x0022
#define CODEC_CHIP_ANA_CTRL             0x0024
#define CODEC_CHIP_LINREG_CTRL          0x0026
#define CODEC_CHIP_REF_CTRL             0x0028
#define CODEC_CHIP_MIC_CTRL             0x002A
#define CODEC_CHIP_LINE_OUT_CTRL        0x002C
#define CODEC_CHIP_LINE_OUT_VOL         0x002E
#define CODEC_CHIP_ANA_POWER            0x0030
#define CODEC_CHIP_PLL_CTRL             0x0032
#define CODEC_CHIP_CLK_TOP_CTRL         0x0034
#define CODEC_CHIP_ANA_STATUS           0x0036
#define CODEC_CHIP_ANA_TEST1            0x0038
#define CODEC_CHIP_ANA_TEST2            0x003A
#define CODEC_CHIP_SHORT_CTRL           0x003C
#define CODEC_DAP_CONTROL               0x0100
#define CODEC_DAP_PEQ                   0x0102
#define CODEC_DAP_BASS_ENHANCE          0x0104
#define CODEC_DAP_BASS_ENHANCE_CTRL     0x0106
#define CODEC_DAP_AUDIO_EQ              0x0108
#define CODEC_DAP_SGTL_SURROUND         0x010A
#define CODEC_DAP_FILTER_COEF_ACCESS    0x010C
#define CODEC_DAP_COEF_WR_B0_MSB        0x010E
#define CODEC_DAP_COEF_WR_B0_LSB        0x0110
#define CODEC_DAP_AUDIO_EQ_BASS_BAND0   0x0116
#define CODEC_DAP_AUDIO_EQ_BAND1        0x0118
#define CODEC_DAP_AUDIO_EQ_BAND2        0x011A
#define CODEC_DAP_AUDIO_EQ_BAND3        0x011C
#define CODEC_DAP_AUDIO_EQ_TREBLE_BAND4 0x011E
#define CODEC_DAP_MAIN_CHAN             0x0120
#define CODEC_DAP_MIX_CHAN              0x0122
#define CODEC_DAP_AVC_CTRL              0x0124
#define CODEC_DAP_AVC_THRESHOLD         0x0126
#define CODEC_DAP_AVC_ATTACK            0x0128
#define CODEC_DAP_AVC_DECAY             0x012A
#define CODEC_DAP_COEF_WR_B1_MSB        0x012C
#define CODEC_DAP_COEF_WR_B1_LSB        0x012E
#define CODEC_DAP_COEF_WR_B2_MSB        0x0130
#define CODEC_DAP_COEF_WR_B2_LSB        0x0132
#define CODEC_DAP_COEF_WR_A1_MSB        0x0134
#define CODEC_DAP_COEF_WR_A1_LSB        0x0136
#define CODEC_DAP_COEF_WR_A2_MSB        0x0138
#define CODEC_DAP_COEF_WR_A2_LSB        0x013A

//------------------------------------------------------------------------------
// SGTL5000 Microphone specific defines ****************************************
#define MIC_BIAS_RES_OFF    0   // Microphone bias output impedance, match with     
#define MIC_BIAS_RES_2k     1   // microphone output impedance
#define MIC_BIAS_RES_4k     2   //
#define MIC_BIAS_RES_8k     3   //

#define MIC_BIAS_VOLT_1V25  0   // Microphone bias voltage, should be under
#define MIC_BIAS_VOLT_1V50  1   // VDDA - 0.2V for adequate PSRR
#define MIC_BIAS_VOLT_1V75  2   //
#define MIC_BIAS_VOLT_2V00  3   //
#define MIC_BIAS_VOLT_2V25  4   //
#define MIC_BIAS_VOLT_2V50  5   //
#define MIC_BIAS_VOLT_2V75  6   //
#define MIC_BIAS_VOLT_3V00  7   //

#define MIC_GAIN_0dB        0   // Microphone amplifier gain, setting a gain 
#define MIC_GAIN_20dB       1   // different than 0dB decreases THD
#define MIC_GAIN_30dB       2   //
#define MIC_GAIN_40dB       3   //
//******************************************************************************
// SGTL5000 input and output routes &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#define CODEC_INPUT_LINE        0
#define CODEC_INPUT_MIC         1 
#define CODEC_INPUT_I2S         2

// SGTL5000 output routes
#define CODEC_OUTPUT_HP_BYP     0
#define CODEC_OUTPUT_HP_ADC     1
#define CODEC_OUTPUT_LINE       2
#define CODEC_OUTPUT_I2S        3
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// CODEC volume defines
#define ADC_VOL_RANGE_DEFAULT   0
#define ADC_VOL_RANGE_RED6DB    1

// CODEC FS frequencies, derive PLL clock based on this value
#define SYS_FS_32kHz        0
#define SYS_FS_44_1kHz      1
#define SYS_FS_48kHz        2
#define SYS_FS_96kHz        3
#define SYS_FS_8kHz         4
#define SYS_FS_11_025kHz    5
#define SYS_FS_12kHz        6
#define SYS_FS_16kHz        7
#define SYS_FS_22_05kHz     8
#define SYS_FS_24kHz        9

#define CODEC_SYS_MCLK      12000000    // SYS_MCLK is 12MHz

#define ADC_MUTE            0
#define HEADPHONE_MUTE      1
#define LINEOUT_MUTE        2
#define DAC_MUTE            3

#define CODEC_DAP_ENABLE          1
#define CODEC_DAP_DISABLE         0

#define DCI_DMA_TX              0
#define DCI_DMA_RX              1
#define DCI_TRANSMIT_ENABLE     1
#define DCI_TRANSMIT_DISABLE    0
#define DCI_RECEIVE_ENABLE      1
#define DCI_RECEIVE_DISABLE     0

#define DCI_RECEIVE_COMPLETE    1
#define DCI_TRANSMIT_COMPLETE   1

// I2S BLOCK TRANSFER define
// 1x DMA transfer = 1x data word (1x sample)
// 128 word transfer = 256 byte transfer
#define CODEC_BLOCK_TRANSFER 128    

typedef struct
{
    // SGTL5000 registers
    uint16_t CHIP_DIG_POWER;
    uint16_t CHIP_CLK_CTRL;
    uint16_t CHIP_I2S_CTRL;
    uint16_t CHIP_SSS_CTRL;
    uint16_t CHIP_ADCDAC_CTRL;
    uint16_t CHIP_DAC_VOL;
    uint16_t CHIP_PAD_STRENGTH;
    uint16_t CHIP_ANA_ADC_CTRL;
    uint16_t CHIP_ANA_HP_CTRL;
    uint16_t CHIP_ANA_CTRL;
    uint16_t CHIP_LINREG_CTRL;
    uint16_t CHIP_REF_CTRL;
    uint16_t CHIP_MIC_CTRL;
    uint16_t CHIP_LINE_OUT_CTRL;
    uint16_t CHIP_LINE_OUT_VOL;
    uint16_t CHIP_ANA_POWER;
    uint16_t CHIP_PLL_CTRL;
    uint16_t CHIP_CLK_TOP_CTRL;
    uint16_t CHIP_ANA_STATUS;
    uint16_t CHIP_ANA_TEST1;
    uint16_t CHIP_ANA_TEST2;
    uint16_t CHIP_SHORT_CTRL;
    uint16_t DAP_CONTROL;
    uint16_t DAP_PEQ;
    uint16_t DAP_BASS_ENHANCE;
    uint16_t DAP_BASS_ENHANCE_CTRL;
    uint16_t DAP_AUDIO_EQ;
    uint16_t DAP_SGTL_SURROUND;
    uint16_t DAP_FILTER_COEF_ACCESS;
    uint16_t DAP_COEF_WR_B0_MSB;
    uint16_t DAP_COEF_WR_B0_LSB;
    uint16_t DAP_AUDIO_EQ_BASS_BAND0;
    uint16_t DAP_AUDIO_EQ_BAND1;
    uint16_t DAP_AUDIO_EQ_BAND2;
    uint16_t DAP_AUDIO_EQ_BAND3;
    uint16_t DAP_AUDIO_EQ_TREBLE_BAND4;
    uint16_t DAP_MAIN_CHAN;
    uint16_t DAP_MIX_CHAN;
    uint16_t DAP_AVC_CTRL;
    uint16_t DAP_AVC_THRESHOLD;
    uint16_t DAP_AVC_ATTACK;
    uint16_t DAP_AVC_DECAY;
    uint16_t DAP_COEF_WR_B1_MSB;
    uint16_t DAP_COEF_WR_B1_LSB;
    uint16_t DAP_COEF_WR_B2_MSB;
    uint16_t DAP_COEF_WR_B2_LSB;
    uint16_t DAP_COEF_WR_A1_MSB;
    uint16_t DAP_COEF_WR_A1_LSB;
    uint16_t DAP_COEF_WR_A2_MSB;
    uint16_t DAP_COEF_WR_A2_LSB;
    
    // Volume variables
    uint8_t dac_vol_left;
    uint8_t dac_vol_right;
    uint8_t hp_vol_left;
    uint8_t hp_vol_right;
    uint8_t lo_vol_left;
    uint8_t lo_vol_right;
    uint8_t adc_vol_left;
    uint8_t adc_vol_right;
    
    // DAP variables
    uint8_t dap_enable;
    
    // DCI variables
    uint8_t DCI_enable_state;
    uint16_t DCI_receive_buffer[CODEC_BLOCK_TRANSFER];
    uint16_t DCI_transmit_buffer[CODEC_BLOCK_TRANSFER];
    uint16_t DCI_transmit_counter;
    uint16_t DCI_receive_counter;
    uint16_t DCI_transmit_length;
    uint16_t DCI_receive_length;
    uint8_t DCI_transmit_enable;
    uint8_t DCI_receive_enable;
    uint8_t interrupt_flag;
    
    // Reference to an SPI structure used to communicate with CODEC
    STRUCT_SPI *spi_ref;
    
    // Hardware-related variables
    uint8_t SPI_channel;    // Specifies SPI module used (SPI_[x], x = 1..2..3..4)
    uint8_t DMA_tx_channel; // Specifies DMA tx channel used (0..14)
    uint8_t DMA_rx_channel; // Specifies DMA rx channel used (0..14)
    uint8_t DMA_tx_buf_pp;  // ping-pong variable (indicates if buffer A or B is in use)
    uint8_t DMA_rx_buf_pp;  // ping-pong variable (indicates if buffer A or B is in use)
}STRUCT_CODEC;

// CODEC basic functions
void CODEC_init (STRUCT_CODEC *codec, STRUCT_SPI *spi, uint8_t spi_channel, uint8_t sys_fs, 
                uint16_t tx_buf_length, uint16_t rx_buf_length, uint8_t DMA_tx_channel, uint8_t DMA_rx_channel);
uint16_t CODEC_spi_write (STRUCT_CODEC *codec, uint16_t adr, uint16_t data);
uint16_t CODEC_spi_modify_write (STRUCT_CODEC *codec, uint16_t adr, uint16_t reg, uint16_t mask, uint16_t data);

// CODEC peripheral configuration
void CODEC_mic_config (STRUCT_CODEC *codec, uint8_t bias_res, uint8_t bias_volt, uint8_t gain);
void CODEC_set_audio_path (STRUCT_CODEC *codec, uint8_t in_channel, uint8_t out_channel, uint8_t dap_enable);

// CODEC volume control
void CODEC_mute (STRUCT_CODEC *codec, uint8_t channel);
void CODEC_unmute (STRUCT_CODEC *codec, uint8_t channel);
void CODEC_set_mic_gain (STRUCT_CODEC *codec, uint8_t gain);
void CODEC_set_adc_volume (STRUCT_CODEC *codec, uint8_t range, uint8_t adc_vol_right, uint8_t adc_vol_left);
void CODEC_set_dac_volume (STRUCT_CODEC *codec, uint8_t dac_vol_right, uint8_t dac_vol_left);
void CODEC_set_hp_volume (STRUCT_CODEC *codec, uint8_t hp_vol_right, uint8_t hp_vol_left);
void CODEC_set_lo_volume (STRUCT_CODEC *codec, uint8_t lo_vol_right, uint8_t lo_vol_left);
void CODEC_set_dac_mono (STRUCT_CODEC *codec);
void CODEC_set_dac_stereo (STRUCT_CODEC *codec);
void CODEC_set_adc_mono (STRUCT_CODEC *codec);
void CODEC_set_adc_stereo (STRUCT_CODEC *codec);


// dsPIC33E DCI module functions
void DCI_init (STRUCT_CODEC *codec, uint16_t tx_buf_length, uint16_t rx_buf_length,
                uint8_t DMA_tx_channel, uint8_t DMA_rx_channel);
void DCI_enable (STRUCT_CODEC *codec);
void DCI_disable (STRUCT_CODEC *codec);
uint8_t DCI_get_interrupt_state (STRUCT_CODEC *codec, uint8_t tx_rx);
uint8_t DCI_fill_dma_tx_buf (STRUCT_CODEC *codec, uint16_t *buf, uint16_t length);
uint8_t DCI_get_pp_buffer_state (STRUCT_CODEC *codec, uint8_t tx_rx);
uint16_t * DCI_unload_dma_rx_buf (STRUCT_CODEC *codec, uint16_t length);
void DCI_set_transmit_state (STRUCT_CODEC *codec, uint8_t state);
void DCI_set_receive_state (STRUCT_CODEC *codec, uint8_t state);
#endif	

