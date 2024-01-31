//***************************************************************************//
// File      : I2C.c
//
// Functions :  void I2C_init (uint8_t port, uint8_t mode, uint8_t address);
//              void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length);
//              void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length);
//              void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length);
//              uint8_t I2C_wait (uint8_t port);
//              uint8_t I2C_read_state (uint8_t port);
//              uint8_t * I2C_get_rx_buffer (uint8_t port);
//              uint8_t I2C_rx_done (uint8_t port);
//              void I2C_change_address (uint8_t adr);
//              void I2C_clear_rx_buffer (uint8_t port);
//
// Includes  :  i2c.h
//           
// Purpose   :  I2C driver for the dsPIC33CK family
//              2x native channels
//              I2C_2 : MikroBus1 I2C port
//              I2C_3 : MikroBus2 I2C port
//
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#include "i2c.h"

STRUCT_I2C i2c_struct[I2C_QTY];

//void I2C_init (uint8_t port, uint8_t mode, uint8_t address)//
//Description : Function initializes the i2c module in slave or master mode
//
//Function prototype : void I2C_init (uint8_t port, uint8_t mode, uint8_t address)
//
//Enter params       : uint8_t port : Specify the I2C port to initialize
//                   : uint8_t mode : Specify the working mode for the I2C port, slave or master
//                   : uint8_t address : Specify the slave address if using the slave mode, otherwise put 0
//
//Exit params        : None
//
//Function call      : I2C_init(I2C_2, I2C_mode_slave, 0x22);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t I2C_init (STRUCT_I2C *i2c, uint8_t port, uint16_t freq, uint8_t mode, uint8_t address)
{   
    if (port == I2C_2)
    {   
        // Initialize I2C port struct variables
        i2c->I2C_channel = I2C_2;
        i2c->i2c_tx_counter = 0;                    // Reset state machine variables
        i2c->i2c_rx_counter = 0;                    //
        i2c->i2c_write_length = 0;                  //
        i2c->i2c_read_length = 0;                   //
        i2c->i2c_int_counter = 0;                   //
        i2c->i2c_done = 1;                          // Bus is free  
        i2c->ack_state = 1;                         // NACK by default
        if (mode == I2C_mode_slave)
        {
            // Module register initializations
            IEC2bits.SI2C2IE = 0;       // Disable I2C slave interrupt    
            I2C2CONLbits.I2CEN = 0;      // Disable module if it was in use
            I2C2CONLbits.A10M = 0;       // 7 bit address
            I2C2ADD = address >> 1;     // Write I2C address
            I2C2CONLbits.DISSLW = 1;     // Disable slew rate control
            I2C2CONLbits.STREN = 1;      // Enable user software clock stretching

            // Module interrupt initialization and enable
            IFS2bits.SI2C2IF = 0;       // Clear slave I2C interrupt flag  
            IEC2bits.SI2C2IE = 1;       // Enable I2C slave interrupt    
            I2C2CONLbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function
            
            i2c->read_mode = I2C_READ_MODE_RESTART; // Default read mode
        }
        
        if (mode == I2C_mode_master)
        {
            I2C2CONLbits.I2CEN = 0;      // Disable module if it was in use
            I2C2CONLbits.DISSLW = 1;     // Disable slew rate control 
            I2C2BRG = freq;             // Set I2C1 frequency     
            IFS2bits.MI2C2IF = 0;       // Clear master I2C interrupt flag  
            IEC2bits.MI2C2IE = 0;       // Disable I2C master interrupt    
            I2C2CONLbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function           
        }        
    }
    
    else if (port == I2C_3)
    {        
        // Initialize I2C port struct variables
        i2c->I2C_channel = I2C_3;
        i2c->i2c_tx_counter = 0;                    // Reset state machine variables
        i2c->i2c_rx_counter = 0;                    //
        i2c->i2c_write_length = 0;                  //
        i2c->i2c_read_length = 0;                   //
        i2c->i2c_int_counter = 0;                   //
        i2c->i2c_done = 1;                          // Bus is free  
        i2c->ack_state = 1;                         // NACK by default        
        if (mode == I2C_mode_slave)
        {
            // Module register initializations
            IEC8bits.SI2C3IE = 0;       // Disable I2C slave interrupt    
            I2C3CONLbits.I2CEN = 0;      // Disable module if it was in use
            I2C3CONLbits.DISSLW = 1;     // Disable slew rate control    
            I2C3CONLbits.A10M = 0;       // 7 bit address
            I2C3ADD = address >> 1;     // Write I2C address
            I2C3CONLbits.DISSLW = 1;     // Disable slew rate control
            I2C3CONLbits.STREN = 1;      // Enable user software clock stretching

            // Module interrupt initialization and enable
            IFS8bits.SI2C3IF = 0;       // Clear slave I2C interrupt flag  
            IPC35bits.SI2C3IP = 4;      // Set default priority to 4 
            IEC8bits.SI2C3IE = 1;       // Enable I2C slave interrupt    
            I2C3CONLbits.I2CEN = 1;      // Enable I2C module & set pins to I2C function
            
            i2c->read_mode = I2C_READ_MODE_RESTART; // Default read mode
        }
                
        if (mode == I2C_mode_master)
        {
            I2C3CONLbits.I2CEN = 0;     // Disable module if it was in use
            I2C3CONLbits.DISSLW = 1;    // Disable slew rate control      
            I2C3BRG = freq;             // Set I2C2 brg    
            IFS8bits.MI2C3IF = 0;       // Clear master I2C interrupt flag  
            IPC35bits.MI2C3IP = 4;       // Set default priority to 4 
            IEC8bits.MI2C3IE = 0;       // Disable I2C master interrupt    
            I2C3CONLbits.I2CEN = 1;     // Enable I2C module & set pins to I2C function           
        }
    }
    return 0;
}
//***********************uint8_t I2C_done (uint8_t port)**********************//
//Description : Function returns I2C bus state
//
//Function prototype : uint8_t I2C_done (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : uint8_t : bus state
//
//Function call      : int_state = I2C_done(_2C_3);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t I2C_done (STRUCT_I2C *i2c)
{
    if (i2c->I2C_channel == I2C_2)
    {
        if ((i2c->i2c_done == 1) && (IEC2bits.MI2C2IE == 0))      // A transaction was successfully managed
        {
            i2c->i2c_done = 0;  // Clear flag
            return 1;
        }
        else
            return 0;
    }
    if (i2c->I2C_channel == I2C_3)
    {
        if ((i2c->i2c_done == 1) && (IEC8bits.MI2C3IE == 0))      // A transaction was successfully managed
        {
            i2c->i2c_done = 0;  // Clear flag
            return 1;
        }
        else
            return 0;        
    }
    else return 0;
}

