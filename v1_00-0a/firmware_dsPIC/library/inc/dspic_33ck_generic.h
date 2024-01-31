//***************************************************************************//
// File      : general.h
//
// Includes  : xc.h
//
// Purpose   : General header file
//
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#ifndef __dspic_33ck_generic_h__
#define	__dspic_33ck_generic_h__

// If using primary oscillator (20MHz) with PLL, uncomment #define POSC_20MHz
#define POSC_20MHz_100MIPS
#ifdef POSC_20MHz_100MIPS
    #define FPLLO 400000000UL
    #define FOSC (FPLLO/2)
    #define FCY (FOSC/2) 
    #define _XTAL_FREQ FCY
    void dspic_posc_20MHz_init (void);
#endif

//#define LPRC_32KHZ
#ifdef SOSC_32KHZ
    void dspic_sosc_32kHz_init (void);
#endif  

// Microchip libraries - Must be included AFTER FCY define
#include <xc.h>
#include <stdint.h>
#include <libpic30.h>
#include <dsp.h>
#include <string.h>
// End of Microchip libraries    
    
typedef struct
{
    uint8_t state;
    uint8_t prev_state;
    uint16_t debounce_cnt;
    uint16_t debounce_thres;
    uint8_t debounce_state;
    uint8_t do_once;
    uint8_t channel;
}STRUCT_BUTTON;

typedef struct
{
    uint8_t state;
    uint8_t prev_state;
    uint8_t channel;
}STRUCT_LED;

// rPICM4CB generic I/Os definition
// Basically LEDs, Pushbutton, Encoder switch, buzzer, RGB LED
// ---------------------- dspic on-board pushbuttons -------------------------//
#define DSPIC_BTN1_RD          PORTEbits.RE14
#define DSPIC_BTN1_DIR         TRISEbits.TRISE14
#define DSPIC_BTN2_RD          PORTEbits.RE15
#define DSPIC_BTN2_DIR         TRISEbits.TRISE15
#define BTN_1                   0
#define BTN_2                   1
#define OUTPUT                  0
#define INPUT                   1
#define BTN_QTY                 2

// ---------------------- dspic on-board LEDs --------------------------------//
#define DSPIC_LED1_WR          LATEbits.LATE10
#define DSPIC_LED1_DIR         TRISEbits.TRISE10
#define DSPIC_LED2_WR          LATEbits.LATE13
#define DSPIC_LED2_DIR         TRISEbits.TRISE13
#define LED_1                   0
#define LED_2                   1
#define HIGH                    1
#define LOW                     0  
#define LED_QTY                 2

// ---------------------- dspic Power-kill all -------------------------------//
#define DSPIC_POWER_KILL_DIR    TRISEbits.TRISE5
#define DSPIC_POWER_KILL_WR     LATEbits.LATE5
#define DSPIC_POWER_KILL_RD     PORTEbits.RE5

// ---------------------- dspic Watchdog assert ------------------------------//
#define DSPIC_WATCHDOG_DIR      TRISEbits.TRISE9
#define DSPIC_WATCHDOG_WR       LATEbits.LATE9
#define DSPIC_WATCHDOG_RD       PORTEbits.RE9

// ---------------------- dspic -> CM4 INTn assert ---------------------------//
#define DSPIC_TO_CM4_INT_DIR    TRISEbits.TRISE7
#define DSPIC_TO_CM4_INT_WR     LATEbits.LATE7
#define DSPIC_TO_CM4_INT_RD     PORTEbits.RE7

// ---------------------- CM4 -> dspic INTn assert ---------------------------//
#define CM4_TO_DSPIC_INT_DIR    TRISEbits.TRISE8
#define CM4_TO_DSPIC_INT_WR     LATEbits.LATE8
#define CM4_TO_DSPIC_INT_RD     PORTEbits.RE8

