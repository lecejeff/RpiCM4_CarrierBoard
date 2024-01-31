//***************************************************************************//
// File      :  can.c
//
// Functions :  
//
// Includes  :  #include "CAN.h"
//              #include "DMA.h"
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
#include "CAN.h"
#include "DMA.h"

__eds__ uint16_t CAN_MSG_BUFFER[NUM_OF_CAN_BUFFERS][8] __attribute__((eds, space(dma), aligned(32*16)));

STRUCT_CAN CAN_struct[CAN_QTY];

//void CAN_init_struct (STRUCT_CAN *node, uint8_t channel, uint32_t bus_freq, uint16_t SID, uint16_t EID, uint16_t rx_mask, uint16_t rx_sid)//
//Description : Function initialize CAN structure parameters
//
//Function prototype : void CAN_init_struct (STRUCT_CAN *node, uint8_t channel, uint32_t bus_freq, uint16_t SID, uint16_t EID, uint16_t rx_mask, uint16_t rx_sid)
//
//Enter params       : STRUCT_CAN *node    : Pointer to a STRUCT_CAN item
//                   : uint8_t channel     : CAN physical channel
//                   : uint32_t bus_freq   : CAN bus frequency, max is 550kbps (hardware constraint)
//                   : uint16_t SID        : CAN Standard identifier
//                   : uint16_t EID        : CAN extended identifier
//                   : uint16_t rx_sid     : CAN receive SID
//
//Exit params        : None
//
//Function call      : CAN_init_struct(CAN_native, CAN_1, 500000, 0x0123, 0, 0x0300, 0x0300);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t CAN_init_message (STRUCT_CAN_MSG *msg, uint16_t SID, uint16_t EID, uint8_t SRR, uint8_t IDE, uint8_t RTR, uint8_t RB1, uint8_t RB0, 
                            uint16_t rx_sid, uint8_t tx_msg_channel, uint8_t rx_msg_channel, uint8_t tx_length, uint8_t rx_length, uint8_t node_type)
{
    uint8_t i = 0;
    if (node_type > 2){return 0;}
    msg->node_type = node_type;

    // Saturation for transmit / receive channel and length
    if (tx_msg_channel > 7){return 0;}      // Only channel 0..7 can be used for transmission
    msg->tx_msg_channel = tx_msg_channel;
    if (rx_msg_channel > 31){return 0;}     // Channel 0..31 can be used for reception
    msg->rx_msg_channel = rx_msg_channel;
    if (tx_length > 8){return 0;}           // CAN 2.0B max payload length is 8 bytes
    msg->tx_length = tx_length;
    if (rx_length > 8){return 0;}           // CAN 2.0B max payload length is 8 bytes
    msg->tx_length = rx_length;
    
    // CAN message configuration bits, see family reference manual
    msg->SID = SID;                         // SID (standard identifier, 11 bits)
    msg->EID = EID;                         // EID (extended identifier, 18 bits)
    msg->SRR = SRR;                         // SRR (remote request substitute bit, 1 = remote, 0 = normal)
    msg->IDE = IDE;                         // IDE (extended identifier bit, 1 = extended, 0 = standard)
    msg->RTR = RTR;                         // RTR (remote transmission request)
    
    // Per CAN 2.0B specification, RB1 and RB0 must always be set to '0'
    if (RB1 != 0){return 0;}
    msg->RB1 = RB1;                         // RB1 (reserved bit 1, must be set to '0')
    if (RB0 != 0){return 0;}
    msg->RB0 = RB0;                         // RB1 (reserved bit 0, must be set to '0')
  
    msg->DLC = tx_length;   
    // Initialize tx and rx payload bytes to 0 by default
    for (i = 0; i < msg->tx_length; i++)
    {
        msg->tx_payload[i] = 0;
        msg->rx_payload[i] = 0;
    }

    // Set tx message channel register
    if ((msg->node_type == CAN_NODE_TYPE_TX_ONLY) || (msg->node_type == CAN_NODE_TYPE_TX_RX))
    {
        switch (msg->tx_msg_channel)
        {
            case 0:
                C1TR01CONbits.TXEN0 = 1;            // Set buffer 0 to transmit buffer
                C1TR01CONbits.TX0PRI = 0x3;
                break; 

            case 1:
                C1TR01CONbits.TXEN1 = 1;            // Set buffer 1 to transmit buffer
                C1TR01CONbits.TX1PRI = 0x3;
                break;

            case 2:
                C1TR23CONbits.TXEN2 = 1;            // Set buffer 2 to transmit buffer
                C1TR23CONbits.TX2PRI = 0x3;
                break;

            case 3:
                C1TR23CONbits.TXEN3 = 1;            // Set buffer 3 to transmit buffer
                C1TR23CONbits.TX3PRI = 0x3;
                break;

            case 4:
                C1TR45CONbits.TXEN4 = 1;            // Set buffer 4 to transmit buffer
                C1TR45CONbits.TX4PRI = 0x3;
                break; 

            case 5:
                C1TR45CONbits.TXEN5 = 1;            // Set buffer 5 to transmit buffer
                C1TR45CONbits.TX5PRI = 0x3;
                break;

            case 6:
                C1TR67CONbits.TXEN6 = 1;            // Set buffer 6 to transmit buffer
                C1TR67CONbits.TX6PRI = 0x3;
                break;

            case 7:
                C1TR67CONbits.TXEN7 = 1;            // Set buffer 7 to transmit buffer
                C1TR67CONbits.TX7PRI = 0x3;
                break;
                
            default:
                return 0;
                break;
        }
        
        // Initialize CAN message
        CAN_MSG_BUFFER[msg->tx_msg_channel][0] = ((msg->SID << 2) | (msg->SRR << 1) | msg->IDE);
        CAN_MSG_BUFFER[msg->tx_msg_channel][1] = msg->EID & 0x3FFC0;
        CAN_MSG_BUFFER[msg->tx_msg_channel][2] = (((msg->EID & 0x0003F) << 10) | (msg->RTR << 9)
                                                | (msg->RB1 << 8) | (msg->RB0 << 4) | (msg->DLC));
        // Initialize CAN message payload to 0
        CAN_MSG_BUFFER[msg->tx_msg_channel][3] = 0;
        CAN_MSG_BUFFER[msg->tx_msg_channel][4] = 0;
        CAN_MSG_BUFFER[msg->tx_msg_channel][5] = 0;
        CAN_MSG_BUFFER[msg->tx_msg_channel][6] = 0;    
        CAN_MSG_BUFFER[msg->tx_msg_channel][7] = 0; 
    }
    
    if ((msg->node_type == CAN_NODE_TYPE_RX_ONLY) || (msg->node_type == CAN_NODE_TYPE_TX_RX))
    {
        msg->rx_sid = rx_sid;
    }

    return 1;
}