//************uint8_t I2C_rx_done (uint8_t port)******************//
//Description : Function returns I2C bus state
//
//Function prototype : uint8_t I2C_rx_done (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : uint8_t : bus state
//
//Function call      : int_state = I2C_rx_done(_2C_3);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t I2C_rx_done (STRUCT_I2C *i2c)
{
    if (i2c->I2C_channel == I2C_2)
    {
        if ((i2c->i2c_rx_done == 1) && (IEC2bits.MI2C2IE == 0))      // A transaction was successfully managed
        {
            i2c->i2c_rx_done = 0;  // Clear flag
            return 1;
        }
        else
            return 0;
    }
    if (i2c->I2C_channel == I2C_3)
    {
        if ((i2c->i2c_rx_done == 1) && (IEC8bits.MI2C3IE == 0))      // A transaction was successfully managed
        {
            i2c->i2c_rx_done = 0;  // Clear flag
            return 1;
        }
        else
            return 0;        
    }
    else return 0;
}

//****************void I2C_clear_rx_buffer (uint8_t port)******************//
//Description : Function clears I2C receive buffer from specified I2C port
//
//Function prototype : void I2C_clear_rx_buffer (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : None
//
//Function call      : I2C_clear_rx_buffer(I2C_2);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_clear_rx_buffer (STRUCT_I2C *i2c)
{
    uint8_t i = 0;
    for (i=0; i<i2c->i2c_read_length; i++)
    {
        i2c->i2c_rx_data[i] = 0;
    }   
}

//void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length)//
//Description : Function fills the I2C transmit buffer on SLAVE ONLY operations 
//
//Function prototype : void I2C_fill_transmit_buffer (uint8_t port, uint8_t *ptr, uint8_t length)
//
//Enter params       : uint8_t port : I2C port number
//                   : uint8_t *ptr : pointer to data array
//                   : uint8_t length : data length in bytes
//
//Exit params        : None
//
//Function call      : I2C_fill_transmit_buffer(I2C_2, data, 5);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_fill_transmit_buffer (STRUCT_I2C *i2c, uint8_t *ptr, uint8_t length)
{
    // Use this function on slave-only transmission
    uint8_t i = 0;
    if (length <= i2c->i2c_write_length)    // - 1 since the 1st byte is always the address (slave-only)
    {
        ptr++;                              // skip the 1st byte since its the address (slave-only)
        for (i=0; i< (length - 1); i++)
        {
            i2c->i2c_tx_data[i] = *ptr;     // Fill the transmit buffer
            ptr++;
        }   
    }
}

//void I2C_master_write (uint8_t port, uint8_t *data, uint8_t length)//
//Description : Function writes an I2C message on the specified port
//              This is an I2C MASTER-only function
//              This is a blocking function
//
//Enter params       : uint8_t port : I2C port number
//                   : uint8_t *data : pointer to data array
//                   : uint8_t length : data length in bytes
//
//Exit params        : None
//
//Function call      : I2C_master_write(_2C_3, data, 6);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_master_write (STRUCT_I2C *i2c, uint8_t *data, uint16_t length)
{
    uint16_t i = 0;
    while(I2C_wait(i2c));      // Check for disabled interrupt
    for (i=0; i<length; i++)
    {
        i2c->i2c_tx_data[i] = *data;    // copy data to struct array
        data++;
    }
    i2c->i2c_write_length = length;     // Set message length 
    i2c->i2c_message_mode = I2C_WRITE;  // Set message type to write
    i2c->i2c_int_counter = 0;           // Reset state machine variables
    i2c->i2c_tx_counter = 0;            //
    i2c->i2c_done = 0;                  // busy i2c flag set
    
    if (i2c->I2C_channel == I2C_2)
    {
        IEC2bits.MI2C2IE = 1;                           // Enable I2C master interrupt 
        I2C2CONLbits.SEN = 1;                            // Start I2C sequence
    }
    
    if (i2c->I2C_channel == I2C_3)
    {
        IEC8bits.MI2C3IE = 1;                           // Enable I2C master interrupt 
        I2C3CONLbits.SEN = 1;                            // Start I2C sequence        
    }
}

