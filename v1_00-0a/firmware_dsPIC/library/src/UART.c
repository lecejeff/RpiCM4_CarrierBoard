//****************************************************************************//
// File      :  UART.c
//
// Includes  :  UART.h
//              string.h
//              DMA.h
//
// Purpose   :  Driver for the dsPIC33CK UART peripheral
//              3x seperate UART channels on rPICM4Cb :
//              UART_1 : dsPIC <-> Debug interface
//              UART_2 : MikroBus 1 UART interface
//              UART_3 : MikroBus 2 UART interface
//              
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#include "UART.h"
#include "DMA.h"

STRUCT_UART UART_struct[UART_QTY];

// Define UART_x channel DMA buffers (either transmit, receive or both)
#ifdef UART1_DMA_ENABLE
uint8_t uart1_dma_tx_buf[UART_MAX_TX];
#endif

#ifdef UART2_DMA_ENABLE
uint8_t uart2_dma_tx_buf[UART_MAX_TX];
#endif

#ifdef UART3_DMA_ENABLE
uint8_t uart3_dma_tx_buf[UART_MAX_TX];
#endif

//***void UART_init (STRUCT_UART *str, uint8_t channel, uint32_t baud, 
//                uint16_t tx_buf_length, uint16_t rx_buf_length)
//Description : Function initialize UART channel at specified baudrate with 
//              specified buffer length, which size cannot exceed 256 bytes
//
//Function prototype : void UART_init (uint8_t channel, uint32_t baud, uint8_t rx_buf_length)
//
//Enter params       : STRUCT_UART *str         : structure pointer type
//                   : uint8_t channel          : UART channel
//                   : uint32_t baud            : UART baudrate
//                   : uint8_t tx_buf_length    : UART transmit buffer length
//                   : uint8_t rx_buf_length    : UART receive buffer length
//
//Exit params        : None
//
//Function call      : UART_init (&UART_x, UART_1, 115200, 32, 32)
//
// Intellitrol           MPLab X v6.05            XC16 v1.10          01/01/2024  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_init (STRUCT_UART *uart, uint8_t channel, uint32_t baud, uint16_t tx_buf_length, 
                uint16_t rx_buf_length, uint8_t DMA_tx_channel)
{
    uint32_t brg = 0;
    switch (channel)
    {
        // UART_1 serves as a UART debug interface on rPICM4CB
        case UART_1:  
            uart->DMA_enabled = 0;          // Disable DMA by default
            
            U1MODEbits.UARTEN = 0;          // Disable UART if it was previously used
            IEC0bits.U1RXIE = 0;            // Disable UART receive interrupt   
            IEC0bits.U1TXIE = 0;            // Disable UART transmit interupt
            IFS0bits.U1RXIF = 0;            // Clear UART receive interrupt flag
            IFS0bits.U1TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART1 input/output pin mapping
            TRISCbits.TRISC12 = 0;          // RC12 configured as an output (UART1_TX)           
            TRISCbits.TRISC13 = 1;          // RC13 configured as an input (UART1_RX)
            //CNPUCbits.CNPUC13 = 1;          // Since URXINV = 0, enable pull-up on RX pin (remove PD on board)
            
            // UART1 peripheral pin mapping
            RPOR14bits.RP60R = 1;           // RC12 (RP60) assigned to UART1_TX
            RPINR18bits.U1RXR = 61;         // RC13 (RP61) assigned to UART1_RX
            
            // Configure baud rate
            U1MODEHbits.BCLKMOD = 1;    // Use fractional division for baudrate
            U1MODEHbits.BCLKSEL = 0;    // Use Fcy as input clock source
            brg = (uint32_t)(FCY / baud);
            if (brg > 0xFFFFF){brg = 0xFFFFF;}
            U1BRGH = (brg >> 16);
            U1BRG = brg;

            // Configure transmit 
            U1MODEbits.UTXEN = 1;           // Enable UART TX
            IPC3bits.U1TXIP = 7;            // TX interrupt priority
            
            // Configure receive
            U1STAHbits.STPMD = 1;           // Trigger RXIF at end of last stop bit
            U1MODEbits.URXEN = 1;           // Enable UART RX
            IPC2bits.U1RXIP = 7;            // RX interrupt priority
            IEC0bits.U1RXIE = 1;            // Enable receive interrupt  

            // Enable UART
            U1MODEbits.UARTEN = 1;          
#ifdef UART1_DMA_ENABLE  
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TRMODE_ONES | DMA_DADMODE_UNCH | DMA_SADMODE_INCR));
            DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart1_dma_tx_buf);
            DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_DESTIN, (volatile uint16_t)&U1TXREG);
            DMA_set_channel_trigger(uart->DMA_tx_channel, 0x05);    // UART1 transmitter