uint8_t CAN_set_rx_filter_sid (STRUCT_CAN_MSG *msg, uint8_t filter_channel, uint16_t filter_SID)
{    
    if ((msg->node_type == CAN_NODE_TYPE_RX_ONLY) || (msg->node_type == CAN_NODE_TYPE_TX_RX))
    {
        if (filter_channel > 15){return 0;}
        msg->rx_filter_channel = filter_channel;
        msg->rx_filter_SID = filter_SID;
        C1CTRL1bits.WIN = 1;                        // Set at 1 to access CAN filter / mask registers  
        switch (msg->rx_filter_channel)
        {
            case 0:
                C1RXF0SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN0 = 1;              // Acceptance filter enabled          
                break;

            case 1:
                C1RXF1SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN1 = 1;              // Acceptance filter enabled  
                break;

            case 2:
                C1RXF2SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN2 = 1;              // Acceptance filter enabled          
                break;

            case 3:
                C1RXF3SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN3 = 1;              // Acceptance filter enabled  
                break;

            case 4:
                C1RXF4SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN4 = 1;              // Acceptance filter enabled          
                break;

            case 5:
                C1RXF5SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN5 = 1;              // Acceptance filter enabled  
                break;

            case 6:
                C1RXF6SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN6 = 1;              // Acceptance filter enabled          
                break;

            case 7:
                C1RXF7SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN7 = 1;              // Acceptance filter enabled  
                break;

            case 8:
                C1RXF8SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN8 = 1;              // Acceptance filter enabled          
                break;

            case 9:
                C1RXF9SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN9 = 1;              // Acceptance filter enabled  
                break;

            case 10:
                C1RXF10SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN10 = 1;              // Acceptance filter enabled          
                break;

            case 11:
                C1RXF11SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN11 = 1;              // Acceptance filter enabled  
                break;

            case 12:
                C1RXF12SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN12 = 1;              // Acceptance filter enabled          
                break;

            case 13:
                C1RXF13SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN13 = 1;              // Acceptance filter enabled  
                break;

            case 14:
                C1RXF14SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN14 = 1;              // Acceptance filter enabled          
                break;

            case 15:
                C1RXF15SIDbits.SID = msg->rx_filter_SID;
                C1FEN1bits.FLTEN15 = 1;              // Acceptance filter enabled  
                break;
        }
        C1CTRL1bits.WIN = 0;                // Set at 0 to access CAN control registers   
        return 1;
    }   
    else
        return 0;
}

