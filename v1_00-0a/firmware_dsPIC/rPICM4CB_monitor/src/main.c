#include "dspic_33ck_generic.h"
#include "UART.h"
#include "Timer.h"
#include "i2c.h"
#include "bno080.h"
#include "pwm.h"

// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FSIGN

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)
#pragma config PLLKEN = ON              // PLL Lock Status Control (PLL lock signal will be used to disable PLL clock output if lock is lost)
#pragma config XTCFG = G2               // XT Config (16-24 MHz crystals)
#pragma config XTBST = DISABLE          // XT Boost (Default kick-start)

// FWDT
#pragma config RWDTPS = PS2147483648    // Run Mode Watchdog Timer Post Scaler select bits (1:2147483648)
#pragma config RCLKSEL = LPRC           // Watchdog Timer Clock Select bits (Always use LPRC)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer operates in Window mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config SWDTPS = PS2147483648    // Sleep Mode Watchdog Timer Post Scaler select bits (1:2147483648)
#pragma config FWDTEN = ON_SW           // Watchdog Timer Enable bit (WDT controlled via SW, use WDTCON.ON bit)

// FPOR
#pragma config BISTDIS = DISABLED       // Memory BIST Feature Disable (mBIST on reset feature disabled)

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGC1 and PGD1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config NOBTSWP = DISABLED       // BOOTSWP instruction disable bit (BOOTSWP instruction is disabled)

// FDMTIVTL
#pragma config DMTIVTL = 0xFFFF         // Dead Man Timer Interval low word (Enter Hexadecimal value)

// FDMTIVTH
#pragma config DMTIVTH = 0xFFFF         // Dead Man Timer Interval high word (Enter Hexadecimal value)

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         // Lower 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF) (Enter Hexadecimal value)

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         // Upper 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF) (Enter Hexadecimal value)

// FDMT
#pragma config DMTDIS = OFF             // Dead Man Timer Disable bit (Dead Man Timer is Disabled and can be enabled by software)

// FDEVOPT
#pragma config ALTI2C1 = OFF            // Alternate I2C1 Pin bit (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 Pin bit (I2C2 mapped to SDA2/SCL2 pins)
#pragma config ALTI2C3 = ON             // Alternate I2C3 Pin bit (I2C3 mapped to ASDA3/ASCL3 pins)
#pragma config SMB3EN = SMBUS3          // SM Bus 3.0 Enable (SMBus 3.0 input threshold is enabled)
#pragma config DUPPWM = PPS             // Duplicate PWM Pin Mapping (Default PWM Functions of PPS and Fixed Pins)
#pragma config SPI2PIN = DEDICATED      // SPI2 Pin Select bit (SPI2 uses dedicated I/O pins)

// FALTREG
#pragma config CTXT1 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 1 bits (Not Assigned)
#pragma config CTXT2 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 2 bits (Not Assigned)
#pragma config CTXT3 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 3 bits (Not Assigned)
#pragma config CTXT4 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 4 bits (Not Assigned)

// FBTSEQ
#pragma config BSEQ = 0xFFF             // Relative value defining which partition will be active after device Reset; the partition containing a lower boot number will be active (Enter Hexadecimal value)
#pragma config IBSEQ = 0xFFF            // The one's complement of BSEQ; must be calculated by the user and written during device programming. (Enter Hexadecimal value)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// Access to PWM struct members
extern STRUCT_PWM PWM_struct[PWM_QTY];
STRUCT_PWM *PWM1L_struct = &PWM_struct[PWM_1L];
STRUCT_PWM *PWM1H_struct = &PWM_struct[PWM_1H];
STRUCT_PWM *PWM2L_struct = &PWM_struct[PWM_2L];
STRUCT_PWM *PWM2H_struct = &PWM_struct[PWM_2H];
STRUCT_PWM *PWM5H_struct = &PWM_struct[PWM_5L];
STRUCT_PWM *PWM5L_struct = &PWM_struct[PWM_5H];
STRUCT_PWM *PWM7H_struct = &PWM_struct[PWM_7L];
STRUCT_PWM *PWM7L_struct = &PWM_struct[PWM_7H];

extern STRUCT_TIMER TIMER_struct[TIMER_QTY];
STRUCT_TIMER *TIMER1_struct = &TIMER_struct[TIMER_1];

extern STRUCT_UART UART_struct[UART_QTY];
STRUCT_UART *UART_DEBUG_struct = &UART_struct[UART_1];
STRUCT_UART *UART_MKB1_struct = &UART_struct[UART_2];
STRUCT_UART *UART_MKB2_struct = &UART_struct[UART_3];

// Access to I2C struct members
extern STRUCT_I2C i2c_struct[I2C_QTY];
STRUCT_I2C *I2C2_struct = &i2c_struct[I2C_2];           // MikroBus 1 I2C port, 100kHz
STRUCT_I2C *I2C3_struct = &i2c_struct[I2C_3];           // MikroBus 2 I2C port, 100kHz

uint8_t counter_sec = 0;
uint8_t counter_100ms = 0;
uint8_t data[2] = {0x88, 0xFD};
uint8_t * result;
uint8_t sht40_buf[6];
uint8_t i = 0;
uint16_t t_ticks = 0;
uint16_t rh_ticks = 0;
uint16_t t_degC = 0;
uint8_t tDegH = 0, tDegL = 0;
uint16_t rh_pRH = 0;
uint8_t rhH = 0, rhL = 0; 
uint8_t duty = 0;