#endif
            break;
           
        // UART_2 serves as a UART interface on MKB 1  
        case UART_2:
            uart->DMA_enabled = 0;          // Disable DMA by default
            
            U2MODEbits.UARTEN = 0;          // Disable UART if it was previously used
            IEC1bits.U2RXIE = 0;            // Disable UART receive interrupt   
            IEC1bits.U2TXIE = 0;            // Disable UART transmit interupt
            IFS1bits.U2RXIF = 0;            // Clear UART receive interrupt flag
            IFS1bits.U2TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART2 input/output pin mapping
            TRISFbits.TRISF15 = 0;          // RF15 configured as an output (MKB1 - UART2_TX)           
            TRISDbits.TRISD13 = 1;          // RD13 configured as an input (MKB1 - UART2_RX)
            //CNPUDbits.CNPUD13 = 1;          // Since URXINV = 0, enable pull-up on RX pin (remove PD on board)
            
            // UART2 peripheral pin mapping
            RPOR31bits.RP95R = 3;           // RF15 (RP95) assigned to MKB1 - UART2_TX
            RPINR19bits.U2RXR = 77;         // RD13 (RP77) assigned to MKB1 - UART2_RX
            
            // Configure baud rate
            U2MODEHbits.BCLKMOD = 1;    // Use fractional division for baudrate
            U2MODEHbits.BCLKSEL = 0;    // Use Fcy as input clock source
            brg = (uint32_t)(FCY / baud);
            if (brg > 0xFFFFF){brg = 0xFFFFF;}
            U2BRGH = (brg >> 16);
            U2BRG = brg;

            // Configure transmit 
            U2MODEbits.UTXEN = 1;           // Enable UART TX
            IPC7bits.U2TXIP = 7;            // TX interrupt priority
            
            // Configure receive
            U2STAHbits.STPMD = 1;           // Trigger RXIF at end of last stop bit
            U2MODEbits.URXEN = 1;           // Enable UART RX
            IPC6bits.U2RXIP = 7;            // RX interrupt priority
            IEC1bits.U2RXIE = 1;            // Enable receive interrupt  

            // Enable UART
            U2MODEbits.UARTEN = 1;          
#ifdef UART2_DMA_ENABLE  
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TRMODE_ONES | DMA_DADMODE_UNCH | DMA_SADMODE_INCR));
            DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart2_dma_tx_buf);
            DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_DESTIN, (volatile uint16_t)&U2TXREG);
            DMA_set_channel_trigger(uart->DMA_tx_channel, 0x0F);    // UART2 transmitter