uint8_t CAN_set_rx_mask (STRUCT_CAN_MSG *msg, uint8_t mask_channel, uint16_t mask)
{
    if ((msg->node_type == CAN_NODE_TYPE_RX_ONLY) || (msg->node_type == CAN_NODE_TYPE_TX_RX))
    {  
        if (mask_channel > 2){return 0;}            // dsPIC33E supports 3x different hardware defined masks
        msg->rx_mask_channel = mask_channel;
        msg->rx_mask = mask;
        C1CTRL1bits.WIN = 1;                        // Set at 1 to access CAN filter / mask registers  
        switch (msg->rx_mask_channel)
        {
            case 0:
                C1RXM0SIDbits.SID = msg->rx_mask;   // Setup acceptance mask
                //C1FMSKSEL1bits.F0MSK = 1;           // Acceptance mask registers contain mask
                       
                break;

            case 1:
                C1RXM1SIDbits.SID = msg->rx_mask;   // Setup acceptance mask
                break;

            case 2:
                C1RXM2SIDbits.SID = msg->rx_mask;   // Setup acceptance mask
                break;
        }
        C1CTRL1bits.WIN = 0;                // Set at 0 to access CAN control registers   
        return 1;
    }
    else
        return 0;
}

uint8_t CAN_assign_rx_mask (STRUCT_CAN_MSG *msg, uint8_t mask_channel)
{
    if ((msg->node_type == CAN_NODE_TYPE_RX_ONLY) || (msg->node_type == CAN_NODE_TYPE_TX_RX))
    {
        if (mask_channel > 2){return 0;}
        switch (msg->rx_msg_channel)
        {
            case 0:
                C1BUFPNT1bits.F0BP = 0;             // Acceptance filter will use message buffer 0    
                break;
                
            case 1:
                C1BUFPNT1bits.F1BP = 1;             // Acceptance filter will use message buffer 1   
                break;
                
            case 2:
                break;
                
            case 3:
                break;
                
            case 4:
                break;
        }
        return 1;
    }
    else
        return 0;
}

