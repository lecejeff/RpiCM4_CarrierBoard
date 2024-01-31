/*
 * File:   FT8XX.h
 * Author: Intellitrol
 *
 * Created on 28 juin 2016, 09:13
 */

#ifndef __FT8XX_H_
#define __FT8XX_H_

#include "FT_Gpu.h"
#include "spi.h"
#include "FT8XX_user_definition.h"

typedef struct
{
    // HW related variables
    STRUCT_SPI *spi;
    STRUCT_TOUCH *touch;
    uint8_t DMA_tx_channel;
    uint8_t DMA_rx_channel;
    uint8_t SPI_channel;
    
    // Pointer for DMA FIFO operation
    uint16_t DMA_wr_ptr;
    
    // BT8XX related variables
    uint16_t cmdBufferRd;
    uint16_t cmdBufferWr;
    uint16_t cmdOffset;
    uint8_t touch_tag;
    uint8_t duty_cycle;
    uint8_t gpio;
    uint8_t red_id;
      
}STRUCT_BT8XX;

#define BT8XX_QTY   1
#define BT8XX_1     0

//Structure definition for FT8XX primitives. Define a number for each of them in
//FT8XX_user_define.h. 

//Module initializations functions
void FT8XX_init (STRUCT_BT8XX *eve, STRUCT_SPI *spi, uint8_t SPI_channel, uint8_t DMA_tx_channel, uint8_t DMA_rx_channel); 
void FT8XX_set_touch_tag (uint8_t prim_type, uint8_t prim_num, uint8_t tag_num);
uint8_t FT8XX_read_touch_tag (STRUCT_BT8XX *eve);
uint8_t FT8XX_get_touch_tag (STRUCT_BT8XX *eve);
void FT8XX_clear_touch_tag (STRUCT_BT8XX *eve);
void FT8XX_touchpanel_calibrate (STRUCT_BT8XX *eve);
STRUCT_TOUCH FT8XX_touchpanel_read (STRUCT_BT8XX *eve, STRUCT_TOUCH touch_read);

// Non-DMA implementation
void FT8XX_start_new_dl (STRUCT_BT8XX *eve);
void FT8XX_update_screen_dl (STRUCT_BT8XX *eve);  
void FT8XX_write_dl_char (STRUCT_BT8XX *eve, uint8_t data);
void FT8XX_write_dl_int (STRUCT_BT8XX *eve, uint16_t data);
void FT8XX_write_dl_long (STRUCT_BT8XX *eve, uint32_t data);
void FT8XX_host_command (STRUCT_BT8XX *eve, uint8_t cmd);
void FT8XX_wr8 (STRUCT_BT8XX *eve, uint32_t adr, uint8_t data);
void FT8XX_wr16(STRUCT_BT8XX *eve, uint32_t adr, uint16_t data);
void FT8XX_wr32(STRUCT_BT8XX *eve, uint32_t adr, uint32_t data);
uint8_t FT8XX_rd8 (STRUCT_BT8XX *eve, uint32_t adr);
uint16_t FT8XX_rd16 (STRUCT_BT8XX *eve, uint32_t adr);
uint32_t FT8XX_rd32 (STRUCT_BT8XX *eve, uint32_t adr);

// DMA implementation
uint8_t FT8XX_DMA_start_new_dl (STRUCT_BT8XX *eve);
void FT8XX_DMA_write_dl_char (STRUCT_BT8XX *eve, uint8_t data);
void FT8XX_DMA_write_dl_int (STRUCT_BT8XX *eve, uint8_t data);
void FT8XX_DMA_write_dl_long (STRUCT_BT8XX *eve, uint8_t data);
void FT8XX_DMA_update_screen_dl (STRUCT_BT8XX *eve);
void FT8XX_DMA_wr8 (STRUCT_BT8XX *eve, uint32_t adr, uint8_t data);     // Add 1 byte to FIFO
void FT8XX_DMA_wr16(STRUCT_BT8XX *eve, uint32_t adr, uint16_t data);    // Add 2 byte to FIFO
void FT8XX_DMA_wr32(STRUCT_BT8XX *eve, uint32_t adr, uint32_t data);    // Add 4 byte to FIFO

uint16_t FT8XX_inc_cmd_offset (uint16_t cur_off, uint8_t cmd_size);
uint16_t FT8XX_get_cmd_offset_value (STRUCT_BT8XX *eve);

void FT8XX_clear_screen (STRUCT_BT8XX *eve, uint32_t color);
void FT8XX_set_context_color (STRUCT_BT8XX *eve, uint32_t color);
void FT8XX_set_context_bcolor (STRUCT_BT8XX *eve, uint32_t color);
void FT8XX_set_context_fcolor (STRUCT_BT8XX *eve, uint32_t color);

// 
void FT8XX_draw_line_h (STRUCT_BT8XX *eve, uint16_t x1, uint16_t x2, uint16_t y, uint16_t w);
void FT8XX_draw_line_v (STRUCT_BT8XX *eve, uint16_t y1, uint16_t y2, uint16_t x, uint16_t w);
void FT8XX_draw_point (STRUCT_BT8XX *eve, uint16_t x, uint16_t y, uint16_t r);
void FT8XX_modify_element_string (uint8_t number, uint8_t type, char *str);  
void FT8XX_write_bitmap (STRUCT_BT8XX *eve, const uint8_t *img_ptr, const uint8_t *lut_ptr, uint32_t img_length, uint32_t base_adr);    