#endif
            break;
            
        // UART_3 serves as a UART interface on MKB 2  
        case UART_3: 
            uart->DMA_enabled = 0;          // Disable DMA by default
            
            U3MODEbits.UARTEN = 0;          // Disable UART if it was previously used
            IEC3bits.U3RXIE = 0;            // Disable UART receive interrupt   
            IEC3bits.U3TXIE = 0;            // Disable UART transmit interupt
            IFS3bits.U3RXIF = 0;            // Clear UART receive interrupt flag
            IFS3bits.U3TXIF = 0;            // Clear UART transmit interrupt flag
            
            // UART3 input/output pin mapping
            TRISDbits.TRISD11 = 0;          // RD11 configured as an output (MKB2 - UART3_TX)           
            TRISFbits.TRISF8 = 1;           // RF8 configured as an input (MKB2 - UART3_RX)
            //CNPUFbits.CNPUF8 = 1;           // Since URXINV = 0, enable pull-up on RX pin (remove PD on board)
            
            // UART3 peripheral pin mapping
            RPOR21bits.RP75R = 27;          // RD11 (RP75) assigned to MKB2 - UART3_TX
            RPINR27bits.U3RXR = 88;         // RF8 (RP88) assigned to MKB2 - UART3_RX
            
            // Configure baud rate
            U3MODEHbits.BCLKMOD = 1;    // Use fractional division for baudrate
            U3MODEHbits.BCLKSEL = 0;    // Use Fcy as input clock source
            brg = (uint32_t)(FCY / baud);
            if (brg > 0xFFFFF){brg = 0xFFFFF;}
            U3BRGH = (brg >> 16);
            U3BRG = brg;

            // Configure transmit 
            U3MODEbits.UTXEN = 1;           // Enable UART TX
            IPC14bits.U3TXIP = 7;            // TX interrupt priority
            
            // Configure receive
            U3STAHbits.STPMD = 1;           // Trigger RXIF at end of last stop bit
            U3MODEbits.URXEN = 1;           // Enable UART RX
            IPC14bits.U3RXIP = 7;            // RX interrupt priority
            IEC3bits.U3RXIE = 1;            // Enable receive interrupt  

            // Enable UART
            U3MODEbits.UARTEN = 1;  
#ifdef UART3_DMA_ENABLE   
            uart->DMA_tx_channel = DMA_tx_channel;
            DMA_init(uart->DMA_tx_channel);
            DMA_set_control_register(uart->DMA_tx_channel, (DMA_SIZE_BYTE | DMA_TRMODE_ONES | DMA_DADMODE_UNCH | DMA_SADMODE_INCR));
            DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart3_dma_tx_buf);
            DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_DESTIN, (volatile uint16_t)&U3TXREG);
            DMA_set_channel_trigger(uart->DMA_tx_channel, 0x69);    // UART3 transmitter       
#endif
            break; 
            
        default:
            break;
    }
    
    if (rx_buf_length > UART_MAX_RX)
    {
        rx_buf_length = UART_MAX_RX;
    }
    uart->rx_buf_length = rx_buf_length;
    
    if (tx_buf_length > UART_MAX_TX)
    {
        tx_buf_length = UART_MAX_TX;
    }    
    uart->tx_buf_length = tx_buf_length;
    
    uart->tx_done = UART_TX_COMPLETE;   // Ready for new transmission
    uart->tx_length = 0;                // 
    uart->rx_done = 0;                  // 
    uart->rx_counter = 0;               // Reset rx data counter
    uart->tx_counter = 0;               // Reset tx data counter
    uart->UART_channel = channel;       // Assign uart channel to struct
}   

//************uint8_t UART_get_rx_buffer_length (STRUCT_UART *uart)************//
//Description : Function returns the receive buffer length of the structure
//
//Function prototype : uint8_t UART_get_rx_buffer_length (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : None
//
//Function call      : uint8_t = UART_get_rx_buffer_length(&UART_x);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint16_t UART_get_rx_buffer_length (STRUCT_UART *uart)
{
    return uart->rx_buf_length;
}

uint8_t UART_get_trmt_state (STRUCT_UART *uart)
{
    switch (uart->UART_channel)
    {
        case UART_1:
            return U1STAbits.TRMT;
            break;
            
        case UART_2:
            return U2STAbits.TRMT;
            break;
            
        case UART_3:
            return U3STAbits.TRMT;
            break;           
            
        default: 
            return 0;
            break;
    }
}