//********************uint8_t CAN_init (STRUCT_CAN *node)*********************//
//Description : Function initialize CAN state machine and registers
//
//Function prototype : uint8_t CAN_init (STRUCT_CAN *node)
//
//Enter params       : STRUCT_CAN *node    : Pointer to a STRUCT_CAN item
//
//Exit params        : uint8_t : CAN configuration status
//                               1 : Configuration successful
//                               0 : Error
//Function call      : CAN_init(CAN_native);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          05/04/2021   
// Jean-Francois Bilodeau, B.E.Eng/CPI #6022173 
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t CAN_init (STRUCT_CAN *node, uint8_t channel, uint32_t bus_freq, uint16_t tx_retry,
                uint8_t DMA_tx_channel, uint8_t DMA_rx_channel, uint8_t node_type)
{
    node->channel = channel;
    node->bus_freq = bus_freq;
    node->DMA_tx_channel = DMA_tx_channel;
    node->DMA_rx_channel = DMA_rx_channel;
    node->ivr_flag = 0;                         // Invalid error flag reset
    node->tbif_flag = 0;                        // Transmit buffer interrupt flag reset
    node->rbif_flag = 0;                        // Receive buffer interrupt flag reset
    node->transmit_retry_counter = tx_retry;    // Transmit retry counter reset
    
    // Make sure node CAN physical channel is in config mode before initializing
    // the CAN registers, otherwise write to these registers will be discarded
    if (CAN_get_mode(node) != CAN_MODE_CONFIG)
    {
        CAN_set_mode (node, CAN_MODE_CONFIG);
        if (CAN_get_mode(node) != CAN_MODE_CONFIG)
        {
            return 0;
        }
    }    
    else
    {      
        switch (node->channel)
        {
            case CAN_1:
                IEC2bits.C1IE = 0;                  // Disable CAN interrupt during initialization
                IFS2bits.C1IF = 0;                  // Reset interrupt flag 
                C1CTRL1bits.WIN = 0;                // Set at 0 to access CAN control registers                 
                switch (NUM_OF_CAN_BUFFERS)
                {
                    case CAN_BUFFER_X32:
                        C1FCTRLbits.DMABS = 6;              // 32 message buffers in device RAM
                        break;
                        
                    case CAN_BUFFER_X24:
                        C1FCTRLbits.DMABS = 5;              // 24 message buffers in device RAM
                        break;
                        
                    case CAN_BUFFER_X16:
                        C1FCTRLbits.DMABS = 4;              // 16 message buffers in device RAM
                        break;
                        
                    case CAN_BUFFER_X12:
                        C1FCTRLbits.DMABS = 3;              // 12 message buffers in device RAM
                        break;
                        
                    case CAN_BUFFER_X8:
                        C1FCTRLbits.DMABS = 2;              // 8 message buffers in device RAM
                        break;
                        
                    case CAN_BUFFER_X6:
                        C1FCTRLbits.DMABS = 1;              // 6 message buffers in device RAM
                        break;
                    
                    case CAN_BUFFER_X4:
                        C1FCTRLbits.DMABS = 0;              // 4 message buffers in device RAM
                        break;
                }
                
                // CAN channel physical pin configuration
                TRISGbits.TRISG15 = 0;              // RG15 configured as an output (CAN1_LBK)
                CAN_LBK_STATE = CAN_LBK_INACTIVE;   // Disable loopback during initialization
                TRISEbits.TRISE5 = 0;               // RE5 configured as an output (CAN1_TX)
                TRISEbits.TRISE6 = 1;               // RE6 configured as an input (CAN1_RX)
                RPOR6bits.RP85R = 0x0E;             // RE5 (RP85) assigned to CAN1_TX
                RPINR26bits.C1RXR = 86;             // RE6 (RPI86) assigned to CAN1_RX

                // CAN bus nominal bit time and time quanta
                C1CTRL1bits.CANCKS = 1;             // IMPLEMENTATION REVERSED, SEE ERRATA DS80000526H-page 8 -> Fcan = Fp (Fcy) = (FOSC / 2)
                C1CFG2bits.SAM = 1;                 // BUS sampled 3x times at the sample point
                // Make sure to limit the CAN bus frequency to 550kHz
                if (node->bus_freq > CAN_MAXIMUM_BUS_FREQ){return 1;}
                else
                {  
                    // Hard requirements for the CAN bus bit timing that must be followed :
                    // Bit time = Sync segment + Propagation segment + Phase segment 1 + Phase segment 2
                    // 1) Propagation segment + Phase segment 1 >=  Phase segment 2
                    // 2) Phase segment 2 >= Synchronous jump width (SJW)  
                    // If you want to change the bit timing, make sure to use 
                    // MPLAB X IDE Tools / Embedded / ECAN bit rate calculator
                    node->time_quantum_frequency = NOMINAL_TIME_QUANTA * node->bus_freq;
                    C1CFG1bits.BRP = (node->time_quantum_frequency / (2*node->bus_freq))+1;
                    C1CFG2bits.SEG1PH = PHASE_SEGMENT_1 - 1;        // Set phase segment 1 
                    C1CFG2bits.SEG2PHTS = 0;                        // Set SEG2 constrained to "Maximum of SEG1PHx bits"
                    C1CFG2bits.SEG2PH = PHASE_SEGMENT_2 - 1;        // Set phase segment 2
                    C1CFG2bits.PRSEG = PROPAGATION_SEGMENT - 1;     // Set propagation segment
                    C1CFG1bits.SJW = SYNCHRONIZATION_JUMP_WIDTH - 1;// Set SJW
                }

                // DMA channel initialization, 1x channel for message transmission
                node->DMA_tx_channel = DMA_tx_channel;
                DMA_init(node->DMA_tx_channel);
                DMA_set_control_register(node->DMA_tx_channel, (DMA_SIZE_WORD | DMA_TXFER_WR_PER | DMA_AMODE_PIA | DMA_CHMODE_CPPD));
                DMA_set_request_source(node->DMA_tx_channel, DMAREQ_ECAN1TX);
                DMA_set_peripheral_address(node->DMA_tx_channel, (volatile uint16_t)&C1TXD);
                DMA_set_buffer_offset_sgl(node->DMA_tx_channel, __builtin_dmapage(CAN_MSG_BUFFER), __builtin_dmaoffset(CAN_MSG_BUFFER));
                DMA_set_txfer_length(node->DMA_tx_channel, 7); 
                
                // DMA channel initialization, 1x channel for message reception
                node->DMA_rx_channel = DMA_tx_channel;
                DMA_init(node->DMA_rx_channel);
                DMA_set_control_register(node->DMA_rx_channel, (DMA_SIZE_WORD | DMA_TXFER_RD_PER | DMA_AMODE_PIA | DMA_CHMODE_CPPD));
                DMA_set_request_source(node->DMA_rx_channel, DMAREQ_ECAN1RX);
                DMA_set_peripheral_address(node->DMA_rx_channel, (volatile uint16_t)&C1RXD);
                DMA_set_buffer_offset_sgl(node->DMA_rx_channel, __builtin_dmapage(CAN_MSG_BUFFER), __builtin_dmaoffset(CAN_MSG_BUFFER));
                DMA_set_txfer_length(node->DMA_rx_channel, 7);  
                
                // Enable CAN interrupts
                IEC2bits.C1IE = 1;
                C1INTEbits.TBIE = 1;
                C1INTEbits.RBIE = 1;
               
                DMA_enable(node->DMA_tx_channel);                // Enable DMA channel and interrupt  
                DMA_enable(node->DMA_rx_channel);                // Enable DMA channel and interrupt 
                return 1;
                break;

            default:
                return 0;                           // Return error 
                break;
        }       
    }
    return 1;
}