// ----------------------- dspic MikroBUS Socket #1 --------------------------//
// AIN
#define DSPIC_MKB1_AIN_DIR      TRISEbits.TRISE0
#define DSPIC_MKB1_AIN_ANS      ANSELEbits.ANSELE0
#define DSPIC_MKB1_AIN_WR       LATEbits.LATE0
#define DSPIC_MKB1_AIN_RD       PORTEbits.RE0
// RST
#define DSPIC_MKB1_RST_DIR      TRISEbits.TRISE1
#define DSPIC_MKB1_RST_WR       LATEbits.LATE1
#define DSPIC_MKB1_RST_RD       PORTEbits.RE1
// SPI CSn
#define DSPIC_MKB1_CSn_DIR      TRISFbits.TRISF0
#define DSPIC_MKB1_CSn_WR       LATFbits.LATF0
#define DSPIC_MKB1_CSn_RD       PORTFbits.RF0
// SPI SCK
#define DSPIC_MKB1_SCK_DIR      TRISCbits.TRISC15
#define DSPIC_MKB1_SCK_WR       LATCbits.LATC15
#define DSPIC_MKB1_SCK_RD       PORTCbits.RC15
// SPI MOSI
#define DSPIC_MKB1_MOSI_DIR     TRISFbits.TRISF1
#define DSPIC_MKB1_MOSI_WR      LATFbits.LATF1
#define DSPIC_MKB1_MOSI_RD      PORTFbits.RF1
// SPI MISO
#define DSPIC_MKB1_MISO_DIR     TRISDbits.TRISD15
#define DSPIC_MKB1_MISO_WR      LATDbits.LATD15
#define DSPIC_MKB1_MISO_RD      PORTDbits.RD15
// INT
#define DSPIC_MKB1_INT_DIR      TRISCbits.TRISC0
#define DSPIC_MKB1_INT_WR       LATCbits.LATC0
#define DSPIC_MKB1_INT_RD       PORTCbits.RC0
// PWM
#define DSPIC_MKB1_PWM_DIR      TRISCbits.TRISC14
#define DSPIC_MKB1_PWM_WR       LATCbits.LATC14
#define DSPIC_MKB1_PWM_RD       PORTCbits.RC14
// SDA
#define DSPIC_MKB1_SDA_DIR      TRISBbits.TRISB5
#define DSPIC_MKB1_SDA_WR       LATBbits.LATB5
#define DSPIC_MKB1_SDA_RD       PORTBbits.RB5
// SCL
#define DSPIC_MKB1_SCL_DIR      TRISBbits.TRISB6
#define DSPIC_MKB1_SCL_WR       LATBbits.LATB6
#define DSPIC_MKB1_SCL_RD       PORTBbits.RB6
// UART TX
#define DSPIC_MKB1_UART_TX_DIR  TRISFbits.TRISF15
#define DSPIC_MKB1_UART_TX_WR   LATFbits.LATF15
#define DSPIC_MKB1_UART_TX_RD   PORTFbits.RF15
// UART RX
#define DSPIC_MKB1_UART_RX_DIR  TRISDbits.TRISD13
#define DSPIC_MKB1_UART_RX_WR   LATDbits.LATD13
#define DSPIC_MKB1_UART_RX_RD   PORTDbits.RD13