//*************void UART_putc (STRUCT_UART *uart, uint8_t data)****************//
//Description : Function transmits a single character on selected channel via
//              interrupt.
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty
//
//Function prototype : void UART_putc (STRUCT_UART *uart, uint8_t data)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : uint8_t data         : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc(UART_1, 'A');
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putc (STRUCT_UART *uart, uint8_t data)
{
    switch(uart->UART_channel)
    {
        case UART_1:
            while(!U1STAbits.TRMT);     // Wait for TX shift reg to be empty    
            uart->tx_length = 1;        // Set TX length
            uart->tx_buf[0] = data;     // Fill buffer 
            uart->DMA_enabled = 0;      // Disable DMA for the next transfer
            IEC0bits.U1TXIE = 1;        // Enable interrupt, which sets the TX interrupt flag            
            break;
            
        case UART_2:
            while(!U2STAbits.TRMT);
            uart->tx_length = 1;    // Set TX length
            uart->tx_buf[0] = data;// Fill buffer   
            uart->DMA_enabled = 0;      // Disable DMA for the next transfer
            IEC1bits.U2TXIE = 1;               
            break;
            
        case UART_3:
            while(!U3STAbits.TRMT);
            uart->tx_length = 1;    // Set TX length
            uart->tx_buf[0] = data;// Fill buffer  
            uart->DMA_enabled = 0;      // Disable DMA for the next transfer
            IEC3bits.U3TXIE = 1;      
            break;           
    }  
}  

//***********void UART_putc_ascii (STRUCT_UART *uart, uint8_t data)************//
//Description : Function converts byte to 2 corresponding ascii characters and 
//              send them through UART
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty
//
//Function prototype : void UART_putc_ascii (STRUCT_UART *uart, uint8_t data)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : uint8_t data         : byte of data
//
//Exit params        : None
//
//Function call      : UART_putc_ascii(&UART_x, 'A');
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putc_ascii (STRUCT_UART *uart, uint8_t data)
{
    uint8_t buf[2];
    hex_to_ascii(data, &buf[0], &buf[1]);   // Convert byte to ascii values
    uart->DMA_enabled = 0;              // Disable DMA for the next transfer
    UART_putbuf(uart, buf, 2);               // Send both values through UART
}

//********void UART_putstr (STRUCT_UART *uart, const char *string)*************//
//Description : Function sends a string of character through UART_x channel
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty              
//
//Function prototype : void UART_putstr (STRUCT_UART *uart, const char *string)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : const char *string   : string of character
//
//Exit params        : None
//
//Function call      : UART_putstr(&UART_x, "Sending command through UART_x");
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putstr(STRUCT_UART *uart, const char *string)
{
    uint16_t i = 0;
    uint16_t length = strlen(string);

    // Wait for previous transaction to be completed
    while (UART_tx_done(uart) != UART_TX_COMPLETE);
    
    // Saturate strlen to UART_MAX_TX define
    if (length > uart->tx_buf_length)
    {
        length = uart->tx_buf_length;
    }

    // Fill TX buffer
    for (i=0; i < length; i++)
    {
        uart->tx_buf[i] = *string++;   // Copy data into transmit buffer  
    }               
    uart->tx_length = length;           // Set the transmit length
    uart->tx_done = UART_TX_IDLE;       // Clear the TX done flag
    uart->DMA_enabled = 0;              // Disable DMA for the next transfer
    UART_send_tx_buffer(uart);
}

