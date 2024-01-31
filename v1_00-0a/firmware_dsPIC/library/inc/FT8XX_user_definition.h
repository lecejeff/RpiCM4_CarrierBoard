/*
 * File:   __FT8XX_USER_DEFINITION_H__
 * Author: Intellitrol
 *
 * Created on 28 juin 2016, 09:13
 */

#ifndef __FT8XX_user_definition_H_
#define __FT8XX_user_definition_H_

#include <stdint.h>

// Physical mapping of the FT8XX GPIOs excluding the SPI port
#define FT8XX_nPD_PIN LATBbits.LATB10
#define FT8XX_nINT_PIN PORTBbits.RB9

//#define MODE_I2C          //define physical layer
#define MODE_SPI

#define RIVERDI_480x272_RESISTIVE_FT800
#ifdef RIVERDI_480x272_RESISTIVE_FT800
    #define TOUCH_PANEL_RESISTIVE
    #define SCREEN_WIDTH 480
    #define SCREEN_HEIGHT 272
#endif

//#define RIVERDI_800x480_CAPACITIVE_FT813
#ifdef RIVERDI_800x480_CAPACITIVE_FT813
    #define TOUCH_PANEL_CAPACITIVE 
    #define SCREEN_WIDTH 800
    #define SCREEN_HEIGHT 480
#endif

// Definitions of the amount of primitives to compile
#define MAX_STR_LEN 128
#define MAX_RECT_NB 1
#define MAX_GRADIENT_NB 1
#define MAX_SLIDER_NB 1
#define MAX_BUTTON_NB 0
#define MAX_TEXT_NB 4
#define MAX_NUMBER_NB 0
#define MAX_TOGGLE_NB 0
#define MAX_DIAL_NB 0
#define MAX_PROGRESS_NB 0
#define MAX_SCROLLBAR_NB 0
#define MAX_CLOCK_NB 0
#define MAX_GAUGE_NB 0
#define MAX_KEYS_NB 4

#if MAX_GRADIENT_NB > 0
#define GRADIENT_X0     0
#define GRADIENT_Y0     1
#define GRADIENT_RGB0   2
#define GRADIENT_X1     3
#define GRADIENT_Y1     4
#define GRADIENT_RGB1   5
typedef struct
{
    uint16_t x0;
    uint16_t y0;
    uint32_t rgb0;
    uint16_t x1;
    uint16_t y1;
    uint32_t rgb1;
    uint8_t touch_tag;
}STGradient;
extern STGradient st_Gradient[MAX_GRADIENT_NB];
#endif

#if MAX_KEYS_NB > 0
#define KEYS_X        0
#define KEYS_Y        1
#define KEYS_WIDTH        2
#define KEYS_HEIGHT        3
#define KEYS_FONT     4
#define KEYS_OPT      5
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t f;
    uint16_t opt;
    uint8_t len;
    uint8_t touch_tag;
    char str[MAX_STR_LEN];
}STKeys;
extern STKeys st_Keys[MAX_KEYS_NB];
#endif

#if MAX_SCROLLBAR_NB > 0
#define SCROLLBAR_X         0
#define SCROLLBAR_Y         1
#define SCROLLBAR_WIDTH     2
#define SCROLLBAR_HEIGHT    3
#define SCROLLBAR_OPT       4
#define SCROLLBAR_VAL       5
#define SCROLLBAR_SIZE      6
#define SCROLLBAR_RANGE     7
typedef struct
{
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
   uint16_t opt;
   uint16_t val;
   uint16_t size;
   uint16_t range;
   uint8_t touch_tag;
}STScrollbar;
extern STScrollbar st_Scrollbar[MAX_SCROLLBAR_NB]; // scroller struct initialization
#endif

#if MAX_DIAL_NB > 0
#define DIAL_X      0
#define DIAL_Y      1
#define DIAL_R      2
#define DIAL_OPT    3
#define DIAL_VALUE  4
typedef struct
{
   uint16_t x;
   uint16_t y;
   uint16_t r;
   uint16_t opt;
   uint16_t val;
   uint8_t touch_tag;
}STDial;
extern STDial st_Dial[MAX_DIAL_NB]; // dial struct initialization
#endif

#if MAX_GAUGE_NB > 0
#define GAUGE_X         0
#define GAUGE_Y         1
#define GAUGE_RADIUS    2
#define GAUGE_OPT       3
#define GAUGE_MAJ       4
#define GAUGE_MIN       5
#define GAUGE_VAL       6
#define GAUGE_RANGE     7
typedef struct
{
   uint16_t x;
   uint16_t y;
   uint16_t r;
   uint16_t opt;
   uint16_t maj;
   uint16_t min;
   uint16_t val;
   uint16_t range;
   uint8_t touch_tag;
}STGauge;
extern STGauge st_Gauge[MAX_GAUGE_NB]; // gauge struct initialization
#endif

#if MAX_PROGRESS_NB > 0
typedef struct
{
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
   uint16_t opt;
   uint16_t val;
   uint16_t range;
   uint8_t touch_tag;
}STProgress; 
extern STProgress st_Progress[MAX_PROGRESS_NB]; // progress struct initialization
#endif