// ----------------------- dspic MikroBUS Socket #2 --------------------------//
// AIN
#define DSPIC_MKB2_AIN_DIR      TRISAbits.TRISA0
#define DSPIC_MKB2_AIN_ANS      ANSELAbits.ANSELA0
#define DSPIC_MKB2_AIN_WR       LATAbits.LATA0
#define DSPIC_MKB2_AIN_RD       PORTAbits.RA0
// RST
#define DSPIC_MKB2_RST_DIR      TRISDbits.TRISD5
#define DSPIC_MKB2_RST_WR       LATDbits.LATD5
#define DSPIC_MKB2_RST_RD       PORTDbits.RD5
// SPI CSn
#define DSPIC_MKB2_CSn_DIR      TRISDbits.TRISD6
#define DSPIC_MKB2_CSn_WR       LATDbits.LATD6
#define DSPIC_MKB2_CSn_RD       PORTDbits.RD6
// SPI SCK
#define DSPIC_MKB2_SCK_DIR      TRISDbits.TRISD7
#define DSPIC_MKB2_SCK_WR       LATDbits.LATD7
#define DSPIC_MKB2_SCK_RD       PORTDbits.RD7
// SPI MOSI
#define DSPIC_MKB2_MOSI_DIR     TRISDbits.TRISD9
#define DSPIC_MKB2_MOSI_WR      LATDbits.LATD9
#define DSPIC_MKB2_MOSI_RD      PORTDbits.RD9
// SPI MISO
#define DSPIC_MKB2_MISO_DIR     TRISBbits.TRISB4
#define DSPIC_MKB2_MISO_WR      LATBbits.LATB4
#define DSPIC_MKB2_MISO_RD      PORTBbits.RB4
// INT
#define DSPIC_MKB2_INT_DIR      TRISFbits.TRISF5
#define DSPIC_MKB2_INT_WR       LATFbits.LATF5
#define DSPIC_MKB2_INT_RD       PORTFbits.RF5
// PWM
#define DSPIC_MKB2_PWM_DIR      TRISFbits.TRISF13
#define DSPIC_MKB2_PWM_WR       LATFbits.LATF13
#define DSPIC_MKB2_PWM_RD       PORTFbits.RF13
// SDA
#define DSPIC_MKB2_SDA_DIR      TRISDbits.TRISD4
#define DSPIC_MKB2_SDA_WR       LATDbits.LATD4
#define DSPIC_MKB2_SDA_RD       PORTDbits.RD4
// SCL
#define DSPIC_MKB2_SCL_DIR      TRISDbits.TRISD3
#define DSPIC_MKB2_SCL_WR       LATDbits.LATD3
#define DSPIC_MKB2_SCL_RD       PORTDbits.RD3
// UART TX
#define DSPIC_MKB2_UART_TX_DIR  TRISDbits.TRISD11
#define DSPIC_MKB2_UART_TX_WR   LATDbits.LATD11
#define DSPIC_MKB2_UART_TX_RD   PORTDbits.RD11
// UART RX
#define DSPIC_MKB2_UART_RX_DIR  TRISFbits.TRISF8
#define DSPIC_MKB2_UART_RX_WR   LATFbits.LATF8
#define DSPIC_MKB2_UART_RX_RD   PORTFbits.RF8

// ----------------------- dspic DEBUG port pins #2 --------------------------//
// UART DEBUG TX
#define DSPIC_DEBUG_UART_TX_DIR TRISCbits.TRISC12
#define DSPIC_DEBUG_UART_TX_WR  LATCbits.LATC12
#define DSPIC_DEBUG_UART_TX_RD  PORTCbits.RC12
// UART DEBUG RX
#define DSPIC_DEBUG_UART_RX_DIR TRISCbits.TRISC13
#define DSPIC_DEBUG_UART_RX_WR  LATCbits.LATC13
#define DSPIC_DEBUG_UART_RX_RD  PORTCbits.RC13
   
void dspic_init(void);
uint8_t dspic_button_init (STRUCT_BUTTON *btn, uint8_t channel, uint16_t debounce_thres);
uint8_t dspic_button_debounce (STRUCT_BUTTON *btn);
uint8_t dspic_button_get_state (STRUCT_BUTTON *btn);
uint8_t dspic_led_init (STRUCT_LED *led, uint8_t channel, uint8_t value);
uint8_t dspic_led_write (STRUCT_LED *led, uint8_t value);

// Various data manipulation functions
void uint16_to_byte8 (uint16_t *ptr_16, uint8_t *ptr_8, uint16_t length);
void byte8_to_uint16 (uint8_t *ptr_8, uint16_t *ptr_16, uint16_t length);
void hex_to_ascii(uint8_t in, uint8_t *out_h, uint8_t *out_l);
int16_t bcd_to_decimal(uint8_t bcd);
uint16_t dec_to_bcd(uint16_t dec);
uint8_t hex_to_dec (uint8_t hex);

#endif	/* __dspic_33ck_generic_h__ */