void CAN_msg_set_payload (STRUCT_CAN_MSG *msg, uint8_t *buf, uint8_t length)
{
    uint8_t i = 0;
    if (length > msg->tx_length){length = msg->tx_length;}
    for (i = 0; i < length; i++)
    {
        msg->tx_payload[i] = *buf++;
    }
    CAN_MSG_BUFFER[msg->tx_msg_channel][3] = (uint16_t)((msg->tx_payload[1]<<8) | msg->tx_payload[0]);
    CAN_MSG_BUFFER[msg->tx_msg_channel][4] = (uint16_t)((msg->tx_payload[3]<<8) | msg->tx_payload[2]);
    CAN_MSG_BUFFER[msg->tx_msg_channel][5] = (uint16_t)((msg->tx_payload[5]<<8) | msg->tx_payload[4]);
    CAN_MSG_BUFFER[msg->tx_msg_channel][6] = (uint16_t)((msg->tx_payload[7]<<8) | msg->tx_payload[6]);
}

uint8_t * CAN_receive_msg (STRUCT_CAN_MSG *msg)
{
    uint8_t i = 0;
    if (((CAN_MSG_BUFFER[msg->rx_msg_channel][i] & 0x1FFC) >> 2) == msg->SID)
    {
        for (i = 0; i < 8; i++)
        {
            msg->rx_message[i] = CAN_MSG_BUFFER[msg->rx_msg_channel][i];
        }
        msg->rx_payload[0] = CAN_MSG_BUFFER[msg->rx_msg_channel][3];
        msg->rx_payload[1] = (CAN_MSG_BUFFER[msg->rx_msg_channel][3] >> 8);
        msg->rx_payload[2] = CAN_MSG_BUFFER[msg->rx_msg_channel][4];
        msg->rx_payload[3] = (CAN_MSG_BUFFER[msg->rx_msg_channel][4] >> 8);
        msg->rx_payload[4] = CAN_MSG_BUFFER[msg->rx_msg_channel][5];
        msg->rx_payload[5] = (CAN_MSG_BUFFER[msg->rx_msg_channel][5] >> 8);
        msg->rx_payload[6] = CAN_MSG_BUFFER[msg->rx_msg_channel][6];
        msg->rx_payload[7] = (CAN_MSG_BUFFER[msg->rx_msg_channel][6] >> 8);             
        return &msg->rx_payload[0];
    }
    else
        return 0;                   // Receive message did not match SID, return 0
}