//********void UART_putstr (STRUCT_UART *uart, const char *string)*************//
//Description : Function sends a string of character through UART_x channel
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty              
//
//Function prototype : void UART_putstr (STRUCT_UART *uart, const char *string)
//
//Enter params       : STRUCT_UART *uart     : structure pointer type
//                   : const char *string   : string of character
//
//Exit params        : None
//
//Function call      : UART_putstr(&UART_x, "Sending command through UART_x");
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_putstr_dma (STRUCT_UART *uart, const char *string)
{
    uint16_t i = 0;
    uint16_t length = strlen(string);

    // Wait for previous transaction to be completed
    if (DMA_get_txfer_state(uart->DMA_tx_channel) == DMA_TXFER_DONE)    // If DMA channel is free, fill buffer and transmit
    {
        if (UART_get_trmt_state(uart) == 1)            // TRMT empty and ready to accept new data         
        {    
            // Saturate strlen to UART_MAX_TX define
            if (length > uart->tx_buf_length)
            {
                length = uart->tx_buf_length;
            }

            // Fill TX buffer
            for (i=0; i < length; i++)
            {
                if (uart->UART_channel == UART_1)
                {
#ifdef UART1_DMA_ENABLE
                    uart1_dma_tx_buf[i] = *string++; 
#endif
                }
                else if (uart->UART_channel == UART_2)
                {
#ifdef UART2_DMA_ENABLE
                    uart2_dma_tx_buf[i] = *string++; 
#endif
                }
                else if (uart->UART_channel == UART_3)
                {
#ifdef UART3_DMA_ENABLE
                    uart3_dma_tx_buf[i] = *string++;
#endif
                }           
                else
                    return 0;
            }
            
            switch (uart->UART_channel)
            {
                case UART_1:
#ifdef UART1_DMA_ENABLE
                    DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart1_dma_tx_buf);
                    DMA_set_txfer_length(uart->DMA_tx_channel, (volatile uint16_t)&uart1_dma_tx_buf, length);     // 0 = 1 txfer, so substract 1 
#endif
                    break;
                    
                case UART_2:
#ifdef UART2_DMA_ENABLE
                    DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart2_dma_tx_buf);
                    DMA_set_txfer_length(uart->DMA_tx_channel, (volatile uint16_t)&uart2_dma_tx_buf, length);     // 0 = 1 txfer, so substract 1 
#endif
                    break;
                    
                case UART_3:
#ifdef UART3_DMA_ENABLE
                    DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart3_dma_tx_buf);
                    DMA_set_txfer_length(uart->DMA_tx_channel, (volatile uint16_t)&uart3_dma_tx_buf, length);     // 0 = 1 txfer, so substract 1 
#endif
                    break;
                    
                default:
                    return 0;
                    break;
            }
            uart->DMA_enabled = 1;      // Enable DMA for the next transfer
            DMA_enable(uart->DMA_tx_channel);
            DMA_set_channel_req(uart->DMA_tx_channel); 
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

//******void UART_putbuf (uint8_t channel, uint8_t *buf, uint16_t length)******//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel
//              This function halts execution until TRMT bit of the selected 
//              UART channel is equal to "1" -> empty   
//
//Function prototype : void UART_putbuf (uint8_t channel, uint8_t *buf, uint16_t length)
//
//Enter params       : uint8_t channel : UART_x channel
//                   : uint8_t *buf : byte buffer
//                   : uint16_t length : buffer length in bytes
//
//Exit params        : None
//
//Function call      : UART_putbuf(UART_1, buf, 256);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_putbuf (STRUCT_UART *uart, uint8_t *buf, uint16_t length)
{
    uint16_t i = 0;
    
    // Wait for previous transaction to have completed
    while (UART_tx_done(uart) != UART_TX_COMPLETE);
    
    // Saturate write length
    if (length > uart->tx_buf_length)
    {
        length = uart->tx_buf_length;
    }
    
    // Fill transmit buffer, set transmit length and TX done flag to idle
    for (i=0; i < length; i++)
    {
        uart->tx_buf[i] = *buf++;    // Copy data into transmit buffer   
    }               
    uart->tx_length = length;        // Set the transmit length  
    uart->tx_done = UART_TX_IDLE;    // Clear the TX done flag
    uart->DMA_enabled = 0;          // Disable DMA for the next transfer
    UART_send_tx_buffer(uart);
}


