#include "dspic_33ck_generic.h"
#include "i2c.h"

// The default I2C address for the BNO08X on the Smart DOF click is 0x94 (7b))
// It can be changed to 0x96 using an on-board solderable jumper
#define BNO08X_DEFAULT_ADDRESS 0x94

#define BNO08X_QTY              1
#define BNO08X_1                0
#define SHTP_MAX_CARGO          32766   // Includes header bytes (4x bytes) -> data max is 32762
#define BNO08X_SENSOR_CHANNELS  6       // 6 channels supported by BNO08X

#define BNO08X_MKB1         0
#define BNO08X_MKB2         1
#define BNO08X_MKB1_INT     0
#define BNO08X_MKB1_RST     0
#define BNO08X_MKB2_INT     1
#define BNO08X_MKB2_RST     1

// BNO08X sensor IDs
#define BNO08X_SENSOR_ID_ACCEL          0x01
#define BNO08X_SENSOR_ID_GYRO           0x02
#define BNO08X_SENSOR_ID_MAGNETO        0x03
#define BNO08X_SENSOR_ID_ACCEL_LIN      0x04
#define BNO08X_SENSOR_ID_GYRO_ROT_VECT  0x05
#define BNO08X_SENSOR_ID_GRAVITY        0x06

// BNO08X cargo channel defines
#define BNO08X_CHANNEL0_SHTP    0
#define BNO08X_CHANNEL1_EXEC    1
#define BNO08X_CHANNEL2_SHCC    2
#define BNO08X_CHANNEL3_SNSREP  3
#define BNO08X_CHANNEL4_WISR    4
#define BNO08X_CHANNEL5_GYRVEC  5

// BNO08X SHTP channel defines
#define BNO08X_SHTP_ADVERTISE_HOST  0
#define BNO08X_SHTP_ADVERTISE_HUB   1

// BNO080 executable cargo defines. Non-defined values are reserved
#define BNO08X_EXEC_WR_RESET        1
#define BNO08X_EXEC_WR_ON           2
#define BNO08X_EXEC_WR_SLEEP        3
#define BNO08X_EXEC_RD_RESET        1

// BNO08X Sensor hub channel supported commands
#define BNO08X_SHCC_COM_RES         0xF1
#define BNO08X_SHCC_COM_REQ         0xF2
#define BNO08X_SHCC_FRS_RD_RES      0xF3
#define BNO08X_SHCC_FRS_RD_REQ      0xF4
#define BNO08X_SHCC_FRS_WR_RES      0xF5
#define BNO08X_SHCC_FRS_WR_DAT      0xF6
#define BNO08X_SHCC_FRW_WR_REQ      0xF7
#define BNO08X_SHCC_PID_RES         0xF8
#define BNO08X_SHCC_PID_REQ         0xF9
#define BNO08X_SHCC_GET_FEAT_RES    0xFC
#define BNO08X_SHCC_SET_FEAT_COM    0xFD
#define BNO08X_SHCC_GET_FEAT_REQ    0xFE

typedef struct
{
    STRUCT_I2C *i2c_ref;
    uint8_t mkb_port;
    uint8_t has_reset;
    uint8_t int_flag;
       
    uint8_t cargo[256];                             // Set cargo length to 256, including SHTP header 
    uint8_t cargo_continue;                         // Cargo length bit 15 (15..0) indicates continuatio of pref. transaction
    uint16_t cargo_length;                          // cargo length including SHTP header
    uint16_t write_length;                          // Total of cargo + adr byte to write on I2C
    uint8_t length_lsb;                             // bit15 indicate if transfer is a continuation of another one
    uint8_t length_msb;                             // bit 14:0 total number of bytes in cargo including header
    uint8_t channel;                                // channel type
    uint8_t channel_seqnum[BNO08X_SENSOR_CHANNELS]; // seq number for each channel
}STRUCT_BNO08X;

uint8_t BNO08X_init (STRUCT_BNO08X *bno, STRUCT_I2C *i2c, uint8_t port);
uint8_t BNO08X_has_reset (STRUCT_BNO08X *bno);
uint8_t BNO08X_int_state (STRUCT_BNO08X *bno);
void BNO08X_parse_shtp (STRUCT_BNO08X *bno, uint8_t *data);
uint16_t BNO08X_get_cargo_length (STRUCT_BNO08X *bno);
uint8_t BNO08X_write (STRUCT_BNO08X *bno, uint8_t channel, uint8_t *data, uint16_t data_length);