//void I2C_master_read (uint8_t port, uint8_t *data, uint8_t w_length, uint8_t r_length)//
//Description : Function reads an I2C message on the specified port
//              This is an I2C MASTER-only function
//              This is a blocking function
//
//Enter params       : uint8_t port : I2C port number
//                   : uint8_t *data : pointer to data array
//                   : uint8_t w_length : write length in bytes
//                   : uint8_t r_length : read length in bytes
//
//Exit params        : None
//
//Function call      : I2C_read(I2C_3, data, 6);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
void I2C_master_read (STRUCT_I2C *i2c, uint8_t mode, uint8_t *data, uint16_t w_length, uint16_t r_length)
{
    uint16_t i = 0;
    while(I2C_wait(i2c));                          // Wait until previous transaction is over
    for (i=0; i<w_length; i++)
    {
        i2c->i2c_tx_data[i] = *data;// copy data to struct table
        data++;
    }
    i2c->i2c_message_mode = I2C_READ;   //  
    i2c->read_mode = mode;
    i2c->i2c_write_length = w_length;   // registers to write before reading
    i2c->i2c_read_length = r_length;    // Read adr + number of bytes to read
    i2c->i2c_int_counter = 0;           // 
    i2c->i2c_tx_counter = 0;            //
    i2c->i2c_rx_counter = 0;            //
    i2c->i2c_done = 0;                  // I2C transaction has begun
    i2c->i2c_rx_done = 0;
    if (i2c->I2C_channel == I2C_2)
    {
        IEC2bits.MI2C2IE = 1;                     // Enable I2C master interrupt 
        I2C2CONLbits.SEN = 1;                      // Start I2C sequence
    }
    
    if (i2c->I2C_channel == I2C_3)
    {
        IEC8bits.MI2C3IE = 1;                       // Enable I2C master interrupt 
        I2C3CONLbits.SEN = 1;                        // Start I2C sequence        
    }
    
    //while(I2C_wait(i2c));                          // Wait until previous transaction is over   
}

//************uint8_t I2C_wait (uint8_t port)*********************//
//Description : Wait for I2C interrupt to flag down (bus unused))
//
//Function prototype : uint8_t I2C_wait (uint8_t port)
//
//Enter params       : uint8_t port : I2C port number
//
//Exit params        : uint8_t : bus state
//
//Function call      : int_state = I2C_wait(I2C_3);
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t I2C_wait (STRUCT_I2C *i2c)
{
    if (i2c->I2C_channel == I2C_2)
    {
        return IEC2bits.MI2C2IE;    // Only used in master mode
    }
    
    else if (i2c->I2C_channel == I2C_3)
    {
        return IEC8bits.MI2C3IE;    // Only used in master mode
    }
    
    else return 0;
}

//************uint8_t * I2C_get_rx_buffer (uint8_t port)***************//
//Description : Return I2C array address (1st element)
//
//Function prototype : uint8_t * I2C_get_rx_buffer (void)
//
//Enter params       : None
//
//Exit params        : uint8_t * : address of 1st element
//
//Function call      : ptr = I2C_get_rx_buffer();
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020 
//****************************************************************************//
uint8_t * I2C_get_rx_buffer (STRUCT_I2C *i2c)
{
    return &i2c->i2c_rx_data[0];
}

uint8_t * I2C_get_rx_buffer_index (STRUCT_I2C *i2c, uint16_t index)
{
    return &i2c->i2c_rx_data[index];
}

uint8_t I2C_get_ack_state (STRUCT_I2C *i2c)
{
    return i2c->ack_state;    
}

