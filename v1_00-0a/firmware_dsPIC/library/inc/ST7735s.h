#ifndef __ST7735S_H_
#define	__ST7735S_H_

dspic_33ck_generic
#include "PMP.h"

#define ST7735_CMD_NOP          0x00
#define ST7735_CMD_SWRESET      0x01
#define ST7735_CMD_RDDID        0x04
#define ST7735_CMD_RDDST        0x09
#define ST7735_CMD_RDDPM        0x0A
#define ST7735_CMD_RDDMADCTL    0x0B
#define ST7735_CMD_RDDCOLMOD    0x0C
#define ST7735_CMD_RDDIM        0x0D
#define ST7735_CMD_RDDSM        0x0E
#define ST7735_CMD_RDDSDR       0x0F
#define ST7735_CMD_SLPIN        0x10
#define ST7735_CMD_SLPOUT       0x11
#define ST7735_CMD_PTLON        0x12
#define ST7735_CMD_NORON        0x13
#define ST7735_CMD_INVOFF       0x20
#define ST7735_CMD_INVON        0x21
#define ST7735_CMD_GAMSET       0x26
#define ST7735_CMD_DISPOFF      0x28
#define ST7735_CMD_DISPON       0x29
#define ST7735_CMD_CASET        0x2A
#define ST7735_CMD_RASET        0x2B
#define ST7735_CMD_RAMWR        0x2C
#define ST7735_CMD_RGBSET       0x2D
#define ST7735_CMD_RAMRD        0x2E
#define ST7735_CMD_PTLAR        0x30
#define ST7735_CMD_SCRLAR       0x33
#define ST7735_CMD_TEOFF        0x34
#define ST7735_CMD_TEON         0x35
#define ST7735_CMD_MADCTL       0x36
#define ST7735_CMD_VSCSAD       0x37
#define ST7735_CMD_IDMOFF       0x38
#define ST7735_CMD_IDMON        0x39
#define ST7735_CMD_COLMOD       0x3A
#define ST7735_CMD_RDID1        0xDA
#define ST7735_CMD_RDID2        0xDB
#define ST7735_CMD_RDID3        0xDC


void ST7735_init (void);
void ST7735_write_uint8 (uint8_t data);
void ST7735_write_uint16 (uint16_t y);
void ST7735_write_command (uint8_t command);
void ST7735_SetPos(uint8_t x0,uint8_t x1,uint8_t y0,uint8_t y1);
void ST7735_Clear(uint16_t bColor);
uint16_t RGB888_to_RGB565 (uint32_t color);

#endif	