#if MAX_CLOCK_NB > 0
typedef struct
{
   uint16_t x;
   uint16_t y;
   uint16_t r;
   uint16_t opt;
   uint8_t h;
   uint8_t m;
   uint8_t s;
   uint8_t ms;
   uint8_t touch_tag;
}STClock;
extern STClock st_Clock[MAX_CLOCK_NB]; // clock struct initialization 
#endif   

#if MAX_TOGGLE_NB > 0
typedef struct
{
   uint16_t x1;
   uint16_t y1;
   uint16_t w;
   uint16_t f;
   uint16_t opt;
   uint8_t len;
   char str[MAX_STR_LEN];
   uint16_t state;
   uint8_t touch_tag;
}STToggle;
extern STToggle st_Toggle[MAX_TOGGLE_NB]; // toggle struct initialization
#endif  

#if MAX_RECT_NB > 0
typedef struct
{
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
    uint16_t w;
    uint8_t touch_tag;
}STRectangle;
extern STRectangle st_Rectangle[MAX_RECT_NB]; // rectangle struct initialization 
#endif

#if MAX_BUTTON_NB > 0
#define BUTTON_X        0
#define BUTTON_Y        1
#define BUTTON_W        2
#define BUTTON_H        3
#define BUTTON_FONT     4
#define BUTTON_OPT      5
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t font;
    uint16_t opt;
    uint8_t len;
    uint8_t state;
    char str[MAX_STR_LEN];
    uint8_t touch_tag;
}STButton;
extern STButton st_Button[MAX_BUTTON_NB]; // button struct initialization
#endif

#if MAX_NUMBER_NB > 0
#define NUMBER_X        0
#define NUMBER_Y        1
#define NUMBER_FONT     2
#define NUMBER_OPT      3
#define NUMBER_VAL      4
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t font;
    uint16_t opt;
    uint32_t num;
    uint8_t touch_tag;
}STNumber;
extern STNumber st_Number[MAX_NUMBER_NB]; // number struct initialization
#endif

#if MAX_TEXT_NB > 0
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t font;
    uint16_t opt;
    uint8_t len;
    char str[MAX_STR_LEN];
    uint8_t touch_tag;
}STText;
extern STText st_Text[MAX_TEXT_NB]; // text struct initialization
#endif

#if MAX_SLIDER_NB > 0
#define SLIDER_X        0
#define SLIDER_Y        1
#define SLIDER_W        2
#define SLIDER_H        3
#define SLIDER_OPT      4
#define SLIDER_VAL      5
#define SLIDER_RANGE    6
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t opt;
    uint16_t val;
    uint16_t range;
    uint8_t touch_tag;
}STSlider;
extern STSlider st_Slider[MAX_SLIDER_NB]; // slider struct initialization
#endif

typedef struct
{
    uint16_t X0;
    uint16_t Y0;
    uint16_t X1;
    uint16_t Y1;
    uint16_t X2;
    uint16_t Y2;
    uint16_t X3;
    uint16_t Y3;
    uint16_t X4;
    uint16_t Y4;    
}STRUCT_TOUCH;
//

#define FONT_15 15
#define FONT_16 16
#define FONT_17 17
#define FONT_18 18
#define FONT_19 19
#define FONT_20 20
#define FONT_21 21
#define FONT_22 22
#define FONT_23 23
#define FONT_24 24
#define FONT_25 25
#define FONT_26 26
#define FONT_27 27
#define FONT_28 28
#define FONT_29 29
#define FONT_30 30
#define FONT_31 31

#define TOGGLE_ON 0xFFFF
#define TOGGLE_OFF 0
#define FT_TP_TEXT 0
#define NO_RANGE 0
#define MID_RANGE 50
#define FULL_RANGE 100

//
#define FT_PRIM_TOGGLE 0
#define FT_PRIM_BUTTON 1
#define FT_PRIM_TEXT 2
#define FT_PRIM_NUMBER 3
#define FT_PRIM_SLIDER 4
#define FT_PRIM_GRADIENT 5
#define FT_PRIM_KEYS 6
#define FT_PRIM_PROGRESS 7
#define FT_PRIM_SCROLLBAR 8
#define FT_PRIM_GAUGE 9
#define FT_PRIM_CLOCK 10
#define FT_PRIM_DIAL 11

// Colors - fully saturated colors defined here
#define RED         0xFF0000		// Red
#define GREEN       0x00FF00		// Green
#define BLUE        0x0000FF		// Blue
#define WHITE       0xFFFFFF		// White
#define BLACK       0x000000		// Black
#define PURPLE      0xFF00FF        // PURPLE
#define YELLOW      0xFFFF00
#define ORANGE      0xFF8000
#define GREY        0x6E6E6E
#define FOREST      0x25844C
#define BURNT       0xDFA44C
#define BROWN       0x5F4B2F
#define MAROON      0x6D1313
#define SKIN        0xF8BFBF
#define PINK        0xFF04C9
#define CORAL       0x42DB96

#endif