int main (void)
{
    dspic_init();
    
    UART_init(UART_DEBUG_struct, UART_1, 115200, UART_MAX_TX, UART_MAX_RX, 0);
    UART_init(UART_MKB1_struct, UART_2, 115200, UART_MAX_TX, UART_MAX_RX, 1);
    UART_init(UART_MKB2_struct, UART_3, 115200, UART_MAX_TX, UART_MAX_RX, 2);
       
    PWM_init(PWM1L_struct, PWM_1L, PWM_TYPE_SERVO);
    PWM_init(PWM1H_struct, PWM_1H, PWM_TYPE_SERVO);
    
    PWM_change_duty_perc(PWM1L_struct, 0);
    PWM_change_duty_perc(PWM1H_struct, 0);
    
    PWM_enable(PWM1L_struct);
    PWM_enable(PWM1H_struct);
    
    TIMER_init(TIMER1_struct, TIMER_1, TIMER_MODE_16B, TIMER_PRESCALER_256, 10);
    
    //I2C_init(I2C2_struct, I2C_2, I2C_FREQ_100k, I2C_mode_master, 0);
    I2C_init(I2C3_struct, I2C_3, I2C_FREQ_100k, I2C_mode_master, 0);
    
    DSPIC_LED1_DIR = 0;
    DSPIC_LED2_DIR = 0;
    DSPIC_BTN1_DIR = 1;
    DSPIC_POWER_KILL_WR = 0;
    DSPIC_POWER_KILL_DIR = 0;
   
    DSPIC_LED1_WR = 1;
    DSPIC_LED2_WR = 1;
    
    UART_putstr_dma(UART_DEBUG_struct, "\r\nrPICM4CB dsPIC33CK UART alive at 115200 with DMA !!!\r\n");
    //UART_putstr_dma(UART_MKB1_struct, "\r\nrPICM4CB dsPIC33CK UART MKB1 at 115200 with DMA !!!\r\n");
    //UART_putstr_dma(UART_MKB2_struct, "\r\nrPICM4CB dsPIC33CK UART MKB2 at 115200 with DMA !!!\r\n");
    
    TIMER_start(TIMER1_struct);
    while (1)
    {
        if (TIMER_get_state(TIMER1_struct, TIMER_INT_STATE))
        {    
            counter_100ms++;
            if (counter_100ms == 1)
            {
                I2C_master_write(I2C3_struct, data, 2);
            }
            
            if (counter_100ms == 2)
            {
                I2C_master_read(I2C3_struct, I2C_READ_MODE_DIRECT, data, 1, 6);
            }
            
            else if (counter_100ms == 3)
            {
                if (I2C_rx_done(I2C3_struct) == 1)
                {
                    result = I2C_get_rx_buffer(I2C3_struct);
                    for (i=0; i<6; i++)
                    {
                        sht40_buf[i] = *result++;
                    }
                    t_ticks = (uint16_t)((sht40_buf[0] * 256) + sht40_buf[1]);
                    rh_ticks = (uint16_t)((sht40_buf[3] * 256) + sht40_buf[4]);
                    t_degC = (float)(-45.0 + (175.0 * (t_ticks/65535.0)));
                    rh_pRH = (float)(-6.0 + (125.0 * (rh_ticks/65535.0)));
                    
                    tDegH = ((t_degC & 0xFF00)>>8);
                    tDegL = t_degC & 0x00FF;
                    tDegH = dec_to_bcd(tDegH);
                    tDegL = dec_to_bcd(tDegL);
                    rhH = ((rh_pRH & 0xFF00)>>8);
                    rhL = rh_pRH & 0x00FF;
                    rhH = dec_to_bcd(rhH);
                    rhL = dec_to_bcd(rhL);
                } 
                counter_100ms = 0;
            }
            
            if (++counter_sec > TIMER_get_freq(TIMER1_struct))
            {
                counter_sec = 0;
                duty += 10;
                if (duty > 100){duty = 0;}
                PWM_change_duty_perc(PWM1L_struct, duty);
                PWM_change_duty_perc(PWM1H_struct, duty);
                PWM_request_update(PWM1L_struct);
                //UART_putstr_dma(UART_DEBUG_struct, "\r\n-Test UART debug with DMA !!!-\r\n");
                UART_putstr(UART_DEBUG_struct, "Data from SHT40 = ");
                UART_putc_ascii(UART_DEBUG_struct, tDegL);
                UART_putc(UART_DEBUG_struct, ':');
                UART_putc_ascii(UART_DEBUG_struct, rhL);
                UART_putc(UART_DEBUG_struct, 0x0D);
                UART_putc(UART_DEBUG_struct, 0x0A);
                DSPIC_LED1_WR = !DSPIC_LED1_WR;
                UART_putstr_dma(UART_MKB1_struct, "\r\n-Test UART MKB1 with DMA !!!-\r\n");
                DSPIC_LED2_WR = !DSPIC_LED2_WR;
                UART_putstr_dma(UART_MKB2_struct, "\r\n-Test UART MKB2 with DMA !!!-\r\n");
            }
        }
    }
    return 0;
}