// I2C2 slave interrupt routine 
// Uncomment this section if I2C1 is used as a slave
void __attribute__((__interrupt__, no_auto_psv)) _SI2C2Interrupt(void)
{   
    uint8_t temp = 0;
    // Address + W received, write data to slave
    if ((!I2C2STATbits.D_A) && (!I2C2STATbits.R_W))
    {
        i2c_struct[I2C_2].i2c_rx_data[i2c_struct[I2C_2].i2c_rx_counter] = I2C2RCV; // Receive address
        while(I2C2STATbits.RBF);    // Wait until receive buffer is empty
        i2c_struct[I2C_2].i2c_rx_counter++;// Increment data counter
        I2C2CONLbits.SCLREL = 1;     // Release the serial clock     
    }

    // Address + R received, read data from slave
    if ((!I2C2STATbits.D_A) && (I2C2STATbits.R_W))
    {
        temp = I2C2RCV;
        while(I2C2STATbits.RBF);
        while(I2C2STATbits.TBF);
        I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter];
        i2c_struct[I2C_2].i2c_tx_counter++;  
        I2C2CONLbits.SCLREL = 1; // release the SCL line  
    }
    
    //Should send more data to master as long as he acks
    if ((I2C2STATbits.D_A) && (I2C2STATbits.R_W))
    {
        // The master responded with an ACK, transfer more data
        if (!I2C2STATbits.ACKSTAT)
        {
            if (i2c_struct[I2C_2].i2c_tx_counter < i2c_struct[I2C_2].i2c_write_length)
            {
                temp = I2C2RCV;
                while(I2C2STATbits.RBF);
                while(I2C2STATbits.TBF);
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter];
                i2c_struct[I2C_2].i2c_tx_counter++; 
                I2C2CONLbits.SCLREL = 1; // release the SCL line
            }
        } 
        else
        {
            i2c_struct[I2C_2].i2c_tx_counter = 0;
            i2c_struct[I2C_2].i2c_done = 1;
        }
    }        
       
    // Receiving data from master (data, adress is in write mode)
    if ((I2C2STATbits.D_A) && (!I2C2STATbits.R_W))
    {
        // Receive more data from master
        if (i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
        {
            i2c_struct[I2C_2].i2c_rx_data[i2c_struct[I2C_2].i2c_rx_counter] = I2C2RCV;
            while(I2C2STATbits.RBF);
            i2c_struct[I2C_2].i2c_rx_counter++;
            I2C2CONLbits.SCLREL = 1; // release the SCL line                      
        }
        
        if (i2c_struct[I2C_2].i2c_rx_counter == i2c_struct[I2C_2].i2c_read_length)
        {
            i2c_struct[I2C_2].i2c_done = 1;
            i2c_struct[I2C_2].i2c_rx_counter = 0;
        }
    } 
    IFS2bits.SI2C2IF = 0;    
}

