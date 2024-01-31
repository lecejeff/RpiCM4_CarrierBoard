//****************************************************************************//
// File      :  dspic_33ck_generic.c
//
// Includes  :  dspic_33ck_generic.h
//
// Purpose   :  Various basic dsPIC33CK functions such as:
//              Oscillator configuration
//              I/O ports default initialization
//              
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#include "dspic_33ck_generic.h"
STRUCT_BUTTON BUTTON_struct[BTN_QTY];
STRUCT_LED LED_struct[LED_QTY];

void dspic_init(void)
{ 
    //INTCON1bits.NSTDIS = 1;                         // Nested interrupt disabled# 
    RCONbits.SWDTEN=0;                              // Watchdog timer disabled 
    
    dspic_posc_20MHz_init();
    
    // At power-up, disable all analog pins
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELF = 0;
}

// This function setups dspic to 200MHz (100MIPS) operation @ 20MHz POSC
void dspic_posc_20MHz_init (void)
{
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    // Input HS OSC 20MHz. Output required is 200MHz for 100MIPS  
    // Where Fpllo = Fplli * (M / (N1 x N2 x N3))
    // Where Fcy = Fpplo / 4 (Fpplo / 2 = Fosc and Fcy = Fosc / 2)
    // M = PLLFBDIV
    // N1 = PLLPRE
    // N2 = POST1DIV
    // N3 = POST2DIV
    //
    // For 200MHz operation (100MIPS), the PLL pre and postscaler values are :
    // PLLFBDIV = 60,   M = 60
    // PLLPRE = 1,      N1 = 1
    // POST1DIV = 3,    N2 = 3
    // POST2DIV = 1,    N3 = 1
    // Fpllo = 20MHz * (60 / (1*3*1)) = 400MHz
    CLKDIVbits.PLLPRE = 1;  
    PLLFBDbits.PLLFBDIV = 60; 
    PLLDIVbits.POST1DIV = 3;
    PLLDIVbits.POST2DIV = 1;
    
    // Initiate Clock Switch to Primary oscillator with PLL
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);    // While COSC doesn't read back Primary Oscillator config
    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN!= 0);
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
}

// This function enables the SOSC circuitry inside the dsPIC. It will use the
// 32.768kHz low-power crystal on dsPeak to drive the Real-time clock circuit
void dspic_sosc_32kHz_init (void)
{
    __builtin_write_OSCCONL(OSCCON | 0x02); // Enable secondary oscillator
    while(OSCCONbits.OSWEN != 1);           // Wait until oscillator ready
}

uint8_t dspic_button_init (STRUCT_BUTTON *btn, uint8_t channel, uint16_t debounce_thres)
{
    switch (channel)
    {
        case BTN_1:
            DSPIC_BTN1_DIR = INPUT;
            break;
            
        case BTN_2:
            DSPIC_BTN2_DIR = INPUT;
            break;
            
        default:
            return 0;
            break;
    }
    btn->channel = channel;
    btn->debounce_thres = debounce_thres;
    btn->debounce_cnt = 1;
    btn->state = HIGH;          // Default state is high because of the on-board pull-up
    btn->prev_state = HIGH;     // Default state is high because of the on-board pull-up
    btn->debounce_state = HIGH; // Default state is high because of the on-board pull-up
    btn->do_once = 0;
    return 1;
}

uint8_t dspic_button_debounce (STRUCT_BUTTON *btn)
{
    switch (btn->channel)
    {
        case BTN_1:
            btn->prev_state = btn->state;
            btn->state = DSPIC_BTN1_RD;
            break;
            
        case BTN_2:
            btn->prev_state = btn->state;
            btn->state = DSPIC_BTN2_RD;
            break;
            
        default: 
            return 0;
            break;
    }
    
    // Button is held low
    if ((btn->state == btn->prev_state) && (btn->state == LOW))
    {
        if (++btn->debounce_cnt > btn->debounce_thres)
        {
            btn->debounce_cnt = btn->debounce_thres;
            btn->debounce_state = LOW;
        }
    }
    else
    {
        if (--btn->debounce_cnt < 1)
        {
            btn->debounce_cnt = 1;
            btn->debounce_state = HIGH;
        }
    }
    return 1;
}

