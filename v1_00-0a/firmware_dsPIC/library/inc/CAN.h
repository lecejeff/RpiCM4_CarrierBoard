//***************************************************************************//
// File      :  can.h
//
// Functions :  
//
// Includes  :  dspeak_generic.h
//           
// Purpose   :  Driver for the dsPIC33E CAN peripheral
//              1x native channel on dsPeak
//              CAN_1 : Native CAN2.0B channel
//
// Intellitrol                   MPLab X v5.45                        06/04/2021  
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
#ifndef __CAN_H_
#define	__CAN_H_

dspic_33ck_generic

// CAN channel definitions for dsPIC33E
#define CAN_1                       0
#define CAN_QTY                     1

#define CAN_NODE_TYPE_TX_ONLY       0
#define CAN_NODE_TYPE_RX_ONLY       1
#define CAN_NODE_TYPE_TX_RX         2

#define CAN_LBK_ACTIVE              1
#define CAN_LBK_INACTIVE            0
#define CAN_LBK_STATE               LATGbits.LATG15

// Define CAN operating mode
#define CAN_MODE_LISTEN_ALL         7
#define CAN_MODE_CONFIG             4
#define CAN_MODE_LISTEN_ONLY        3
#define CAN_MODE_LOOPBACK_INT       2
#define CAN_MODE_DISABLE            1
#define CAN_MODE_NORMAL             0

// Define system Time quanta and nominal bit time
#define NOMINAL_TIME_QUANTA         10  // Between 8 and 25
#define PHASE_SEGMENT_1             4   //     
#define PHASE_SEGMENT_2             4   //  
#define PROPAGATION_SEGMENT         1   //
#define SYNCHRONIZATION_JUMP_WIDTH  1   //

#define CAN_BUFFER_X32              32
#define CAN_BUFFER_X24              24
#define CAN_BUFFER_X16              16
#define CAN_BUFFER_X12              12
#define CAN_BUFFER_X8               8
#define CAN_BUFFER_X6               6
#define CAN_BUFFER_X4               4

#define NUM_OF_CAN_BUFFERS          CAN_BUFFER_X4

#define CAN_MAXIMUM_BUS_FREQ        550000UL

#define CAN_MAXIMUM_TX_RETRY        32

typedef struct
{
    uint8_t node_type;              // Type of CAN node
    
    uint8_t channel;                // Physical CAN channel
    uint8_t DMA_tx_channel;         // Physcial DMA TX channel
    uint8_t DMA_rx_channel;         // Physcial DMA RX channel
    
    uint8_t mode;
    uint8_t old_mode;
    uint8_t int_state;
    
    uint32_t time_quantum_frequency;
    uint32_t bus_freq;
    
    uint8_t ivr_flag;
    uint8_t rbif_flag;
    uint8_t tbif_flag;
    
    uint16_t transmit_retry_counter;
}STRUCT_CAN;

typedef struct
{
    uint8_t node_type;          // Type of CAN node
    
    uint8_t tx_msg_channel;     // CAN transmit message channel
    uint8_t rx_msg_channel;     // CAN receive message channel
    uint16_t tx_message[8];
    uint16_t rx_message[8];
    uint8_t tx_payload[8];
    uint8_t rx_payload[8];
    
    uint8_t tx_length;
    uint8_t rx_length;
    
    uint8_t rx_mask_channel;
    uint16_t rx_mask;
    uint16_t rx_sid;
    
    uint16_t rx_filter_channel;
    uint16_t rx_filter_SID;

    // Standard CAN frame bit defines
    uint16_t SID;
    uint16_t EID;
    uint8_t SRR;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t RB1;
    uint8_t RB0;
    uint8_t DLC;
}STRUCT_CAN_MSG;

uint8_t CAN_init_message (STRUCT_CAN_MSG *msg, uint16_t SID, uint16_t EID, uint8_t SRR, 
                            uint8_t IDE, uint8_t RTR, uint8_t RB1, uint8_t RB0, 
                            uint16_t rx_sid, uint8_t tx_msg_channel, uint8_t rx_msg_channel,
                            uint8_t tx_length, uint8_t rx_length, uint8_t node_type);

uint8_t CAN_init (STRUCT_CAN *node, uint8_t channel, uint32_t bus_freq, uint16_t tx_retry,
                uint8_t DMA_tx_channel, uint8_t DMA_rx_channel, uint8_t node_type);

uint8_t CAN_set_rx_filter_sid (STRUCT_CAN_MSG *msg, uint8_t filter_channel, uint16_t filter_SID);

uint8_t CAN_set_rx_mask (STRUCT_CAN_MSG *msg, uint8_t mask_channel, uint16_t mask);
uint8_t CAN_assign_rx_mask (STRUCT_CAN_MSG *msg, uint8_t mask_channel);

void CAN_msg_set_payload (STRUCT_CAN_MSG *msg, uint8_t *buf, uint8_t length);
uint8_t * CAN_receive_msg (STRUCT_CAN_MSG *msg);
uint8_t CAN_set_mode (STRUCT_CAN *node, uint8_t mode);
uint8_t CAN_set_txmsg_channel(STRUCT_CAN_MSG *msg);
uint8_t CAN_get_mode (STRUCT_CAN *node);
uint16_t CAN_get_txmsg_errcnt (STRUCT_CAN *node);
uint16_t CAN_get_rxmsg_errcnt (STRUCT_CAN *node);
uint8_t CAN_get_ivr_state (STRUCT_CAN *node);
uint8_t CAN_send_msg (STRUCT_CAN *node, STRUCT_CAN_MSG *msg, uint8_t priority);
uint8_t CAN_get_txbuf_state (STRUCT_CAN *node);
uint8_t CAN_get_rxbuf_state (STRUCT_CAN *node);

#endif	