// Returns 0 on successful execution
// Returns 0xFF on error
// Blocking call
uint8_t CAN_set_mode (STRUCT_CAN *node, uint8_t mode)
{
    node->old_mode = node->mode;
    node->mode = mode;  
    switch (node->channel)
    {
        case CAN_1:
            C1CTRL1bits.REQOP = mode;
            break;
        
        default:
            return 0xFF;
            break;
    }
    // Mode change occurs only when the bus is idle (11 successive recessive bits)
    // The following blocks the program execution until mode change is done
    while(CAN_get_mode(node) != node->mode);
    return node->mode;
}

// Returns 0,1,2,3,4 or 7 in case of successful operation
// Returns 0xFF on error
uint8_t CAN_get_mode (STRUCT_CAN *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1CTRL1bits.OPMODE;
            break;
            
        default:
            return 0xFF;
            break;
    }
}

// Non-blocking
// Return 1 on successful transmission
// Return 0 if trying to send message but the CAN node is not in NORMAL mode
// Return 2 if TXENn = 0 and trying to send a message using this TXENn buffer
// Return 3 if trying to send a message but the module is already transmitting
// This function call assumes a message has already been properly configured
uint8_t CAN_send_msg (STRUCT_CAN *node, STRUCT_CAN_MSG *msg, uint8_t priority)
{
    if (priority > 3){return 0;}
    if (CAN_get_mode(node) != CAN_MODE_NORMAL)
    {
        // CAN bus is not in normal mode, cannot send message
        return 0;
    }

    // From CAN Family reference manual p.36, "Setting the TXREQm bit when the 
    // TXENn bit is '0' will result in unpredictable module behavior"
    switch (msg->tx_msg_channel)
    {
        case 0:
            if (C1TR01CONbits.TXREQ0 == 0)
            {
                if (C1TR01CONbits.TXEN0 == 0){return 2;}
                else
                {
                    C1TR01CONbits.TX0PRI = priority;
                    C1TR01CONbits.TXREQ0 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 1:
            if (C1TR01CONbits.TXREQ1 == 0)
            {
                if (C1TR01CONbits.TXEN1 == 0){return 2;}
                else
                {
                    C1TR01CONbits.TX1PRI = priority;
                    C1TR01CONbits.TXREQ1 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 2:
            if (C1TR23CONbits.TXREQ2 == 0)
            {
                if (C1TR23CONbits.TXEN2 == 0){return 2;}
                else
                {
                    C1TR23CONbits.TX2PRI = priority;
                    C1TR23CONbits.TXREQ2 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 3:
            if (C1TR23CONbits.TXREQ3 == 0)
            {
                if (C1TR23CONbits.TXEN3 == 0){return 2;}
                else
                {
                    C1TR23CONbits.TX3PRI = priority;
                    C1TR23CONbits.TXREQ3 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 4:
            if (C1TR45CONbits.TXREQ4 == 0) 
            {
                if (C1TR45CONbits.TXEN4 == 0){return 2;}
                else
                {
                    C1TR45CONbits.TX4PRI = priority;
                    C1TR45CONbits.TXREQ4 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 5:
            if (C1TR45CONbits.TXREQ5 == 0)
            {
                if (C1TR45CONbits.TXEN5 == 0){return 2;}
                else
                {
                    C1TR45CONbits.TX5PRI = priority;
                    C1TR45CONbits.TXREQ5 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 6:
            if (C1TR67CONbits.TXREQ6 == 0)
            {
                if (C1TR67CONbits.TXEN6 == 0){return 2;}
                else
                {
                    C1TR67CONbits.TX6PRI = priority;
                    C1TR67CONbits.TXREQ6 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break;

        case 7:
            if (C1TR67CONbits.TXREQ7 == 0)
            {
                if (C1TR67CONbits.TXEN7 == 0){return 2;}
                else
                {
                    C1TR67CONbits.TX7PRI = priority;
                    C1TR67CONbits.TXREQ7 = 0x1; 
                    return 1;
                }
            }
            else
                return 3;
            break; 
            
        default:
            return 0;
            break;
    }
}

uint16_t CAN_get_txmsg_errcnt (STRUCT_CAN *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1ECbits.TERRCNT;
            break;
            
        default:
            return 0;
            break;
    }
}

uint16_t CAN_get_rxmsg_errcnt (STRUCT_CAN *node)
{
    switch (node->channel)
    {
        case CAN_1:
            return C1ECbits.RERRCNT;
            break;
            
        default:
            return 0;
            break;
    }    
}

uint8_t CAN_get_ivr_state (STRUCT_CAN *node)
{
    if (node->ivr_flag == 1)
    {
        node->ivr_flag = 0;
        return 1;
    }
    else
        return 0;
}

uint8_t CAN_get_txbuf_state (STRUCT_CAN *node)
{
    if (node->tbif_flag == 1)
    {
        node->tbif_flag = 0;
        return 1;
    }
    else
        return 0;    
}

uint8_t CAN_get_rxbuf_state (STRUCT_CAN *node)
{
    if (node->rbif_flag == 1)
    {
        node->rbif_flag = 0;
        return 1;
    }
    else
        return 0;    
}

// ECAN1 event interrupt
void __attribute__((__interrupt__, no_auto_psv))_C1Interrupt(void)
{
    IFS2bits.C1IF = 0;      // clear interrupt flag
    if(C1INTFbits.TBIF)     // Transmit buffer interrupt flag
    {
        CAN_struct[CAN_1].tbif_flag = 1;
        C1INTFbits.TBIF = 0;
    }

    if(C1INTFbits.RBIF)     // Receive buffer interrupt flag
    {
        CAN_struct[CAN_1].rbif_flag = 1;
        C1INTFbits.RBIF = 0;
    }
    
    if (C1INTFbits.ERRIF)
    {        
        CAN_struct[CAN_1].ivr_flag = 1;
        if (C1INTFbits.IVRIF)   // Invalid message interrupt triggered?
        {
            C1INTFbits.IVRIF = 0;
        }
        C1INTFbits.ERRIF = 0;
    }
}

//// ECAN1 Receive data ready interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C1RxRdyInterrupt(void)
//{
//    
//}
//
// ECAN1 Transmit data request interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C1TxReqInterrupt(void)
//{
//}

//// ECAN2 Receive data ready interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C2RxRdyInterrupt(void)
//{
//    
//}
//
//// ECAN2 Transmit data request interrupt
//void __attribute__((__interrupt__, no_auto_psv))_C2TxReqInterrupt(void)
//{
//    
//}
