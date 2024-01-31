#include "BNO080.h"

STRUCT_BNO08X BNO08X_struct[BNO08X_QTY];

uint8_t BNO08X_init (STRUCT_BNO08X *bno, STRUCT_I2C *i2c, uint8_t port)
{      
    uint16_t i = 0;
    bno->i2c_ref = i2c;
    bno->mkb_port = port;
    bno->has_reset = 0;     // 1st external interrupt caused by device waking up
    bno->int_flag = 0;
    bno->cargo_continue = 0;    
    
    for (i=0; i<BNO08X_SENSOR_CHANNELS; i++)
    {
        bno->channel_seqnum[i] = 0;
    }
    for (i=0; i<256; i++)
    {
        bno->cargo[i] = 0;
    }
    
    I2C_init(bno->i2c_ref, I2C_3, I2C_FREQ_100k, I2C_mode_master, 0);   // Initialize MikroBus I2C port
    
    if (bno->mkb_port == BNO08X_MKB1)
    {
        DSPIC_MKB1_RST_DIR = 0;    // Set reset pin to output
        DSPIC_MKB1_RST_WR = 1;     // Get the IC out of reset
        __delay_ms(10);
        DSPIC_MKB1_RST_WR = 0;     // Keep BNO080 in reset
        __delay_ms(10);
        DSPIC_MKB1_PWM_DIR = 0;    // Set BOOTn pin to output
        DSPIC_MKB1_PWM_WR = 1;     // Set BOOTn to 1 (I2C mode, no bootloader)
        DSPIC_MKB1_INT_DIR = 1;    // Set interrupt pin to input
        __delay_ms(10);
        DSPIC_MKB1_RST_WR = 1;     // Get the IC out of reset
        __delay_ms(100);
    }
    
    if (bno->mkb_port == BNO08X_MKB2)
    {
        DSPIC_MKB2_RST_DIR = 0;    // Set reset pin to output
        DSPIC_MKB2_RST_WR = 1;     // Get the IC out of reset
        __delay_ms(10);
        DSPIC_MKB2_RST_WR = 0;     // Keep BNO080 in reset
        __delay_ms(10);
        DSPIC_MKB2_PWM_DIR = 0;    // Set BOOTn pin to output
        DSPIC_MKB2_PWM_WR = 1;     // Set BOOTn to 1 (I2C mode, no bootloader)
        DSPIC_MKB2_INT_DIR = 1;    // Set interrupt pin to input
        __delay_ms(10);
        //DSPIC_MKB2_RST_WR = 1;     // Get the IC out of reset
        //__delay_ms(100);
        
        // Enable external interrupt on BNO08X interrupt pin
        RPINR0bits.INT1R = 85;      // RF5 = RP85
        IEC0bits.INT1IE = 1;
        IFS0bits.INT1IF = 0;
        INTCON2bits.INT1EP = 1;     // Interrupt on negative edge
        DSPIC_MKB2_RST_WR = 1;      // Get the IC out of reset
        __delay_ms(100);
        // must detect a high-to-low transition on INTn pin before trying to
        // communicate with BNO08X
    }
    return 0;
}

uint8_t BNO08X_has_reset (STRUCT_BNO08X *bno)
{
    return bno->has_reset;
}

uint8_t BNO08X_int_state (STRUCT_BNO08X *bno)
{
    if (bno->int_flag == 1)
    {
        bno->int_flag = 0;
        return 1;
    }
    else
        return 0;
}

void BNO08X_parse_shtp (STRUCT_BNO08X *bno, uint8_t *data)
{
    uint16_t length = 0;
    bno->length_lsb = *data++;
    bno->length_msb = *data++; 
    if (((bno->length_msb & 0x80) >> 7) == 1)
    {
        bno->cargo_continue = 1;
        bno->length_msb &= 0x7F;    // Clear MSB
    }
    else
        bno->cargo_continue = 0;
    length = bno->length_lsb | (bno->length_msb << 8);
    bno->cargo_length = length & 0x7F;
    bno->channel = *data++;
    bno->channel_seqnum[bno->channel] = *data;
}

uint16_t BNO08X_get_cargo_length (STRUCT_BNO08X *bno)
{
    return bno->cargo_length;
}

uint8_t BNO08X_write (STRUCT_BNO08X *bno, uint8_t channel, uint8_t *data, uint16_t data_length)
{
    uint16_t i = 0;
    if (data_length > SHTP_MAX_CARGO)
    {
        return 0;
    }
    else
    {
        bno->cargo_length = data_length;
        bno->write_length = bno->cargo_length + 5;   // Include address byte + SHTP header
        bno->cargo[0] = BNO08X_DEFAULT_ADDRESS;
        bno->cargo[1] = (bno->cargo_length & 0x00FF);
        bno->cargo[2] = ((bno->cargo_length & 0xFF)<<8);
        bno->cargo[3] = channel;
        bno->cargo[4] = bno->channel_seqnum[channel]++;
        for (; i<data_length; i++)
        {
            bno->cargo[i+5] = *data++;
        }
        I2C_master_write(bno->i2c_ref, bno->cargo, bno->write_length);
    }
    return 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _INT1Interrupt(void)
{
    IFS0bits.INT1IF = 0;
    // 1st boot behavior
    if (BNO08X_struct[BNO08X_1].has_reset == 0)
    {
        BNO08X_struct[BNO08X_1].has_reset = 1;
    }
    // 
    else
    {
        BNO08X_struct[BNO08X_1].int_flag = 1;
    }
}