//***uint8_t UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint8_t length)*****//
//Description : Function sends a buffer of data of specified length to UART_x
//              channel using DMA
//
//Function prototype : void UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint8_t length)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//                   : uint8_t *buf     : write buffer
//                   : uint8_t length   : write length in bytes
//
//Exit params        : uint8_t
//
//Function call      : UART_putbuf_dma(&UART_x, buf, length(buf));
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_putbuf_dma (STRUCT_UART *uart, uint8_t *buf, uint16_t length)
{
    uint16_t i = 0;               
    if (DMA_get_txfer_state(uart->DMA_tx_channel) == DMA_TXFER_DONE)    // If DMA channel is free, fill buffer and transmit
    {
        if (UART_get_trmt_state(uart) == 1)            // TRMT empty and ready to accept new data         
        {
            for (i=0; i < length; i++)
            {
                if (uart->UART_channel == UART_1)
                {
#ifdef UART1_DMA_ENABLE   
                    uart1_dma_tx_buf[i] = *buf++;
#endif
                }

                else if (uart->UART_channel == UART_2)
                {
#ifdef UART2_DMA_ENABLE
                    uart2_dma_tx_buf[i] = *buf++;                  
#endif
                }

                else if (uart->UART_channel == UART_3)
                {     
#ifdef UART3_DMA_ENABLE
                    uart3_dma_tx_buf[i] = *buf++;
#endif
                }                             
                else
                    return 0;
            }
            
            switch (uart->UART_channel)
            {
                case UART_1:
#ifdef UART1_DMA_ENABLE
                    DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart1_dma_tx_buf);
                    DMA_set_txfer_length(uart->DMA_tx_channel, (volatile uint16_t)&uart1_dma_tx_buf, length);     // 0 = 1 txfer, so substract 1 
#endif
                    break;
                    
                case UART_2:
#ifdef UART2_DMA_ENABLE
                    DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart2_dma_tx_buf);
                    DMA_set_txfer_length(uart->DMA_tx_channel, (volatile uint16_t)&uart2_dma_tx_buf, length);     // 0 = 1 txfer, so substract 1 
#endif
                    break;
                    
                case UART_3:
#ifdef UART3_DMA_ENABLE
                    DMA_set_peripheral_address(uart->DMA_tx_channel, DMA_ARRAY_SOURCE, (volatile uint16_t)&uart3_dma_tx_buf);
                    DMA_set_txfer_length(uart->DMA_tx_channel, (volatile uint16_t)&uart3_dma_tx_buf, length);     // 0 = 1 txfer, so substract 1 
#endif
                    break;
                    
                default:
                    return 0;
                    break;
            }
            uart->DMA_enabled = 1;      // Enable DMA for the next transfer
            DMA_enable(uart->DMA_tx_channel);
            DMA_set_channel_req(uart->DMA_tx_channel);          
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;       
}

//**void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint8_t length)**//
//Description : Function fills the TX buffer and set it's length in the struct
//
//Function prototype : void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint8_t length)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//                   : uint8_t *data    : write buffer
//                   : uint8_t length   : write length
//
//Exit params        : None
//
//Function call      : UART_fill_tx_buffer(&UART_x, buf, length(buf));
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_fill_tx_buffer (STRUCT_UART *uart, uint8_t *data, uint16_t length)
{
    uint16_t i = 0;
    
    // Wait for previous transaction to be complete
    while (UART_tx_done(uart) != UART_TX_COMPLETE);
    
    // Saturate write length
    if (length > uart->tx_buf_length)
    {
        length = uart->tx_buf_length;
    }
    
    for (i=0; i < length; i++)
    {
        uart->tx_buf[i] = *data++; // Fill the transmit buffer
    }           
    uart->tx_length = length;       // Write TX buffer length
    uart->tx_done = UART_TX_IDLE;   // Set transfer state to IDLE
}

//*****************void UART_send_tx_buffer (STRUCT_UART *uart)****************//
//Description : Used complementary to UART_fill_tx_buffer
//              Functions sets the str UART channel interrupt flag to start the
//              transmission of the frame written in UART_fill_tx_buffer
//
//Function prototype : void UART_send_tx_buffer (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : None
//
//Function call      : UART_send_tx_buffer(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_send_tx_buffer (STRUCT_UART *uart)
{
    switch(uart->UART_channel)
    {
        case UART_1:
            IEC0bits.U1TXIE = 1;    // Enable TX interrupt, which set the TX interrupt flag
            break;
            
        case UART_2:   
            IEC1bits.U2TXIE = 1;
            break;
            
        case UART_3:
            IEC3bits.U3TXIE = 1;
            break;
            
        default:
            break;
    }
}
//Exit params        : None
//
//Function call      : UART_clear_rx_buffer(&UART_x, 0);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void UART_clear_rx_buffer (STRUCT_UART *uart, uint8_t clr_byte)
{
    uint16_t i = 0;
    for (i=0; i < uart->rx_buf_length; i++)
    {
        uart->rx_buf[i] = clr_byte;
    }
    uart->rx_counter = 0;
}