uint8_t dspic_button_get_state (STRUCT_BUTTON *btn)
{
    if ((btn->channel >= 0) && (btn->channel < BTN_QTY))
    {
        return btn->debounce_state;
    }
    else
        return HIGH;
}

uint8_t dspic_led_init (STRUCT_LED *led, uint8_t channel, uint8_t value)
{
    switch (channel)
    {
        case LED_1:
            DSPIC_LED1_DIR = OUTPUT;
            DSPIC_LED1_WR = value;
            break;
            
        case LED_2:
            DSPIC_LED2_DIR = OUTPUT;
            DSPIC_LED2_WR = value;
            break;
            
        default:
            return 0;
            break;
    }
    led->channel = channel;
    led->state = value;          // Default state is high because of the on-board pull-up
    led->prev_state = value;     // Default state is high because of the on-board pull-up    
    return 1;
}

uint8_t dspic_led_write (STRUCT_LED *led, uint8_t value)
{
    switch (led->channel)
    {
        case LED_1:
            DSPIC_LED1_WR = value;
            break;
            
        case LED_2:
            DSPIC_LED2_WR = value;
            break;
            
        default:
            return 0;
            break;
    }    
    led->prev_state = led->state;
    led->state = value;
    return 1;
}

void hex_to_ascii (uint8_t in, uint8_t *out_h, uint8_t *out_l)
{
    *out_h = ((in & 0xF0)>>4);	   
    *out_l = (in & 0x0F);
    if ((*out_h < 10) && (*out_h >= 0)) 
    {
        *out_h = *out_h + 0x30;			
    }
    
    else if ((*out_h >= 0x0A) && (*out_h <= 0x0F))
    {
        *out_h = (*out_h + 0x37);		
    }
    
    if ((*out_l < 10) && (*out_l >= 0))	
    {
        *out_l = (*out_l + 0x30);		
    }
    
    else if ((*out_l >= 0x0A) && (*out_l <= 0x0F))
    {
        *out_l = (*out_l + 0x37);
    }   
}

int16_t bcd_to_decimal (uint8_t bcd)
{
    return bcd - 6 * (bcd >> 4);
}

uint16_t dec_to_bcd (uint16_t dec)
{
    uint16_t ones = 0;
    uint16_t tens = 0;
    uint16_t temp = 0;

    ones = dec%10;
    temp = dec/10;
    tens = temp<<4;
    return (tens + ones);
}

uint8_t hex_to_dec (uint8_t hex)
{
    uint8_t val=0;
    uint8_t decimal=0;
    if(hex>='0' && hex<='9')  
    {  
        val = hex - 48;  
    }  
    else if(hex>='a' && hex<='f')  
    {  
        val = hex - 97 + 10;  
    }  
    else if(hex>='A' && hex<='F')  
    {  
        val = hex - 65 + 10;  
    }
    decimal = val * pow(16, 1);  
    return decimal;    
}

void uint16_to_byte8 (uint16_t *ptr_16, uint8_t *ptr_8, uint16_t length)
{
    uint16_t i = 0;
    for (; i < length; i++)
    {
        *ptr_8++ = ((*ptr_16 & 0xFF00) >> 8);
        *ptr_8++ = (*ptr_16++ & 0x00FF);
    }
}

void byte8_to_uint16 (uint8_t *ptr_8, uint16_t *ptr_16, uint16_t length)
{
    uint16_t i = 0;
    for (; i<length; i++)
    { 
        *ptr_16 = ((*ptr_8++ << 8) & 0xFF00);
        *ptr_16 = (*ptr_16 | *ptr_8++);
        ptr_16++;
    }
}