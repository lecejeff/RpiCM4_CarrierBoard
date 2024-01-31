//***************************************************************************//
// File      : I2C.h
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
// Includes  : dspic_33ck_generic.h
//
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#ifndef __I2C_H__
#define	__I2C_H__

#include "dspic_33ck_generic.h"

#define I2C_2 0
#define I2C_3 1
#define I2C_QTY 2

#define I2C_FREQ_100k 492   //@Fcy=100MIPS
#define I2C_FREQ_400k 117   //@Fcy=100MIPS

#define I2C_mode_master 0
#define I2C_mode_slave 1

#define I2C_TX_LENGTH 512
#define I2C_RX_LENGTH 512

#define I2C_WRITE 0
#define I2C_READ 1

#define I2C_READ_MODE_RESTART       0
#define I2C_READ_MODE_DIRECT        1
#define I2C_READ_MODE_STOP_START    2

typedef struct
{   
    uint8_t I2C_channel;
    uint16_t i2c_tx_counter;
    uint16_t i2c_rx_counter;
    uint8_t i2c_tx_data[I2C_TX_LENGTH];
    uint8_t i2c_rx_data[I2C_RX_LENGTH];
    uint8_t i2c_int_counter;
    uint16_t i2c_write_length;
    uint16_t i2c_read_length;
    uint8_t i2c_done;
    uint8_t i2c_rx_done;
    uint8_t i2c_message_mode;
    uint8_t ack_state;
    uint8_t read_mode;
}STRUCT_I2C;

uint8_t I2C_init (STRUCT_I2C *i2c, uint8_t port, uint16_t freq, uint8_t mode, uint8_t address);
void I2C_fill_transmit_buffer (STRUCT_I2C *i2c, uint8_t *ptr, uint8_t length);
void I2C_master_write (STRUCT_I2C *i2c, uint8_t *data, uint16_t length);
void I2C_master_read (STRUCT_I2C *i2c, uint8_t mode, uint8_t *data, uint16_t w_length, uint16_t r_length);
uint8_t I2C_wait (STRUCT_I2C *i2c);
uint8_t I2C_read_state (STRUCT_I2C *i2c);
uint8_t * I2C_get_rx_buffer (STRUCT_I2C *i2c);
uint8_t * I2C_get_rx_buffer_index (STRUCT_I2C *i2c, uint16_t index);
uint8_t I2C_done (STRUCT_I2C *i2c);
uint8_t I2C_rx_done (STRUCT_I2C *i2c);
uint8_t I2C_get_ack_state (STRUCT_I2C *i2c);
void I2C_change_address (STRUCT_I2C *i2c);
void I2C_clear_rx_buffer (STRUCT_I2C *i2c);


#endif	/* I2C_H */