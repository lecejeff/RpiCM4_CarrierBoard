//****************************************************************************//
// File      :  DMA.h
//
// Functions :  
//
// Includes  :  dspic_33ck_generic.h
//
// Purpose   :  Driver for the dsPIC33CK DMA engine
//    
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#ifndef __dma_h__
#define __dma_h__

#include "dspic_33ck_generic.h"

//------------------------------- DMA defines --------------------------------//
// Library and peripheral-oriented defines
#define DMA_QTY             8

#define DMA_CH0             0
#define DMA_CH1             1
#define DMA_CH2             2
#define DMA_CH3             3
#define DMA_CH4             4
#define DMA_CH5             5
#define DMA_CH6             6
#define DMA_CH7             7
#define DMA_ALL_INIT        8

#define DMA_MAX_TX_LENGTH   0x4000  // dsPIC33EP512MU814 datasheet, p.167 ((2^14)+1)

#define DMA_ARRAY_SOURCE        0
#define DMA_ARRAY_DESTIN        1

#define DMA_TXFER_IDLE          2
#define DMA_TXFER_DONE          1
#define DMA_TXFER_IN_PROGRESS   0

#define DMA_STATE_DISABLED      3
#define DMA_STATE_ENABLED       2
#define DMA_STATE_ASSIGNED      1
#define DMA_STATE_UNASSIGNED    0

#define DMA_BUF_MODE_PP         0
#define DMA_BUF_MODE_SGL        1

// DMACHx
#define DMA_CH_ENABLE       0x0001
#define DMA_CH_DISABLE      0
#define DMA_SIZE_BYTE       0x0002
#define DMA_SIZE_WORD       0
#define DMA_TRMODE_ONES     0       // One shot
#define DMA_TRMODE_RONES    0x0004  // Repeated one shot
#define DMA_TRMODE_CONT     0x0008  // Continuous
#define DMA_TRMODE_RCONT    0x000C  // Repeated continuous
#define DMA_DADMODE_UNCH    0       // DMADSTn remains unchanged
#define DMA_DADMODE_INCR    0x0010  // DMADSTn increm. based on SIZE 
#define DMA_DADMODE_DECR    0x0020  // DMADSRn decrem. based on SIZE
#define DMA_DADMODE_PIAU    0x0030  // DMADSTn is used in periph. indirect addr
#define DMA_SADMODE_UNCH    0       // DMASRCn remains unchanged
#define DMA_SADMODE_INCR    0x0040  // DMASRCn increm. based on SIZE 
#define DMA_SADMODE_DECR    0x0080  // DMASRCn decrem. based on SIZE
#define DMA_SADMODE_PIAU    0x00C0  // DMASRCn is used in periph. indirect addr
#define DMA_CHREQ           0x0100  // Initiate DMA request
#define DMA_RELOAD          0x0200  
#define DMA_NULLWR          0x0400

//DMAxREQ
#define DMA_TXFER_FORCE     1
#define DMA_TXFER_AUTO      0
//DMAxREQ PERIPHERALS
#define DMAREQ_INT0         0x0
#define DMAREQ_IC1          0x1
#define DMAREQ_IC2          0x5
#define DMAREQ_IC3          0x25
#define DMAREQ_IC4          0x26
#define DMAREQ_OC1          0x2
#define DMAREQ_OC2          0x6
#define DMAREQ_OC3          0x19
#define DMAREQ_OC4          0x1A
#define DMAREQ_TMR2         0x7
#define DMAREQ_TMR3         0x8
#define DMAREQ_TMR4         0x1B
#define DMAREQ_TMR5         0x1C
#define DMAREQ_SPI1         0xA     
#define DMAREQ_SPI2         0x21
#define DMAREQ_SPI3         0x5B
#define DMAREQ_SPI4         0x7B
#define DMAREQ_U1RX         0xB
#define DMAREQ_U1TX         0xC
#define DMAREQ_U2RX         0x1E
#define DMAREQ_U2TX         0x1F
#define DMAREQ_U3RX         0x52
#define DMAREQ_U3TX         0x53
#define DMAREQ_U4RX         0x58
#define DMAREQ_U4TX         0x59
#define DMAREQ_ECAN1RX      0x22    
#define DMAREQ_ECAN1TX      0x46
#define DMAREQ_ECAN2RX      0x37
#define DMAREQ_ECAN2TX      0x47
#define DMAREQ_DCI          0x3C
#define DMAREQ_ADC1         0x0D
#define DMAREQ_ADC2         0x1A
#define DMAREQ_PMP          0x2D

//DMAPAD
#define DMAPAD_RD_IC1       0x0144
#define DMAPAD_RD_IC2       0x014C
#define DMAPAD_RD_IC3       0x0154
#define DMAPAD_RD_IC4       0x015C
#define DMAPAD_WR_OC1R      0x0906
#define DMAPAD_WR_OC1RS     0x0904
#define DMAPAD_WR_OC2R      0x0910
#define DMAPAD_WR_OC2RS     0x090E
#define DMAPAD_WR_OC3R      0x091A
#define DMAPAD_WR_OC3RS     0x0918
#define DMAPAD_WR_OC4R      0x0924
#define DMAPAD_WR_OC4RS     0x0922
#define DMAPAD_SPI1         0x0248
#define DMAPAD_SPI2         0x0268
#define DMAPAD_SPI3         0x02A8
#define DMAPAD_SPI4         0x02C8
#define DMAPAD_RD_U1RX      0x0226
#define DMAPAD_WR_U1TX      0x0224
#define DMAPAD_RD_U2RX      0x0236
#define DMAPAD_WR_U2TX      0x0234
#define DMAPAD_RD_U3RX      0x0256
#define DMAPAD_WR_U3TX      0x0254
#define DMAPAD_RD_U4RX      0x02B6
#define DMAPAD_WR_U4TX      0x02B4
#define DMAPAD_RD_ECAN1     0x0440
#define DMAPAD_WR_ECAN1     0x0442
#define DMAPAD_RD_ECAN2     0x0540
#define DMAPAD_WR_ECAN2     0x0542
#define DMAPAD_RD_DCI       0x0290
#define DMAPAD_WR_DCI       0x0298
#define DMAPAD_RD_ADC1      0x0300
#define DMAPAD_RD_ADC2      0x0340
#define DAMPAD_PMP          0x0608

typedef struct
{
    uint16_t buf_length;
    uint8_t txfer_state;
    uint8_t prev_txfer_state;
    uint8_t ping_pong;
}STRUCT_DMA;

void DMA_struct_init (uint8_t channel);
void DMA_init (uint8_t channel);
uint8_t DMA_get_txfer_state (uint8_t channel);
void DMA_set_txfer_state (uint8_t channel, uint8_t state);
void DMA_set_control_register (uint8_t channel, uint16_t DMACHx);
void DMA_set_peripheral_address (uint8_t channel, uint8_t mode, uint16_t addr);
void DMA_set_txfer_length(uint8_t channel, uint16_t address, uint16_t length);
void DMA_set_channel_req (uint8_t channel);
void DMA_set_channel_trigger (uint8_t channel, uint8_t trigger);
uint8_t DMA_get_channel_req (uint8_t channel);
void DMA_enable (uint8_t channel);
void DMA_disable (uint8_t channel);
uint16_t DMA_get_buffer_address (uint8_t channel, uint8_t mode);
uint8_t DMA_get_pingpong_state (uint8_t channel);
#endif