//****************uint8_t UART_rx_done (STRUCT_UART *uart)*********************//
//Description : Function checks the str rx_done flag
//              If rx_done = 1, the reception of the frame is over, returns 1
//              If rx_done = 0, the reception of the frame is still in progress, returns 0
//
//Function prototype : uint8_t UART_rx_done (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : uint8_t
//
//Function call      : uint8_t = UART_rx_done(UART_1);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022  
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_rx_done (STRUCT_UART *uart)
{
    if (uart->rx_done == UART_RX_COMPLETE)
    {
        uart->rx_done = UART_RX_IDLE;
        return UART_RX_COMPLETE;
    }
    else
        return UART_RX_IDLE;
}

//*****************uint8_t UART_tx_done (STRUCT_UART *uart)********************//
//Description : Function checks the str UART channel transmit shift register empty bit TRMT
//              If TRMT = 1 (last transmission completed), returns 1
//              If TMRT = 0 (transmission in progress or queued), return 0
//
//Function prototype : uint8_t UART_tx_done (STRUCT_UART *uart)
//
//Enter params       : STRUCT_UART *uart : structure pointer type
//
//Exit params        : uint8_t
//
//Function call      : uint8_t = UART_tx_done(&UART_x);
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
uint8_t UART_tx_done (STRUCT_UART *uart)
{
    if (uart->tx_done == UART_TX_COMPLETE)
    {
        uart->tx_done = UART_TX_IDLE;
        return UART_TX_COMPLETE;  
    }
    else
        return UART_TX_IDLE;        
}

//**********************UART1 receive interrupt function**********************//
//Description : UART1 receive interrupt.
//
//Function prototype : _U1RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;      // clear RX interrupt flag
    uint8_t temp;
    if (UART_struct[UART_1].rx_counter < UART_struct[UART_1].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_1].rx_buf[UART_struct[UART_1].rx_counter++] = U1RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_1].rx_counter >= UART_struct[UART_1].rx_buf_length)          // All data received?
    {
        // Empty rxreg from any parasitic data
        if (!U1STAHbits.URXBE)
        {
            while (!U1STAHbits.URXBE)
            {
                temp = U1RXREG;
            }
        } 
        UART_struct[UART_1].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_1].rx_counter = 0;                                            // Clear the RX counter
    }
}

//**********************UART1 transmit interrupt function*********************//
//Description : UART1 transmit interrupt.
//
//Function prototype : _U1TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;                                // clear TX interrupt flag
    if (UART_struct[UART_1].DMA_enabled == 0)           // if DMA is disabled
    {
        if (UART_struct[UART_1].tx_length == 1)        // Single transmission
        {
            U1TXREG = UART_struct[UART_1].tx_buf[0]; // Transfer data to UART transmit buffer
            UART_struct[UART_1].tx_done = UART_TX_COMPLETE;          // Set the TX done flag
            IEC0bits.U1TXIE = 0;                           // Disable TX interrupt (no more data to send)
        }  

        if (UART_struct[UART_1].tx_length > 1)        // Multiple transmission
        {
            if (UART_struct[UART_1].tx_counter < UART_struct[UART_1].tx_length)         // At least 1 more byte to transfer 
            {
                U1TXREG = UART_struct[UART_1].tx_buf[UART_struct[UART_1].tx_counter++]; // Copy TX data to UART TX buffer            
                if (UART_struct[UART_1].tx_counter == UART_struct[UART_1].tx_length)    // More data to send?
                {
                    while(!U1STAbits.TRMT);
                    UART_struct[UART_1].tx_done = UART_TX_COMPLETE;                     // Set the TX done flag
                    UART_struct[UART_1].tx_counter = 0;                                 // Clear TX counter
                    IEC0bits.U1TXIE = 0;                                                // Disable TX interrupt (no more data to send)
                }               
            }           
        }
    }
}

//**********************UART2 receive interrupt function**********************//
//Description : UART2 receive interrupt.
//
//Function prototype : _U2RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;      // clear RX interrupt flag
    uint8_t temp=0;
    if (UART_struct[UART_2].rx_counter < UART_struct[UART_2].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_2].rx_buf[UART_struct[UART_2].rx_counter++] = U2RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_2].rx_counter >= UART_struct[UART_2].rx_buf_length)          // All data received?
    {       
        // Empty rxreg from any parasitic data
        if (!U2STAHbits.URXBE)
        {
            while (!U2STAHbits.URXBE)
            {
                temp = U2RXREG;
            }
        }                                                
        UART_struct[UART_2].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_2].rx_counter = 0;                                            // Clear the RX counter
    }  
}