// I2C2 master interrupt routine 
// Uncomment these lines if the I2C2 is used as master
void __attribute__((__interrupt__, no_auto_psv)) _MI2C2Interrupt(void)
{
    static uint8_t ack_adr = 0;    
    if (i2c_struct[I2C_2].i2c_message_mode == I2C_WRITE) // write
    {
        if (i2c_struct[I2C_2].i2c_int_counter == 0)
        {
            IFS2bits.MI2C2IF = 0;   // Lower interrupt flag 
            ack_adr = 0;            // ack address flag low
            I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter]; //send adr
            i2c_struct[I2C_2].i2c_int_counter++;
            i2c_struct[I2C_2].i2c_tx_counter++;  
        }            

        else if (i2c_struct[I2C_2].i2c_int_counter == 1) // send data
        {
            // Do once, scan ack state form when the address byte was sent, see if the device responded
            if (ack_adr == 0)
            {
                ack_adr = 1;
                i2c_struct[I2C_2].ack_state = I2C2STATbits.ACKSTAT;
            }
            
            if (i2c_struct[I2C_2].i2c_tx_counter < i2c_struct[I2C_2].i2c_write_length)//send data
            {  
                IFS2bits.MI2C2IF = 0;   // Lower interrupt flag  
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter];
                i2c_struct[I2C_2].i2c_tx_counter++;
            }

            else //all the data was transmitted
            {
                IFS2bits.MI2C2IF = 0;                           // Lower interrupt flag 
                i2c_struct[I2C_2].i2c_int_counter++;     
                I2C2CONLbits.PEN = 1;                            //enable stop sequence
            }
        }    

        else if (i2c_struct[I2C_2].i2c_int_counter == 2)   //end transmission 
        {
            i2c_struct[I2C_2].i2c_int_counter = 0;         // Reset interrupt counter
            i2c_struct[I2C_2].i2c_tx_counter = 0;          // Reset transmit counter
            i2c_struct[I2C_2].i2c_done = 1;                // Bus is free, transaction over
            ack_adr = 0;
            IEC2bits.MI2C2IE = 0;                               // Disable SSP interrupt
            IFS2bits.MI2C2IF = 0;                               // Lower interrupt flag
        }
    }   // End of I2C_write interrupt
    
    else if (i2c_struct[I2C_2].i2c_message_mode == I2C_READ) // read
    {       
        // I2C read mode with restart usually implies the host writes to the slave
        // to set an address pointer, then the host issues a restart, followed by ADR + 1
        if (i2c_struct[I2C_2].read_mode == I2C_READ_MODE_RESTART)
        {
            if (i2c_struct[I2C_2].i2c_int_counter == 0)    // send adr 
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter];
                i2c_struct[I2C_2].i2c_int_counter++;
                i2c_struct[I2C_2].i2c_tx_counter++;
            }    

            else if (i2c_struct[I2C_2].i2c_int_counter == 1) //send register to read
            {
                IFS2bits.MI2C2IF = 0; 
                if (i2c_struct[I2C_2].i2c_tx_counter < i2c_struct[I2C_2].i2c_write_length)
                {
                    I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter];
                    i2c_struct[I2C_2].i2c_tx_counter++;
                }

                else   
                {
                    I2C2CONLbits.RSEN = 1; //enable restart
                    i2c_struct[I2C_2].i2c_int_counter++;
                }   
            }

            // Send read address
            else if (i2c_struct[I2C_2].i2c_int_counter == 2)
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_rx_counter] + 1; //send adr + 1
                i2c_struct[I2C_2].i2c_int_counter++;             
            }

            // Enable data read on I2C bus
            else if (i2c_struct[I2C_2].i2c_int_counter == 3)
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2CONLbits.RCEN = 1;
                i2c_struct[I2C_2].i2c_int_counter++; 
            }    

            // Read data from i2c slave
            else if (i2c_struct[I2C_2].i2c_int_counter == 4)
            {
                IFS2bits.MI2C2IF = 0; 
                if (i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
                {   
                    // Get byte and store in rx buffer
                    i2c_struct[I2C_2].i2c_rx_data[i2c_struct[I2C_2].i2c_rx_counter] = I2C2RCV;

                    // See if we wish to receive more data as set in i2c_read_length field
                    if (++i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
                    {          
                        I2C2CONLbits.ACKDT = 0; //send ack
                        I2C2CONLbits.ACKEN = 1; //continue data reception
                        i2c_struct[I2C_2].i2c_int_counter = 3;
                    }   

                    // No more data to receive, send NACK to slave device
                    else
                    {
                        I2C2CONLbits.ACKDT = 1; //send nack
                        I2C2CONLbits.ACKEN = 1;
                    }   
                } 

                // Stop I2C transaction
                else
                {
                    I2C2CONLbits.PEN = 1;
                    i2c_struct[I2C_2].i2c_int_counter++; 
                }            
            }

            else if (i2c_struct[I2C_2].i2c_int_counter == 5)
            {
                i2c_struct[I2C_2].i2c_int_counter = 0;
                i2c_struct[I2C_2].i2c_rx_counter = 0;
                i2c_struct[I2C_2].i2c_tx_counter = 0;
                i2c_struct[I2C_2].i2c_done = 1;
                i2c_struct[I2C_2].i2c_rx_done = 1;
                IEC2bits.MI2C2IE = 0;
                IFS2bits.MI2C2IF = 0;
            } 
        }
        
        if (i2c_struct[I2C_2].read_mode == I2C_READ_MODE_DIRECT)
        {   
            // Send read address
            if (i2c_struct[I2C_2].i2c_int_counter == 0)
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter] + 1; //send adr + 1
                i2c_struct[I2C_2].i2c_tx_counter++;
                i2c_struct[I2C_2].i2c_int_counter++;             
            }

            // Enable data read on I2C bus
            else if (i2c_struct[I2C_2].i2c_int_counter == 1)
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2CONLbits.RCEN = 1;
                i2c_struct[I2C_2].i2c_int_counter++; 
            }    

            // Read data from i2c slave
            else if (i2c_struct[I2C_2].i2c_int_counter == 2)
            {
                IFS2bits.MI2C2IF = 0; 
                if (i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
                {   
                    // Get byte and store in rx buffer
                    i2c_struct[I2C_2].i2c_rx_data[i2c_struct[I2C_2].i2c_rx_counter] = I2C2RCV;

                    // See if we wish to receive more data as set in i2c_read_length field
                    if (++i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
                    {          
                        I2C2CONLbits.ACKDT = 0; //send ack
                        I2C2CONLbits.ACKEN = 1; //continue data reception
                        i2c_struct[I2C_2].i2c_int_counter = 1;
                    }   

                    // No more data to receive, send NACK to slave device
                    else
                    {
                        I2C2CONLbits.ACKDT = 1; //send nack
                        I2C2CONLbits.ACKEN = 1;
                    }   
                } 

                // Stop I2C transaction
                else
                {
                    I2C2CONLbits.PEN = 1;
                    i2c_struct[I2C_2].i2c_int_counter++; 
                }            
            }

            else if (i2c_struct[I2C_2].i2c_int_counter == 3)
            {
                i2c_struct[I2C_2].i2c_int_counter = 0;
                i2c_struct[I2C_2].i2c_rx_counter = 0;
                i2c_struct[I2C_2].i2c_tx_counter = 0;
                i2c_struct[I2C_2].i2c_done = 1;
                i2c_struct[I2C_2].i2c_rx_done = 1;
                IEC2bits.MI2C2IE = 0;
                IFS2bits.MI2C2IF = 0;
            } 
        } 
        
        if (i2c_struct[I2C_2].read_mode == I2C_READ_MODE_STOP_START)
        {
            if (i2c_struct[I2C_2].i2c_int_counter == 0)
            {
                IFS2bits.MI2C2IF = 0;   // Lower interrupt flag 
                ack_adr = 0;            // ack address flag low
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter]; //send adr
                i2c_struct[I2C_2].i2c_int_counter++;
                i2c_struct[I2C_2].i2c_tx_counter++;  
            }            

            else if (i2c_struct[I2C_2].i2c_int_counter == 1) // send data
            {
                // Do once, scan ack state form when the address byte was sent, see if the device responded
                if (ack_adr == 0)
                {
                    ack_adr = 1;
                    i2c_struct[I2C_2].ack_state = I2C2STATbits.ACKSTAT;
                }

                if (i2c_struct[I2C_2].i2c_tx_counter < i2c_struct[I2C_2].i2c_write_length)//send data
                {  
                    IFS2bits.MI2C2IF = 0;   // Lower interrupt flag  
                    I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter];
                    i2c_struct[I2C_2].i2c_tx_counter++;
                }

                else //all the data was transmitted
                {
                    IFS2bits.MI2C2IF = 0;                           // Lower interrupt flag 
                    I2C2CONLbits.PEN = 1;                            //enable stop sequence
                    i2c_struct[I2C_2].i2c_int_counter++;     
                }
            }
            
            else if (i2c_struct[I2C_2].i2c_int_counter == 2)
            {
                IFS2bits.MI2C2IF = 0;  
                I2C2CONLbits.SEN = 1;                        // Start I2C sequence 
                i2c_struct[I2C_2].i2c_int_counter++; 
            }
            
            // Send read address
            else if (i2c_struct[I2C_2].i2c_int_counter == 3)
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2TRN = i2c_struct[I2C_2].i2c_tx_data[i2c_struct[I2C_2].i2c_tx_counter] + 1; //send adr + 1
                i2c_struct[I2C_2].i2c_tx_counter++;
                i2c_struct[I2C_2].i2c_int_counter++;             
            }

            // Enable data read on I2C bus
            else if (i2c_struct[I2C_2].i2c_int_counter == 4)
            {
                IFS2bits.MI2C2IF = 0; 
                I2C2CONLbits.RCEN = 1;
                i2c_struct[I2C_2].i2c_int_counter++; 
            }    

            // Read data from i2c slave
            else if (i2c_struct[I2C_2].i2c_int_counter == 5)
            {
                IFS2bits.MI2C2IF = 0; 
                if (i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
                {   
                    // Get byte and store in rx buffer
                    i2c_struct[I2C_2].i2c_rx_data[i2c_struct[I2C_2].i2c_rx_counter] = I2C2RCV;

                    // See if we wish to receive more data as set in i2c_read_length field
                    if (++i2c_struct[I2C_2].i2c_rx_counter < i2c_struct[I2C_2].i2c_read_length)
                    {          
                        I2C2CONLbits.ACKDT = 0; //send ack
                        I2C2CONLbits.ACKEN = 1; //continue data reception
                        i2c_struct[I2C_2].i2c_int_counter = 4;
                    }   

                    // No more data to receive, send NACK to slave device
                    else
                    {
                        I2C2CONLbits.ACKDT = 1; //send nack
                        I2C2CONLbits.ACKEN = 1;
                    }   
                } 

                // Stop I2C transaction
                else
                {
                    I2C2CONLbits.PEN = 1;
                    i2c_struct[I2C_2].i2c_int_counter++; 
                }            
            }

            else if (i2c_struct[I2C_2].i2c_int_counter == 6)
            {
                i2c_struct[I2C_2].i2c_int_counter = 0;
                i2c_struct[I2C_2].i2c_rx_counter = 0;
                i2c_struct[I2C_2].i2c_tx_counter = 0;
                i2c_struct[I2C_2].i2c_done = 1;
                i2c_struct[I2C_2].i2c_rx_done = 1;
                IEC2bits.MI2C2IE = 0;
                IFS2bits.MI2C2IF = 0;
            } 
        }        
    }
}