// Commands not related to graphics primitives
void FT8XX_CMD_tracker(STRUCT_BT8XX *eve, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t tag);
void FT8XX_CMD_sketch (STRUCT_BT8XX *eve, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format);
void FT8XX_CMD_interrupt (STRUCT_BT8XX *eve, uint32_t ms);
void FT8XX_CMD_append (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t num);

// FT8XX memory related commands
void FT8XX_CMD_memzero (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t num);
uint32_t FT8XX_CMD_memcrc (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t num);
void FT8XX_CMD_memset (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t value, uint32_t num);

#if MAX_GRADIENT_NB > 0
 void FT8XX_CMD_gradient(uint8_t number, uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1);
 void FT8XX_draw_gradient (STRUCT_BT8XX *eve, STGradient *st_Gradient);
 void FT8XX_modify_gradient (STGradient *st_Gradient, uint8_t type, uint32_t value);
 uint8_t FT8XX_get_gradient_nb (void);
#endif

#if MAX_KEYS_NB > 0
 void FT8XX_CMD_keys(uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t opt, char *str);
 void FT8XX_draw_keys(STRUCT_BT8XX *eve, STKeys *st_Keys);
 void FT8XX_modify_keys (STKeys *st_Keys, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_keys_nb (void);
#endif

#if MAX_WINDOW_NB > 0
 uint8_t FT8XX_check_window (STWindow *st_Window, STRUCT_TOUCH touch_data);
 uint8_t FT8XX_check_touch (STWindow *st_Window, STRUCT_TOUCH touch_data);
 void FT8XX_CMD_window (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
 uint8_t FT8XX_heck_screen_press (STRUCT_TOUCH touch_data);
#endif

#if MAX_SLIDER_NB > 0
 void FT8XX_CMD_slider (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t v, uint16_t r);
 //uint16_t slider_update (STTouch touch_read, STSlider *st_Slider, STWindow *st_Window);
 void FT8XX_draw_slider (STRUCT_BT8XX *eve, STSlider *st_Slider);
 void FT8XX_modify_slider (STSlider *st_Slider, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_slider_nb (void);
#endif

#if MAX_TEXT_NB > 0
 void FT8XX_CMD_text (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, const char *str);
 void FT8XX_draw_text (STRUCT_BT8XX *eve, STText *st_Text);
 uint8_t FT8XX_get_text_nb (void);
#endif

#if MAX_NUMBER_NB > 0
 void FT8XX_CMD_number (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, uint32_t n);
 void FT8XX_draw_number (STRUCT_BT8XX *eve, STNumber *st_Number);
 void FT8XX_modify_number (STNumber *st_Number, uint8_t type, uint32_t value);
 uint8_t FT8XX_get_number_nb (void);
#endif

#if MAX_BUTTON_NB > 0
 void FT8XX_CMD_button (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t o, const char *str);
 void FT8XX_draw_button (STRUCT_BT8XX *eve, STButton *st_Button);
 void FT8XX_modify_button (STButton *st_Button, uint8_t type, uint16_t value);
 void FT8XX_modify_button_string (STButton *st_Button, char *str, uint8_t length);
 uint8_t FT8XX_get_button_nb (void);
#endif

#if MAX_TOGGLE_NB > 0
 void FT8XX_CMD_toggle (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t f, uint16_t o, uint8_t state, const char *str);
 void FT8XX_draw_toggle (STRUCT_BT8XX *eve, STToggle *st_Toggle);
 void FT8XX_change_toggle_state (STToggle *st_Toggle, uint8_t state);
 uint8_t FT8XX_get_toggle_nb (void);
#endif

#if MAX_CLOCK_NB > 0
 void FT8XX_CMD_clock (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint8_t h, uint8_t m, uint8_t s, uint8_t ms);
 void FT8XX_modify_clock_hms (STClock *st_Clock, uint8_t h, uint8_t m, uint8_t s);
 void FT8XX_draw_clock (STRUCT_BT8XX *eve, STClock *st_Clock);
 uint8_t FT8XX_get_clock_nb (void);
#endif

#if MAX_SCROLLBAR_NB > 0
 void FT8XX_CMD_scrollbar (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t size, uint16_t range);
 void FT8XX_draw_scrollbar  (STRUCT_BT8XX *eve, STScrollbar *st_Scrollbar);
 void FT8XX_modify_scrollbar (STScrollbar *st_Scrollbar, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_scrollbar_nb (void);
#endif

#if MAX_PROGRESS_NB > 0
 void FT8XX_CMD_progress (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t range);
 void FT8XX_draw_progress (STRUCT_BT8XX *eve, STProgress *st_Progress);
 void FT8XX_modify_progress (STProgress *st_Progress, uint8_t val);
 uint8_t FT8XX_get_progress_nb (void);
#endif

#if MAX_GAUGE_NB > 0
 void FT8XX_CMD_gauge (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t maj, uint16_t min, uint16_t val, uint16_t range);
 void FT8XX_draw_gauge (STRUCT_BT8XX *eve, STGauge *st_Gauge);
 void FT8XX_modify_gauge (STGauge *st_Gauge, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_gauge_nb (void);
#endif

#if MAX_DIAL_NB > 0
 void FT8XX_CMD_dial (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t val);
 void FT8XX_draw_dial (STRUCT_BT8XX *eve, STDial *st_Dial);
 void FT8XX_modify_dial (STDial *st_Dial, uint8_t type, uint16_t value);
 uint8_t FT8XX_get_dial_nb (void);
#endif

#if MAX_RECT_NB > 0
 void FT8XX_CMD_rectangle (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t w);
 void FT8XX_draw_rectangle (STRUCT_BT8XX *eve, STRectangle *st_Rectangle);
 uint8_t FT8XX_get_rectangle_nb (void);
#endif  
 


#endif