//**********************UART2 transmit interrupt function*********************//
//Description : UART2 transmit interrupt.
//
//Function prototype : _U2TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U2TXInterrupt(void)
{
    IFS1bits.U2TXIF = 0;  
    if (UART_struct[UART_2].DMA_enabled == 0)           // if DMA is disabled
    {
        if (UART_struct[UART_2].tx_length == 1)
        {
            U2TXREG = UART_struct[UART_2].tx_buf[0];
            UART_struct[UART_2].tx_done = UART_TX_COMPLETE;       
            IEC1bits.U2TXIE = 0;                          
        }  

        if (UART_struct[UART_2].tx_length > 1)
        {
            if (UART_struct[UART_2].tx_counter < UART_struct[UART_2].tx_length)
            {
                U2TXREG = UART_struct[UART_2].tx_buf[UART_struct[UART_2].tx_counter++];         
                if (UART_struct[UART_2].tx_counter == UART_struct[UART_2].tx_length)
                {
                    while(!U2STAbits.TRMT);
                    UART_struct[UART_2].tx_done = UART_TX_COMPLETE;     
                    UART_struct[UART_2].tx_counter = 0;  
                    IEC1bits.U2TXIE = 0;
                }               
            }           
        } 
    }
}

//**********************UART3 receive interrupt function**********************//
//Description : UART3 receive interrupt.
//
//Function prototype : _U3RXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U3RXInterrupt(void)
{
    IFS3bits.U3RXIF = 0;      // clear RX interrupt flag
    uint8_t temp=0;
    if (UART_struct[UART_3].rx_counter < UART_struct[UART_3].rx_buf_length)           // Waiting for more data?
    {
        UART_struct[UART_3].rx_buf[UART_struct[UART_3].rx_counter++] = U3RXREG;  // Yes, copy it from the UxRXREG
    }

    if (UART_struct[UART_3].rx_counter >= UART_struct[UART_3].rx_buf_length)          // All data received?
    {
        // Empty rxreg from any parasitic data
        if (!U3STAHbits.URXBE)
        {
            while (!U3STAHbits.URXBE)
            {
                temp = U3RXREG;
            }
        } 
        UART_struct[UART_3].rx_done = UART_RX_COMPLETE;                                // Yes, set the RX done flag
        UART_struct[UART_3].rx_counter = 0;                                            // Clear the RX counter
    }  
}

//**********************UART3 transmit interrupt function*********************//
//Description : UART3 transmit interrupt.
//
//Function prototype : _U3TXInterrupt(void) 
//
//Enter params       : None
//
//Exit params        : None
//
// Intellitrol           MPLab X v5.45            XC16 v1.61          01/02/2022 
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff/dspeak
//****************************************************************************//
void __attribute__((__interrupt__, auto_psv)) _U3TXInterrupt(void)
{
    IFS3bits.U3TXIF = 0;      // clear TX interrupt flag
    if (UART_struct[UART_3].DMA_enabled == 0)           // if DMA is disabled
    {
        if (UART_struct[UART_3].tx_length == 1)
        {
            U3TXREG = UART_struct[UART_3].tx_buf[0]; 
            UART_struct[UART_3].tx_done = UART_TX_COMPLETE;          
            IEC3bits.U3TXIE = 0;                        
        }  

        if (UART_struct[UART_3].tx_length > 1)
        {
            if (UART_struct[UART_3].tx_counter < UART_struct[UART_3].tx_length)
            {
                U3TXREG = UART_struct[UART_3].tx_buf[UART_struct[UART_3].tx_counter++];             
                if (UART_struct[UART_3].tx_counter == UART_struct[UART_3].tx_length)
                {
                    while(!U3STAbits.TRMT);
                    UART_struct[UART_3].tx_done = UART_TX_COMPLETE;     
                    UART_struct[UART_3].tx_counter = 0;   
                    IEC3bits.U3TXIE = 0;                    
                }               
            }           
        }   
    }
}