// I2C3 master interrupt routine 
// Uncomment these lines if the I2C2 is used as master
void __attribute__((__interrupt__, no_auto_psv)) _MI2C3Interrupt(void)
{
    static uint8_t ack_adr = 0;  
    if (i2c_struct[I2C_3].i2c_message_mode == I2C_WRITE) // write
    {
        if (i2c_struct[I2C_3].i2c_int_counter == 0)
        {
            IFS8bits.MI2C3IF = 0;   // Lower interrupt flag 
            I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter]; //send adr
            i2c_struct[I2C_3].i2c_int_counter++;
            i2c_struct[I2C_3].i2c_tx_counter++;  
        }            

        else if (i2c_struct[I2C_3].i2c_int_counter == 1) // send data
        {
            //i2c_struct[I2C_3].ack_state = I2C_get_ack_state(i2c_struct[I2C_3]);
            if (i2c_struct[I2C_3].i2c_tx_counter < i2c_struct[I2C_3].i2c_write_length)//send data
            {  
                IFS8bits.MI2C3IF = 0;   // Lower interrupt flag  
                I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter];
                i2c_struct[I2C_3].i2c_tx_counter++;
            }

            else //all the data was transmitted
            {
                IFS8bits.MI2C3IF = 0;                           // Lower interrupt flag 
                i2c_struct[I2C_3].i2c_int_counter++;     
                I2C3CONLbits.PEN = 1;                            //enable stop sequence
            }
        }    

        else if (i2c_struct[I2C_3].i2c_int_counter == 2)   //end transmission 
        {
            i2c_struct[I2C_3].i2c_int_counter = 0;         // Reset interrupt counter
            i2c_struct[I2C_3].i2c_tx_counter = 0;          // Reset transmit counter
            i2c_struct[I2C_3].i2c_done = 1;                // Bus is free, transaction over
            IEC8bits.MI2C3IE = 0;                               // Disable SSP interrupt
            IFS8bits.MI2C3IF = 0;                               // Lower interrupt flag
        }
    }   // End of I2C_write interrupt
    
    else if (i2c_struct[I2C_3].i2c_message_mode == I2C_READ) // read
    {       
        // I2C read mode with restart usually implies the host writes to the slave
        // to set an address pointer, then the host issues a restart, followed by ADR + 1
        if (i2c_struct[I2C_3].read_mode == I2C_READ_MODE_RESTART)
        {
            if (i2c_struct[I2C_3].i2c_int_counter == 0)    // send adr 
            {
                IFS8bits.MI2C3IF = 0; 
                I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter];
                i2c_struct[I2C_3].i2c_int_counter++;
                i2c_struct[I2C_3].i2c_tx_counter++;
            }    

            else if (i2c_struct[I2C_3].i2c_int_counter == 1) //send register to read
            {
                IFS8bits.MI2C3IF = 0; 
                if (i2c_struct[I2C_3].i2c_tx_counter < i2c_struct[I2C_3].i2c_write_length)
                {
                    I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter];
                    i2c_struct[I2C_3].i2c_tx_counter++;
                }

                else   
                {
                    I2C3CONLbits.RSEN = 1; //enable restart
                    i2c_struct[I2C_3].i2c_int_counter++;
                }   
            }

            // Send read address
            else if (i2c_struct[I2C_3].i2c_int_counter == 2)
            {
                IFS8bits.MI2C3IF = 0; 
                I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_rx_counter] + 1; //send adr + 1
                i2c_struct[I2C_3].i2c_int_counter++;             
            }

            // Enable data read on I2C bus
            else if (i2c_struct[I2C_3].i2c_int_counter == 3)
            {
                IFS8bits.MI2C3IF = 0; 
                I2C3CONLbits.RCEN = 1;
                i2c_struct[I2C_3].i2c_int_counter++; 
            }    

            // Read data from i2c slave
            else if (i2c_struct[I2C_3].i2c_int_counter == 4)
            {
                IFS8bits.MI2C3IF = 0; 
                if (i2c_struct[I2C_3].i2c_rx_counter < i2c_struct[I2C_3].i2c_read_length)
                {   
                    // Get byte and store in rx buffer
                    i2c_struct[I2C_3].i2c_rx_data[i2c_struct[I2C_3].i2c_rx_counter] = I2C3RCV;

                    // See if we wish to receive more data as set in i2c_read_length field
                    if (++i2c_struct[I2C_3].i2c_rx_counter < i2c_struct[I2C_3].i2c_read_length)
                    {          
                        I2C3CONLbits.ACKDT = 0; //send ack
                        I2C3CONLbits.ACKEN = 1; //continue data reception
                        i2c_struct[I2C_3].i2c_int_counter = 3;
                    }   

                    // No more data to receive, send NACK to slave device
                    else
                    {
                        I2C3CONLbits.ACKDT = 1; //send nack
                        I2C3CONLbits.ACKEN = 1;
                    }   
                } 

                // Stop I2C transaction
                else
                {
                    I2C3CONLbits.PEN = 1;
                    i2c_struct[I2C_3].i2c_int_counter++; 
                }            
            }

            else if (i2c_struct[I2C_3].i2c_int_counter == 5)
            {
                i2c_struct[I2C_3].i2c_int_counter = 0;
                i2c_struct[I2C_3].i2c_rx_counter = 0;
                i2c_struct[I2C_3].i2c_tx_counter = 0;
                i2c_struct[I2C_3].i2c_done = 1;
                i2c_struct[I2C_3].i2c_rx_done = 1;
                IEC8bits.MI2C3IE = 0;
                IFS8bits.MI2C3IF = 0;
            } 
        }
        
        if (i2c_struct[I2C_3].read_mode == I2C_READ_MODE_DIRECT)
        {   
            // Send read address
            if (i2c_struct[I2C_3].i2c_int_counter == 0)
            {
                IFS8bits.MI2C3IF = 0; 
                I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter] + 1; //send adr + 1
                i2c_struct[I2C_3].i2c_tx_counter++;
                i2c_struct[I2C_3].i2c_int_counter++;             
            }

            // Enable data read on I2C bus
            else if (i2c_struct[I2C_3].i2c_int_counter == 1)
            {
                IFS8bits.MI2C3IF = 0;  
                I2C3CONLbits.RCEN = 1;
                i2c_struct[I2C_3].i2c_int_counter++; 
            }    

            // Read data from i2c slave
            else if (i2c_struct[I2C_3].i2c_int_counter == 2)
            {
                IFS8bits.MI2C3IF = 0;  
                if (i2c_struct[I2C_3].i2c_rx_counter < i2c_struct[I2C_3].i2c_read_length)
                {   
                    // Get byte and store in rx buffer
                    i2c_struct[I2C_3].i2c_rx_data[i2c_struct[I2C_3].i2c_rx_counter] = I2C3RCV;

                    // See if we wish to receive more data as set in i2c_read_length field
                    if (++i2c_struct[I2C_3].i2c_rx_counter < i2c_struct[I2C_3].i2c_read_length)
                    {          
                        I2C3CONLbits.ACKDT = 0; //send ack
                        I2C3CONLbits.ACKEN = 1; //continue data reception
                        i2c_struct[I2C_3].i2c_int_counter = 1;
                    }   

                    // No more data to receive, send NACK to slave device
                    else
                    {
                        I2C3CONLbits.ACKDT = 1; //send nack
                        I2C3CONLbits.ACKEN = 1;
                    }   
                } 

                // Stop I2C transaction
                else
                {
                    I2C3CONLbits.PEN = 1;
                    i2c_struct[I2C_3].i2c_int_counter++; 
                }            
            }

            else if (i2c_struct[I2C_3].i2c_int_counter == 3)
            {
                i2c_struct[I2C_3].i2c_int_counter = 0;
                i2c_struct[I2C_3].i2c_rx_counter = 0;
                i2c_struct[I2C_3].i2c_tx_counter = 0;
                i2c_struct[I2C_3].i2c_done = 1;
                i2c_struct[I2C_3].i2c_rx_done = 1;
                IEC8bits.MI2C3IE = 0;
                IFS8bits.MI2C3IF = 0; 
            } 
        } 
        
        if (i2c_struct[I2C_3].read_mode == I2C_READ_MODE_STOP_START)
        {
            if (i2c_struct[I2C_3].i2c_int_counter == 0)
            {
                IFS8bits.MI2C3IF = 0;  
                ack_adr = 0;            // ack address flag low
                I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter]; //send adr
                i2c_struct[I2C_3].i2c_int_counter++;
                i2c_struct[I2C_3].i2c_tx_counter++;  
            }            

            else if (i2c_struct[I2C_3].i2c_int_counter == 1) // send data
            {
                // Do once, scan ack state form when the address byte was sent, see if the device responded
                if (ack_adr == 0)
                {
                    ack_adr = 1;
                    i2c_struct[I2C_3].ack_state = I2C3STATbits.ACKSTAT;
                }

                if (i2c_struct[I2C_3].i2c_tx_counter < i2c_struct[I2C_3].i2c_write_length)//send data
                {  
                    IFS8bits.MI2C3IF = 0; 
                    I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter];
                    i2c_struct[I2C_3].i2c_tx_counter++;
                }

                else //all the data was transmitted
                {
                    IFS8bits.MI2C3IF = 0; 
                    I2C3CONLbits.PEN = 1;                            //enable stop sequence
                    i2c_struct[I2C_3].i2c_int_counter++;     
                }
            }
            
            else if (i2c_struct[I2C_3].i2c_int_counter == 2)
            {
                IFS8bits.MI2C3IF = 0;   
                I2C3CONLbits.SEN = 1;                        // Start I2C sequence 
                i2c_struct[I2C_3].i2c_int_counter++; 
            }
            
            // Send read address
            else if (i2c_struct[I2C_3].i2c_int_counter == 3)
            {
                IFS8bits.MI2C3IF = 0; 
                I2C3TRN = i2c_struct[I2C_3].i2c_tx_data[i2c_struct[I2C_3].i2c_tx_counter] + 1; //send adr + 1
                i2c_struct[I2C_3].i2c_tx_counter++;
                i2c_struct[I2C_3].i2c_int_counter++;             
            }

            // Enable data read on I2C bus
            else if (i2c_struct[I2C_3].i2c_int_counter == 4)
            {
                IFS8bits.MI2C3IF = 0; 
                I2C3CONLbits.RCEN = 1;
                i2c_struct[I2C_3].i2c_int_counter++; 
            }    

            // Read data from i2c slave
            else if (i2c_struct[I2C_3].i2c_int_counter == 5)
            {
                IFS8bits.MI2C3IF = 0; 
                if (i2c_struct[I2C_3].i2c_rx_counter < i2c_struct[I2C_3].i2c_read_length)
                {   
                    // Get byte and store in rx buffer
                    i2c_struct[I2C_3].i2c_rx_data[i2c_struct[I2C_3].i2c_rx_counter] = I2C3RCV;

                    // See if we wish to receive more data as set in i2c_read_length field
                    if (++i2c_struct[I2C_3].i2c_rx_counter < i2c_struct[I2C_3].i2c_read_length)
                    {          
                        I2C3CONLbits.ACKDT = 0; //send ack
                        I2C3CONLbits.ACKEN = 1; //continue data reception
                        i2c_struct[I2C_3].i2c_int_counter = 4;
                    }   

                    // No more data to receive, send NACK to slave device
                    else
                    {
                        I2C3CONLbits.ACKDT = 1; //send nack
                        I2C3CONLbits.ACKEN = 1;
                    }   
                } 

                // Stop I2C transaction
                else
                {
                    I2C3CONLbits.PEN = 1;
                    i2c_struct[I2C_3].i2c_int_counter++; 
                }            
            }

            else if (i2c_struct[I2C_3].i2c_int_counter == 6)
            {
                i2c_struct[I2C_3].i2c_int_counter = 0;
                i2c_struct[I2C_3].i2c_rx_counter = 0;
                i2c_struct[I2C_3].i2c_tx_counter = 0;
                i2c_struct[I2C_3].i2c_done = 1;
                i2c_struct[I2C_3].i2c_rx_done = 1;
                IEC8bits.MI2C3IE = 0;
                IFS8bits.MI2C3IF = 0; 
            } 
        }       
    }
}
