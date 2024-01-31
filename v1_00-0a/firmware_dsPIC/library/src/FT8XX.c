#include "FT8XX.h"

STRUCT_BT8XX BT8XX_struct[BT8XX_QTY];

#ifdef SPI1_DMA_ENABLE
    extern __eds__ uint8_t spi1_dma_tx_buf[EVE_SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
    extern __eds__ uint8_t spi1_dma_rx_buf[EVE_SPI_BUF_LENGTH] __attribute__((eds,space(dma)));
#endif

#if MAX_KEYS_NB > 0
STKeys st_Keys[MAX_KEYS_NB];
#endif

#if MAX_GRADIENT_NB > 0
STGradient st_Gradient[MAX_GRADIENT_NB];
#endif

#if MAX_SCROLLBAR_NB > 0
STScrollbar st_Scrollbar[MAX_SCROLLBAR_NB];
#endif

#if MAX_DIAL_NB > 0
STDial st_Dial[MAX_DIAL_NB];
#endif

#if MAX_TEXT_NB > 0
STText st_Text[MAX_TEXT_NB];
#endif

#if MAX_GAUGE_NB > 0
STGauge st_Gauge[MAX_GAUGE_NB];
#endif

#if MAX_PROGRESS_NB > 0
STProgress st_Progress[MAX_PROGRESS_NB];
#endif

#if MAX_CLOCK_NB > 0
STClock st_Clock[MAX_CLOCK_NB];
#endif

#if MAX_TOGGLE_NB > 0
STToggle st_Toggle[MAX_TOGGLE_NB];
#endif

#if MAX_RECT_NB > 0
STRectangle st_Rectangle[MAX_RECT_NB];
#endif

#if MAX_BUTTON_NB > 0
STButton st_Button[MAX_BUTTON_NB];
#endif

#if MAX_NUMBER_NB > 0
STNumber st_Number[MAX_NUMBER_NB];
#endif

#if MAX_SLIDER_NB > 0
STSlider st_Slider[MAX_SLIDER_NB];
#endif

#if MAX_WINDOW_NB > 0
STWindow st_Window[MAX_WINDOW_NB];
#endif

#ifdef RIVERDI_800x480_CAPACITIVE_FT813
uint16_t ft8xx_lcd_hcycle = 928;
uint16_t ft8xx_lcd_hsize = 800;
uint16_t ft8xx_lcd_hoffset = 88;
uint16_t ft8xx_lcd_hsync0 = 40;
uint16_t ft8xx_lcd_hsync1 = 88;
uint16_t ft8xx_lcd_cspread = 1;

uint16_t ft8xx_lcd_vcycle = 525;
uint16_t ft8xx_lcd_vsize = 480;
uint16_t ft8xx_lcd_voffset = 32;
uint16_t ft8xx_lcd_vsync0 = 13;
uint16_t ft8xx_lcd_vsync1 = 16;
uint16_t ft8xx_lcd_swizzle = 0;
uint16_t ft8xx_lcd_pclk_pol = 0;
uint16_t ft8xx_lcd_pclk = 2;
#endif

#ifdef RIVERDI_480x272_RESISTIVE_FT800
uint16_t ft8xx_lcd_hcycle = 548;
uint16_t ft8xx_lcd_hsize = 480;
uint16_t ft8xx_lcd_hoffset = 43;
uint16_t ft8xx_lcd_hsync0 = 0;
uint16_t ft8xx_lcd_hsync1 = 41;
uint16_t ft8xx_lcd_cspread = 1;

uint16_t ft8xx_lcd_vcycle = 292;
uint16_t ft8xx_lcd_vsize = 272;
uint16_t ft8xx_lcd_voffset = 12;
uint16_t ft8xx_lcd_vsync0 = 0;
uint16_t ft8xx_lcd_vsync1 = 10;
uint16_t ft8xx_lcd_swizzle = 0;
uint16_t ft8xx_lcd_pclk_pol = 1;
uint16_t ft8xx_lcd_pclk = 5;
#endif

uint8_t slider_nb = 0;


//***************************uint8_t FT_init (void))*******************************//
//Description : Function initializes FT8XXX display to given parameters
//
//Function prototype : uint8_t FT_init (void)
//
//Enter params       : None
//
//Exit params        : 0 : init was a success
//                     1 : init failed due to bad processor clk
//
//Function call      : uint8_t = FT_init();
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_init (STRUCT_BT8XX *eve, STRUCT_SPI *spi, uint8_t SPI_channel, uint8_t DMA_tx_channel, uint8_t DMA_rx_channel)
{   
    eve->spi = spi;
    eve->SPI_channel = SPI_channel;
    eve->DMA_tx_channel = DMA_tx_channel;
    eve->DMA_rx_channel = DMA_rx_channel;
    
    eve->DMA_wr_ptr = 0;
    eve->cmdBufferRd = 0;
    eve->cmdBufferWr = 0;
    eve->cmdOffset = 0;
    eve->duty_cycle = 0;
    eve->red_id = 0;
    eve->gpio = 0;
    
    // Initialize FT8XX SPI port. SPI1 maximum clock frequency for full duplex is 9MHz
    // PPRE = 2, primary prescale 1:4
    // SPRE = 0, Secondary prescale 8:1
    // Fspi = FCY / 8 = 8.75MHz
    SPI_init(eve->spi, eve->SPI_channel, SPI_MODE0, 2, 0, SPI_BUF_LENGTH, SPI_BUF_LENGTH, DMA_tx_channel, DMA_rx_channel);    
                                                    
        
    // Set FT8XXX nINT pin to input
    TRISBbits.TRISB9 = 1;
    
    // Set FT8XX nPD pin to output
    TRISBbits.TRISB10 = 0;

    // Wake-up the FT8XX chip by cycling the power down pin     
    FT8XX_nPD_PIN = 1;
    __delay_ms(50);
    FT8XX_nPD_PIN = 0;
    __delay_ms(50);
    FT8XX_nPD_PIN = 1; 
    __delay_ms(50);  

    FT8XX_host_command(eve, FT8XX_ACTIVE);             //FT8XX wake_up command
    __delay_ms(50);
    FT8XX_host_command(eve, FT8XX_ACTIVE);             //FT8XX wake_up command
    __delay_ms(50);
    FT8XX_host_command(eve, FT8XX_ACTIVE);             //FT8XX wake_up command
    __delay_ms(50);  

    #ifdef FT_80X_ENABLE
    FT8XX_host_command(eve, FT8XX_CLKEXT);             //Set clock to external oscillator
    #endif

    #ifdef FT_81X_ENABLE
    FT8XX_host_command(FT8XX_CLKINT);             //Set clock to internal oscillator
    #endif
    __delay_ms(50);
    FT8XX_host_command(eve, FT8XX_CLK48M);             //FT8XX clock set to 48MHz
    __delay_ms(50);
    FT8XX_host_command(eve, FT8XX_CORERST);            //reset FT8XX core CPU
    __delay_ms(50);
    FT8XX_host_command(eve, FT8XX_GPUACTIVE);          //activate GPU
    __delay_ms(100);
    eve->red_id = FT8XX_rd8(eve, REG_ID);        //FT_read_8bit(0x102400);
    while (eve->red_id  != 0x7C)                //Check if clock switch was performed
    {
        eve->red_id  = FT8XX_rd8(eve, REG_ID);
        __delay_ms(10);
    }
    //Clock switch was a success, initialize FT8XX display parameters
    FT8XX_wr8(eve, REG_PCLK, 0);                 // no PCLK on init, wait for init done
    FT8XX_wr8(eve, REG_PWM_DUTY, 0);             // no backlight until init done

    FT8XX_wr16(eve, REG_HCYCLE,  ft8xx_lcd_hcycle);    // total number of clocks per line, incl front/back porch
    FT8XX_wr16(eve, REG_HSIZE,   ft8xx_lcd_hsize);     // active display width
    FT8XX_wr16(eve, REG_HOFFSET, ft8xx_lcd_hoffset);   // start of active line
    FT8XX_wr16(eve, REG_HSYNC0,  ft8xx_lcd_hsync0);    // start of horizontal sync pulse
    FT8XX_wr16(eve, REG_HSYNC1,  ft8xx_lcd_hsync1);    // end of horizontal sync pulse
    FT8XX_wr16(eve, REG_CSPREAD, ft8xx_lcd_cspread);

    FT8XX_wr16(eve, REG_VCYCLE,  ft8xx_lcd_vcycle);    // total number of lines per screen, incl pre/post
    FT8XX_wr16(eve, REG_VSIZE,   ft8xx_lcd_vsize);     // active display height       
    FT8XX_wr16(eve, REG_VOFFSET, ft8xx_lcd_voffset);   // start of active screen
    FT8XX_wr16(eve, REG_VSYNC0,  ft8xx_lcd_vsync0);    // start of vertical sync pulse
    FT8XX_wr16(eve, REG_VSYNC1,  ft8xx_lcd_vsync1);    // end of vertical sync pulse
    FT8XX_wr16(eve, REG_SWIZZLE,  ft8xx_lcd_swizzle);   // FT8XX output to LCD - pin order
    FT8XX_wr16(eve, REG_PCLK_POL, ft8xx_lcd_pclk_pol);  // LCD data is clocked in on this PCLK edge

    FT8XX_wr8(eve, REG_VOL_PB, ZERO);            // turn recorded audio volume down
    FT8XX_wr8(eve, REG_VOL_SOUND, ZERO);         // turn synthesizer volume down
    FT8XX_wr16(eve, REG_SOUND, 0x6000);          // set synthesizer to mute

    //***************************************
    // Write Initial Display List & Enable Display (clear screen, set ptr to 0)
    FT8XX_start_new_dl(eve);
    FT8XX_clear_screen(eve, BLACK);
    FT8XX_update_screen_dl(eve);
    eve->gpio = FT8XX_rd8(eve, REG_GPIO);  // Read the FT800 GPIO register for a read/modify/write operation
    eve->gpio = eve->gpio | 0x80;   // set bit 7 of FT800 GPIO register (DISP) - others are inputs
    FT8XX_wr8(eve, REG_GPIO, eve->gpio);  // Enable the DISP signal to the LCD panel
    FT8XX_wr8(eve, REG_PCLK, ft8xx_lcd_pclk);     // Now start clocking data to the LCD panel
    for (eve->duty_cycle = 0; eve->duty_cycle < 127; eve->duty_cycle++) //127 is full
    {
        FT8XX_wr8(eve, REG_PWM_DUTY, eve->duty_cycle); // Turn on backlight - ramp up slowly to full brighness
        __delay_ms(1);
    }
    // If you want to enable the touchpanel, uncomment the following lines
    #ifdef FT_80X_ENABLE
    FT8XX_wr8(eve, REG_TOUCH_MODE, FT8XX_TOUCH_MODE_CONTINUOUS);    //Touch enabled
    #endif

    #ifdef FT_81X_ENABLE
        FT8XX_wr8(REG_CTOUCH_MODE, FT8XX_TOUCH_MODE_CONTINUOUS);      // Touch enabled
        FT8XX_wr8(REG_CTOUCH_EXTENDED, 1);                            // Compatibility mode
    #endif
    FT8XX_touchpanel_calibrate(eve);
}

//**********************void FT_touchpanel_init (void)************************//
//Description : Function initializes FT801 touch panel, calibration too
//
//Function prototype : void FT_touchpanel_init (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : FT_touchpanel_init();
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_touchpanel_calibrate (STRUCT_BT8XX *eve)
{
    FT8XX_start_new_dl(eve);                    //Start new dlist
    FT8XX_write_dl_long(eve, CLEAR(1, 1, 1));
    FT8XX_write_dl_long(eve, CMD_CALIBRATE);           //Run self-calibration routine
    FT8XX_update_screen_dl(eve);                //update dlist
    //This while loop will end the touchpanel initialisation
    do
    {
        eve->cmdBufferRd = FT8XX_rd16(eve, REG_CMD_READ);
        eve->cmdBufferWr = FT8XX_rd16(eve, REG_CMD_WRITE);
        
    }   while ((eve->cmdBufferWr != 0) && (eve->cmdBufferWr != eve->cmdBufferRd));

    #ifdef TOUCH_PANEL_CAPACITIVE
        FT8XX_wr8(REG_CTOUCH_EXTENDED, 1);   //Compatibility touch mode
    #endif
}

void FT8XX_clear_touch_tag (STRUCT_BT8XX *eve)
{
    eve->touch_tag = 0;
}

uint8_t FT8XX_read_touch_tag (STRUCT_BT8XX *eve)
{
    static uint8_t touch_counter = 1;
    static uint8_t tag_flag = 0;
    // Check if the screen was touched
    if (((FT8XX_rd32(eve, REG_TOUCH_DIRECT_XY)) & 0x8000) == 0x0000)
    {
        if (++touch_counter > 5)
        {
            touch_counter = 5;     
            if (tag_flag == 0)
            {
                tag_flag = 1;   
                eve->touch_tag = FT8XX_rd32(eve, REG_TOUCH_TAG);                            
            }
        }
    }
    else
    {
        if (--touch_counter < 1)
        {
            touch_counter = 1;
            tag_flag = 0; 
            eve->touch_tag = 0;
        }                     
    }

    return eve->touch_tag;
}

uint8_t FT8XX_get_touch_tag (STRUCT_BT8XX *eve)
{
    return eve->touch_tag;
}

void FT8XX_set_touch_tag (uint8_t prim_type, uint8_t prim_num, uint8_t tag_num)
{  
    switch (prim_type)
    {
        #if MAX_TOGGLE_NB > 0
        case FT_PRIM_TOGGLE:
            st_Toggle[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_BUTTON_NB > 0
        case FT_PRIM_BUTTON:
            st_Button[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_TEXT_NB > 0
        case FT_PRIM_TEXT:
            st_Text[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_GRADIENT_NB > 0
        case FT_PRIM_GRADIENT:
            st_Gradient[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_KEYS_NB > 0
        case FT_PRIM_KEYS:
            st_Keys[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_PROGRESS_NB > 0
        case FT_PRIM_PROGRESS:
            st_Progress[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_SCROLLBAR_NB > 0
        case FT_PRIM_SCROLLBAR:
            st_Scrollbar[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_GAUGE_NB > 0
        case FT_PRIM_GAUGE:
            st_Gauge[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_CLOCK_NB > 0
        case FT_PRIM_CLOCK:
            st_Clock[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_DIAL_NB > 0
        case FT_PRIM_DIAL:
            st_Dial[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_NUMBER_NB > 0
        case FT_PRIM_NUMBER:
            st_Number[prim_num].touch_tag = tag_num;
        break;
        #endif

        #if MAX_SLIDER_NB > 0
        case FT_PRIM_SLIDER:
            st_Slider[prim_num].touch_tag = tag_num;
        break;
        #endif
    }
}

//**********************void FT_write_command (uint8_t command)********************//
//Description : Function write command to FT801
//
//Function prototype : void FT_write_command (uint8_t command)
//
//Enter params       : uint8_t command : command to send to FT801
//
//Exit params        : None
//
//Function call      : FT_write_command (FT800_CLK48M);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_host_command (STRUCT_BT8XX *eve, uint8_t command)
{
    uint8_t wr_data[3] = {command, 0, 0};
    SPI_load_tx_buffer(eve->spi, wr_data, 3);
    SPI_write(eve->spi, FT8XX_EVE_CS);
}

//**********************void FT_write_8bit (uint32_t adr, uint8_t data)******************//
//Description : Function writes 8 bit data to specified adr
//
//Function prototype : void FT_write_8bit (uint32_t adr, uint8_t data)
//
//Enter params       : uint32_t adr : FT801 register address
//                     uint8_t data: Data to write to register
//
//Exit params        : None
//
//Function call      : FT_write_8bit (CMD_BUTTON, OPT_NONE);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_wr8 (STRUCT_BT8XX *eve, uint32_t adr, uint8_t data)
{
    uint8_t wr_data[4] = {((adr >> 16) | MEM_WRITE), (adr>>8), adr, data};
    SPI_load_tx_buffer(eve->spi, wr_data, 4);
    SPI_write(eve->spi, FT8XX_EVE_CS);
    // byte 0 = (uint8_t)((adr >> 16) | MEM_WRITE);   // Write 24 bit ADR
    // byte 1 = (uint8_t)(adr>>8);                    // 
    // byte 2 = adr                                         //
    // byte 3 = data                                        // Write 8 bit data
}

//*******************void FT_write_16bit (uint32_t adr, uint16_t data)********************//
//Description : Function writes 16 bit data + adr to FT8XX
//
//Function prototype : void FT_write_16bit (uint32_t adr, uint16_t data)
//
//Enter params       : uint32_t adr : register adress
//                     uint16_t data: data to write
//
//Exit params        : None
//
//Function call      : FT_write_16bit(REG, 0x0A1E);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_wr16 (STRUCT_BT8XX *eve, uint32_t adr, uint16_t data)
{
    uint8_t wr_data[5] = {((adr >> 16) | MEM_WRITE), (adr>>8), adr, data, data >> 8};
    SPI_load_tx_buffer(eve->spi, wr_data, 5);
    SPI_write(eve->spi, FT8XX_EVE_CS);    
    // byte 0 = (uint8_t)((adr >> 16) | MEM_WRITE);     // Write 24 bit ADR
    // byte 1 = (uint8_t)(adr>>8);                      // 
    // byte 2 = adr;                                          //
    // byte 3 = (uint8_t)(data);                        // Write 16 bit data
    // byte 4 = (uint8_t)(data >> 8);                   // Little endian
}

//*******************void FT_write_32bit (uint32_t adr, uint32_t data)********************//
//Description : Function writes 32 bit data + adr to FT8XX
//
//Function prototype : void FT_write_32bit (uint32_t adr, uint32_t data)
//
//Enter params       : uint32_t adr : register adress
//                     uint32_t data: data to write
//
//Exit params        : None
//
//Function call      : FT_write_32bit(REG, 0x0A1E0A12);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_wr32 (STRUCT_BT8XX *eve, uint32_t adr, uint32_t data)
{
    uint8_t wr_data[7] = {((adr >> 16) | MEM_WRITE), (adr>>8), adr, data, data >> 8, data >> 16, data >> 24};
    SPI_load_tx_buffer(eve->spi, wr_data, 7);
    SPI_write(eve->spi, FT8XX_EVE_CS);      
    // byte 0 = (uint8_t)((adr >> 16) | MEM_WRITE);     // Write 24 bit ADR
    // byte 1 = (uint8_t)(adr>>8);                      //
    // byte 2 = adr;                                          //
    // byte 3 = (uint8_t)(data);                        // Writing 32 bit data
    // byte 4 = (uint8_t)(data >> 8);                   // Little endian
    // byte 5 = (uint8_t)(data >> 16);                  //
    // byte 6 = (uint8_t)(data >> 24);                  //
}

//*************************uint8_t FT_read_8bit (uint32_t adr)***************************//
//Description : Function reads 8 bit data from FT8XX
//
//Function prototype : uint8_t FT_read_8bit (uint32_t adr)
//
//Enter params       : uint32_t adr : register adress
//
//Exit params        : uint8_t : data from register
//
//Function call      : uint8_t = FT_read_8bit(0x0A1E0A12);
//
//Intellitrol  08/07/2016
//******************************************************************************
uint8_t FT8XX_rd8 (STRUCT_BT8XX *eve, uint32_t adr)
{
    uint8_t data[5] = {((adr >> 16) | MEM_READ), (adr>>8), adr, 0, 0};
    SPI_load_tx_buffer(eve->spi, data, 5);
    SPI_write(eve->spi, FT8XX_EVE_CS);    
    while(SPI_get_txfer_state(eve->spi)!= SPI_TX_COMPLETE);
    return SPI_get_rx_buffer_index(eve->spi, 4);
    // byte 0 = (uint8_t)((adr >> 16) | MEM_READ);   // Write 24 bit ADR
    // byte 1 = (uint8_t)(adr>>8);                    // 
    // byte 2 = adr                                         //
    // byte 3 = dummy                                       // send 0
    // byte 4 = rd8                                         // send 0, read 8-bit value from FT8XX
}

//*************************uint16_t FT_read_16bit (uint32_t adr)**************************//
//Description : Function reads 16 bit data from FT8XX
//
//Function prototype : uint8_t FT_read_16bit (uint32_t adr)
//
//Enter params       : uint32_t adr : register adress
//
//Exit params        : uint16_t : data from register
//
//Function call      : uint16_t = FT_read_16bit(0x0A1E0A12);
//
//Intellitrol  08/07/2016
//******************************************************************************
uint16_t FT8XX_rd16 (STRUCT_BT8XX *eve, uint32_t adr)
{
    uint8_t data_read1, data_read2;
    uint16_t rd16 = 0;    
    uint8_t data[6] = {((adr >> 16) | MEM_READ), (adr>>8), adr, 0, 0, 0};
    SPI_load_tx_buffer(eve->spi, data, 6);
    SPI_write(eve->spi, FT8XX_EVE_CS);    
    while(SPI_get_txfer_state(eve->spi)!= SPI_TX_COMPLETE);
    data_read1 = SPI_get_rx_buffer_index(eve->spi, 4);    
    data_read2 = SPI_get_rx_buffer_index(eve->spi, 5); 
    rd16 = ((data_read2 << 8) | data_read1);
    return (rd16);
    // byte 0 = (uint8_t)((adr >> 16) | MEM_READ);   // Write 24 bit ADR
    // byte 1 = (uint8_t)(adr>>8);                    // 
    // byte 2 = adr                                         //
    // byte 3 = dummy                                       // send 0
    // byte 4 = rd16, LSbyte                                // send 0, read less significant byte of the 16-bit data
    // byte 5 = rd16, MSbyte                                // send 0, read most significant byte of the 16-bit data
}

//*************************uint32_t FT_read_32bit (uint32_t adr)**************************//
//Description : Function reads 32 bit data from FT8XX
//
//Function prototype : uint8_t FT_read_32bit (uint32_t adr)
//
//Enter params       : uint32_t adr : register adress
//
//Exit params        : uint32_t : data from register
//
//Function call      : uint32_t = FT_read_16bit(0x0A1E0A12);
//
//Intellitrol  08/07/2016
//******************************************************************************
uint32_t FT8XX_rd32 (STRUCT_BT8XX *eve, uint32_t adr)
{
    uint32_t data_read1, data_read2, data_read3, data_read4;
    uint32_t rd32 = 0x00000000;  
    uint8_t data[8] = {((adr >> 16) | MEM_READ), (adr>>8), adr, 0, 0, 0, 0, 0};
    SPI_load_tx_buffer(eve->spi, data, 8);
    SPI_write(eve->spi, FT8XX_EVE_CS);    
    while(SPI_get_txfer_state(eve->spi)!= SPI_TX_COMPLETE);
    data_read1 = SPI_get_rx_buffer_index(eve->spi, 4);    
    data_read2 = SPI_get_rx_buffer_index(eve->spi, 5); 
    data_read3 = SPI_get_rx_buffer_index(eve->spi, 6);    
    data_read4 = SPI_get_rx_buffer_index(eve->spi, 7);     
    rd32 = (uint32_t)(data_read4 << 24);
    rd32 = (uint32_t)(rd32 | data_read3 << 16);
    rd32 = (uint32_t)(rd32 | data_read2 << 8);
    rd32 = (uint32_t)(rd32 | data_read1);
    return (rd32);
    // byte 0 = (uint8_t)((adr >> 16) | MEM_READ);   // Write 24 bit ADR
    // byte 1 = (uint8_t)(adr>>8);                    // 
    // byte 2 = adr                                         //
    // byte 3 = dummy                                       // send 0
    // byte 4 = rd16, 0..7                                  // send 0, read bits 0..7 of the 32-bit data
    // byte 5 = rd16, 8..15                                 // send 0, read bits 8..15 of the 32-bit data
    // byte 6 = rd32, 16..23                                // send 0, read bits 16..23 of the 32-bit data
    // byte 7 = rd32, 24..31                                // send 0, read bits 24..31 of the 32-bit data
}

//*************************void FT_start_new_dl (void)************************//
//Description : Function starts a new display list
//
//Function prototype : void FT_start_new_dl (void)
//
//Enter params       : none
//
//Exit params        : none
//
//Function call      : FT_start_new_dl();
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_start_new_dl (STRUCT_BT8XX *eve)
{
    // Read processor read/write pointers, make them equals to start new DL
    eve->cmdOffset = 0;
    do
    {
        eve->cmdBufferRd = FT8XX_rd16(eve, REG_CMD_READ);
        eve->cmdBufferWr = FT8XX_rd16(eve, REG_CMD_WRITE);
    }
    while ((eve->cmdBufferWr != 0) && (eve->cmdBufferWr != eve->cmdBufferRd));
    // Ready to print a new display list
    eve->cmdOffset = eve->cmdBufferWr;  // Offset set to begin of display buffer
    FT8XX_write_dl_long(eve, CMD_DLSTART); // Start of new display list
    FT8XX_write_dl_long(eve, CLEAR(1, 1, 1));
}

//*********************void FT_update_screen_dl (void)************************//
//Description : Function updates the actual displa with new display list
//
//Function prototype : void FT_update_screen_dl (void)
//
//Enter params       : none
//
//Exit params        : none
//
//Function call      : FT_update_screen_dl();
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_update_screen_dl (STRUCT_BT8XX *eve)
{
    FT8XX_write_dl_long(eve, FT8XX_DISPLAY());           // Request display swap
    FT8XX_write_dl_long(eve, CMD_SWAP);            // swap internal display list
    FT8XX_wr16(eve, REG_CMD_WRITE, eve->cmdOffset);     // Write list to display, now active
}

//************************void write_dl_char (uint8_t byte)************************//
//Description : Function writes char to display list
//
//Function prototype : void write_dl_char (uint8_t byte)
//
//Enter params       : unsinged char : data to write to display list
//
//Exit params        : none
//
//Function call      : write_dl_char(0xAE);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_write_dl_char (STRUCT_BT8XX *eve, uint8_t data)
{
    FT8XX_wr16(eve, RAM_CMD + eve->cmdOffset, data);          // Write data to display list
    eve->cmdOffset = FT8XX_inc_cmd_offset(eve->cmdOffset, 1); // get new cmdOffset value
}

//************************void write_dl_int (uint16_t d1)************************//
//Description : Function writes int to display list
//
//Function prototype : void write_dl_int (uint16_t d1)
//
//Enter params       : unsinged int : data to write to display list
//
//Exit params        : none
//
//Function call      : FT_write_dlint(0xAE0C);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_write_dl_int (STRUCT_BT8XX *eve, uint16_t data)
{
    FT8XX_wr16(eve, RAM_CMD + eve->cmdOffset, data);            // write data to display list
    eve->cmdOffset = FT8XX_inc_cmd_offset(eve->cmdOffset, 2);   // get new cmdOffset value
}

//**************************void write_dl_long (uint32_t cmd)*************************//
//Description : Function writes long to display list
//
//Function prototype : void write_dl_long (uint32_t cmd)
//
//Enter params       : unsinged long : data to write to display list
//
//Exit params        : none
//
//Function call      : FT_write_dlint(0xAE0CDDE3);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_write_dl_long (STRUCT_BT8XX *eve, uint32_t data)
{
    FT8XX_wr32(eve, RAM_CMD + eve->cmdOffset, data);          // write data to display list
    eve->cmdOffset = FT8XX_inc_cmd_offset(eve->cmdOffset, 4); // get new cmdOffset value
}

//****************uint16_t FT_inc_cmd_offset (uint16_t cur_off, uint8_t cmd_size)**************//
//Description : Function increments write ring buffer inside FT801, and returns
//              the new offset value to stay between a range of 0 through 4096
//
//Function prototype : uint16_t FT_inc_cmd_offset (uint16_t cur_off, uint8_t cmd_size)
//
//Enter params       : uint16_t cur_off : current pointer offset
//                     uint8_t cmd_size: cmd size to add to pointer offset
//
//Exit params        : uint16_t : new offset value
//
//Function call      : uint16_t = FT_inc_cmd_offset(CURR_OFF, 4);
//
//Intellitrol  08/07/2016
//******************************************************************************
uint16_t FT8XX_inc_cmd_offset (uint16_t cur_off, uint8_t cmd_size)
{
    uint16_t new_offset;
    new_offset = cur_off + cmd_size;     // increm new offset
    if (new_offset > 4095)               // Ring buffer size not exceeded ?
    {
        new_offset = (new_offset - 4096);
    }
    return new_offset;                   //return new offset value
}


//**********************uint16_t FT_get_cmd_offset_value (void)*********************//
//Description : Function gets actual cmdOffset value
//
//Function prototype : uint16_t FT_get_cmd_offset_value (void)
//
//Enter params       : none
//
//Exit params        : uint16_t : cmdOffset value
//
//Function call      :uint16_t = FT_get_cmd_offset_value();
//
//Intellitrol  08/07/2016
//******************************************************************************
uint16_t FT8XX_get_cmd_offset_value (STRUCT_BT8XX *eve)
{
    return (eve->cmdOffset); // return cmdOffset value
}

//*************************void FT_set_bcolor (uint32_t color)**********************//
//Description : Function set's display background color
//
//Function prototype : void FT_set_bcolor (uint32_t color)
//
//Enter params       : uint32_t : color wanted, as R/G/B value
//
//Exit params        : none
//
//Function call      : FT_set_bcolor (RED);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_set_context_bcolor (STRUCT_BT8XX *eve, uint32_t color)
{
    FT8XX_write_dl_long(eve, CMD_BGCOLOR); // Write Bcolor command to display list
    FT8XX_write_dl_long(eve, color);       // Write color
}

//*************************void FT_set_fcolor (uint32_t color)**********************//
//Description : Function set's display foreground color
//
//Function prototype : void FT_set_fcolor (uint32_t color)
//
//Enter params       : uint32_t : color wanted, as R/G/B value
//
//Exit params        : none
//
//Function call      : FT_set_fcolor (RED);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_set_context_fcolor (STRUCT_BT8XX *eve, uint32_t color)
{
    FT8XX_write_dl_long(eve, CMD_FGCOLOR); // Write Bcolor command to display lis
    FT8XX_write_dl_long(eve, color);       // Write color
}

//**************************void FT_set_color (uint32_t color)**********************//
//Description : FT directive to set new color to primitives
//
//Function prototype : void FT_set_color (uint32_t color)
//
//Enter params       : uint32_t : color wanted, as R/G/B value
//
//Exit params        : none
//
//Function call      : FT_set_color (RED);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_set_context_color (STRUCT_BT8XX *eve, uint32_t color)
{
    uint8_t R, G, B;
    B = (uint8_t)color;
    G = (uint8_t)(color >> 8);
    R = (uint8_t)(color >> 16);
    FT8XX_write_dl_long(eve, COLOR_RGB(R, G, B));
}

void FT8XX_write_bitmap (STRUCT_BT8XX *eve, const uint8_t *img_ptr, const uint8_t *lut_ptr, uint32_t img_length, uint32_t base_adr)
{
    uint32_t counter = 0;
    uint16_t lut_counter = 0;

    #ifdef FT_80X_ENABLE
    while (lut_counter < FT_RAM_PAL_SIZE)
    {
        FT8XX_wr8(eve, RAM_PAL + lut_counter, *lut_ptr++);
        lut_counter++;
    }

    while (img_length> 0)
    {
        FT8XX_wr8(eve, (RAM_G + counter++ + base_adr), *img_ptr++);
        img_length--;
    }
    #endif
}   

void FT8XX_draw_point (STRUCT_BT8XX *eve, uint16_t x, uint16_t y, uint16_t r)
{
    FT8XX_write_dl_long(eve, BEGIN(FTPOINTS));             //Begin primitive
    FT8XX_write_dl_long(eve, POINT_SIZE(r * 16));          //write line width
    FT8XX_write_dl_long(eve, VERTEX2F(x * 16, y * 16));    //draw line
}
//**void init_slider (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t v, uint16_t r)**//
//Description : Function will init a st_Slider[number] object with input values
//
//Function prototype : void init_slider (uint8_t number, uint16_t x, y, w, h, opt, v, r))
//
//Enter params       : uint8_t : number : number of stslider object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          w      : width of object
//                          h      : height of object
//                          opt    : object options (refer to FT datasheet for valid options)
//                          v      : value
//                          r      : range
//
//Exit params        : none
//
//Function call      : init_slider (0, 50, 50, 20, 100, OPT_NONE, 50, 100);
//                     //Init slider 0 at position 50,50 with a w/h of 20/100,
//                     with no special options, value of 50 over a range of 100
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_SLIDER_NB > 0
void FT8XX_CMD_slider (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t v, uint16_t r)
{    
    st_Slider[number].x = x;    //initialise variable with input values
    st_Slider[number].y = y;
    st_Slider[number].w = w;
    st_Slider[number].h = h;
    st_Slider[number].opt = opt;
    st_Slider[number].val = v;
    st_Slider[number].range = r;
    slider_nb++;
}

//*****************void FT_draw_slider (STSlider *st_Slider)*******************//
//Description : FT function to draw a touch slider, specified through STSlider
//
//Function prototype : void FT_draw_slider (STSlider *st_Slider)
//
//Enter params       : *st_Slider : slider struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_slider (st_Slider[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_slider (STRUCT_BT8XX *eve, STSlider *st_Slider)
{
    FT8XX_write_dl_long(eve, CMD_SLIDER);
    FT8XX_write_dl_int(eve, st_Slider->x);     // x
    FT8XX_write_dl_int(eve, st_Slider->y);     // y
    FT8XX_write_dl_int(eve, st_Slider->w);  // width
    FT8XX_write_dl_int(eve, st_Slider->h); // height
    FT8XX_write_dl_int(eve, st_Slider->opt);    // option
    FT8XX_write_dl_int(eve, st_Slider->val);  // 16 bit value
    FT8XX_write_dl_long(eve, st_Slider->range); // 32 bit range (stay in 4 bytes multiples)
}

void FT8XX_modify_slider (STSlider *st_Slider, uint8_t type, uint16_t value)
{
    switch (type)
    {
        case SLIDER_X:
            st_Slider->x = value;
        break;

        case SLIDER_Y:
            st_Slider->y = value;
        break;

        case SLIDER_W:
            st_Slider->w = value;
        break;

        case SLIDER_H:
            st_Slider->h = value;
        break;

        case SLIDER_OPT:
            st_Slider->opt = value;
        break;

        case SLIDER_VAL:
            if (value > st_Slider->range)
            {
                st_Slider->val = st_Slider->range;
            }
            else
            st_Slider->val = value;
        break;

        case SLIDER_RANGE:
            st_Slider->range = value;
        break;

        default:
        break;
    }
}

uint8_t FT8XX_get_slider_nb (void)
{
    return slider_nb;
}
#endif //#if MAX_SLIDER_NB > 0

//void init_button (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t o, const char *str)//
//Description : Function will init a st_Button[number] object with input values
//
//Function prototype : void init_button (uint8_t number, uint16_t x, y, w, h, o, const char *str))
//
//Enter params       : uint8_t : number : number of st_Button object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          w      : width of object
//                          h      : height of object
//                          o      : object options (refer to FT datasheet for valid options)
//                          str    : string inside the box ("Hello world")
//
//Exit params        : none
//
//Function call      : init_button (0, 50, 50, 20, 100, OPT_NONE, "Hello world");
//                     //Init button 0 at position 50,50 with a w/h of 20/100,
//                     with no special options and Hello world text
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_BUTTON_NB > 0
void FT8XX_CMD_button (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t o, const char *str)
{
    uint8_t cnt = 0, temp = 0; //initialise variable with input values
    st_Button[number].x = x;
    st_Button[number].y = y;
    st_Button[number].w = w;
    st_Button[number].h = h;
    st_Button[number].font = f;
    st_Button[number].opt = o;
    st_Button[number].state = 0;    //State is, by default, 0
    //This while loop fills st_Button struct with input string
    while (*str != 0x00)
    {
        st_Button[number].str[cnt] = *str;
        str++;
        cnt++;
    }
    
    //this part of the code ensures that transactions are 4 bytes wide. if
    //string byte # is not a multiple of 4, data is appended with null bytes
    st_Button[number].str[cnt] = 0x00;
    cnt++;
    temp = cnt & 0x0F;
    if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
    {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
        st_Button[number].str[cnt] = 0x00;
        cnt++;
        temp = cnt & 0x0F;
        }
    }
    st_Button[number].len = cnt; //length is written to struct
}

//*****************void FT_draw_button (STButton *st_Button)*******************//
//Description : FT function to draw a button, specified through STButton struct
//
//Function prototype : void FT_draw_button (STButton *st_Button)
//
//Enter params       : *st_Button : button struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_button (st_Button[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_button (STRUCT_BT8XX *eve, STButton *st_Button)
{
    uint8_t cnt = 0;
    FT8XX_write_dl_long(eve, CMD_BUTTON);
    FT8XX_write_dl_int(eve, st_Button->x);    // x position on screen
    FT8XX_write_dl_int(eve, st_Button->y);    // y position on screen
    FT8XX_write_dl_int(eve, st_Button->w);    // width
    FT8XX_write_dl_int(eve, st_Button->h);    // height
    FT8XX_write_dl_int(eve, st_Button->font); // primitive font
    FT8XX_write_dl_int(eve, st_Button->opt);  // primitive options
    while (cnt < st_Button->len)        // write button text until eos
    {
        FT8XX_write_dl_char(eve, st_Button->str[cnt]);
        cnt++;
    }
}

void FT8XX_modify_button (STButton *st_Button, uint8_t type, uint16_t value)
{
    switch (type)
    {
        case BUTTON_X:
            st_Button->x = value;
        break;

        case BUTTON_Y:  
            st_Button->y = value;
        break;

        case BUTTON_W:
            st_Button->w = value;
        break;

        case BUTTON_H:
            st_Button->h = value;
        break;

        case BUTTON_FONT:
            st_Button->font = value;
        break;

        case BUTTON_OPT:
            st_Button->opt = value;
        break;

        default:
        break; 
    }
}

#endif //#if MAX_BUTTON_NB > 0


//*****void init_text (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, const char *str)******//
//Description : Function will init a st_Text[number] object with input values
//
//Function prototype : void init_text (uint8_t number, uint16_t x, y, f, o, const char *str))
//
//Enter params       : uint8_t : number : number of st_Text object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          f      : font of object
//                          o      : object options (refer to FT datasheet for valid options)
//                          str    : string value("Hello world")
//
//Exit params        : none
//
//Function call      : init_text (0, 50, 50, 23, OPT_NONE, "Hello world");
//                     //Init text 0 at position 50,50 with font 23, no option
//                     and Hello world text
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_TEXT_NB > 0
void FT8XX_CMD_text (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, const char *str)
{
    uint8_t cnt = 0, temp = 0; //initialise variable with input values
    st_Text[number].x = x;
    st_Text[number].y = y;
    st_Text[number].font = f;
    st_Text[number].opt = o;
    //This while loop fills st_Button struct with input string
    while (*str != 0x00)
    {
        st_Text[number].str[cnt] = *str;
        str++;
        cnt++;
    }

    //this part of the code ensures that transactions are 4 bytes wide. if
    //string byte # is not a multiple of 4, data is appended with null bytes
    st_Text[number].str[cnt] = 0x00;
    cnt++;
    temp = cnt & 0x0F;
    if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
    {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
        st_Text[number].str[cnt] = 0x00;
        cnt++;
        temp = cnt & 0x0F;
        }
    }
    st_Text[number].len = cnt; //length is written to struct
}

//*******************void FT_draw_text (STText *st_Text)***********************//
//Description : Function draws text, specified by STText properties
//
//Function prototype : void FT_draw_text (STText *st_Text)
//
//Enter params       : *st_Text : text struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_text (st_Text[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_text (STRUCT_BT8XX *eve, STText *st_Text)
{
    uint8_t cnt = 0;
    FT8XX_write_dl_long(eve, CMD_TEXT);        // FT text command
    FT8XX_write_dl_int(eve, st_Text->x);       // x position on screen
    FT8XX_write_dl_int(eve, st_Text->y);       // y position on screen
    FT8XX_write_dl_int(eve, st_Text->font);    // font parameter
    FT8XX_write_dl_int(eve, st_Text->opt);     // FT text primitives options
    while (cnt < st_Text->len)      // write text until eos
    {
        FT8XX_write_dl_char(eve, st_Text->str[cnt]);
        cnt++;
    }
}

#endif //#if MAX_TEXT_NB > 0

#if MAX_GRADIENT_NB > 0
void FT8XX_CMD_gradient(uint8_t number, uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1)
{
    st_Gradient[number].x0 = x0;
    st_Gradient[number].y0 = y0;
    st_Gradient[number].rgb0 = rgb0;
    st_Gradient[number].x1 = x1;
    st_Gradient[number].y1 = y1;
    st_Gradient[number].rgb1 = rgb1;
}

void FT8XX_draw_gradient (STRUCT_BT8XX *eve, STGradient *st_Gradient)
{
    FT8XX_write_dl_long(eve, CMD_GRADIENT);        // 
    FT8XX_write_dl_int(eve, st_Gradient->x0);       //
    FT8XX_write_dl_int(eve, st_Gradient->y0);       // 
    FT8XX_write_dl_long(eve, st_Gradient->rgb0);    // 
    FT8XX_write_dl_int(eve, st_Gradient->x1);     //
    FT8XX_write_dl_int(eve, st_Gradient->y1);     //
    FT8XX_write_dl_long(eve, st_Gradient->rgb1);    //         
}

void FT8XX_modify_gradient (STGradient *st_Gradient, uint8_t type, uint32_t value)
{
    switch(type)
    {
        case GRADIENT_X0:
            st_Gradient->x0 = value;
        break;

        case GRADIENT_Y0:
            st_Gradient->y0 = value;
        break;

        case GRADIENT_RGB0:
            st_Gradient->rgb0 = value;
        break;

        case GRADIENT_X1:
            st_Gradient->x1 = value;
        break;

        case GRADIENT_Y1:
            st_Gradient->y1 = value;
        break;

        case GRADIENT_RGB1:
            st_Gradient->rgb1 = value;
        break;

        default:
        break;
    }
}

#endif

//*********void init_number (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, uint32_t n*********//
//Description : Function will init a st_Number[number] object with input values
//
//Function prototype : void init_number (uint8_t number, uint16_t x, y, f, o, uint32_t n))
//
//Enter params       : uint8_t : number : number of st_Number object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          f      : font of object
//                          o      : object options (refer to FT datasheet for valid options)
//                     uint32_t : n      : value of number (0 to 2^32)
//
//Exit params        : none
//
//Function call      : init_number (0, 50, 50, 23, OPT_NONE, 0x0A0A);
//                     //Init number 0 at position 50,50 with font 23, no option
//                     and with a value of 0x0A0A
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_NUMBER_NB > 0
void FT8XX_CMD_number (uint8_t number, uint16_t x, uint16_t y, uint16_t f, uint16_t o, uint32_t n)
{
    st_Number[number].x = x;   //initialise struct
    st_Number[number].y = y;
    st_Number[number].font = f;
    st_Number[number].opt = o;
    st_Number[number].num = n;
}

//*******************void FT_draw_number (STNumber *st_Number)*****************//
//Description : Function draws number, specified by STNumber properties
//
//Function prototype : void FT_draw_number (STNumber *st_Number)
//
//Enter params       : *st_Number : number struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_number (st_Number[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_number (STRUCT_BT8XX *eve, STNumber *st_Number)
{
    FT8XX_write_dl_long(eve, CMD_NUMBER);
    FT8XX_write_dl_int(eve, st_Number->x);    // x
    FT8XX_write_dl_int(eve, st_Number->y);    // y
    FT8XX_write_dl_int(eve, st_Number->font); // font
    FT8XX_write_dl_int(eve, st_Number->opt);  // option
    FT8XX_write_dl_long(eve, st_Number->num); // 32 bit number
}

void FT8XX_modify_number (STNumber *st_Number, uint8_t type, uint32_t value)
{
    switch (type)
    {
        case NUMBER_X:
            st_Number->x = value;
        break;

        case NUMBER_Y: 
            st_Number->y = value;
        break;

        case NUMBER_FONT:
            st_Number->font = value;
        break;

        case NUMBER_OPT:
            st_Number->opt = value;
        break;

        case NUMBER_VAL:
            st_Number->num = value;
        break;

        default:
        break;
    }
}

#endif //#if MAX_NUMBER_NB > 0

//********void init_window (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)***********//
//Description : Function will init a st_Window[number] touch window
//              A window is NOT a viewable rectangle : it's transparent and
//              used to track&detect touch tag on the display to process touch
//              input.
//
//Function prototype : void init_window (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
//
//Enter params       : uint8_t : number : number of st_Window object(0 to MAX_PRIM_XX)
//                     uint16_t : x1     : x start position
//                          y1     : y start position
//                          x2     : x end position
//                          y2     : y end position
//
//Exit params        : none
//
//Function call      : init_window (100, 50, 200, 100);
//                     Init a touch window that start from (100,50) and ends
//                     at (200,100) (x,y)
//
//                  (100,50)
//                     ____________________________________
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |____________________________________|
//                                                      (200,100)
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_WINDOW_NB > 0
void FT8XX_CMD_window (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    st_Window[number].x1 = x1;        //set struct default values
    st_Window[number].x2 = x2;
    st_Window[number].y1 = y1;
    st_Window[number].y2 = y2;
    st_Window[number].ucCntr = 1;
    st_Window[number].ucNewState = 0;
    st_Window[number].ucOldState = 0;
    st_Window[number].ucTouchGood = 0;
    st_Window[number].ucReadOK = 0;
    st_Window[number].one_touch = 0;
}
#endif //#if MAX_WINDOW_NB > 0


//****void init_rectangle (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t w)******//
//Description : Function will init a st_Rectangle[number] rectangle
//              A rectangle is viewable on the display : it writes 4 lines
//              of width w
//
//Function prototype : void init_rectangle (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t w))
//
//Enter params       : uint8_t : number : number of st_Rectangle object(0 to MAX_PRIM_XX)
//                     uint16_t : x1     : x start position
//                          y1     : y start position
//                          x2     : x end position
//                          y2     : y end position
//                           w     : width of line (refer to FT801 datasheet)
//
//Exit params        : none
//
//Function call      : init_rectangle (100, 50, 200, 100, 10);
//                     Init a rectangle that start from (100,50) and ends
//                     at (200,100) (x,y) with line width of 10
//
//                  (100,50)
//                     ____________________________________
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |                                    |
//                    |____________________________________|
//                                                      (200,100)
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_RECT_NB > 0
void FT8XX_CMD_rectangle (uint8_t number, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t w)
{
    st_Rectangle[number].x1 = x1;//set struct default values
    st_Rectangle[number].y1 = y1;
    st_Rectangle[number].x2 = x2;
    st_Rectangle[number].y2 = y2;
    st_Rectangle[number].w = w;
}

//**************void FT_draw_rectangle (STRectangle *st_Rectangle)************//
//Description : FT function to draw a rectangle from st_Rectangle parameters
//
//Function prototype : void FT_draw_rectangle (STRectangle *st_Rectangle)
//
//Enter params       : *st_Rectangle : struct which contains rectangle values
//
//Exit params        : none
//
//Function call      : FT_draw_rectangle (&strectangle[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_rectangle (STRUCT_BT8XX *eve, STRectangle *st_Rectangle)
{
    //function calls FT_draw_line_h and FT_draw_line_v which handles
    //line drawing for x and y positions
    FT8XX_draw_line_h(eve, st_Rectangle->x1, st_Rectangle->x2, st_Rectangle->y1, st_Rectangle->w);
    FT8XX_draw_line_h(eve, st_Rectangle->x1, st_Rectangle->x2, st_Rectangle->y2, st_Rectangle->w);
    FT8XX_draw_line_v(eve, st_Rectangle->y1, st_Rectangle->y2, st_Rectangle->x1, st_Rectangle->w);
    FT8XX_draw_line_v(eve, st_Rectangle->y1, st_Rectangle->y2, st_Rectangle->x2, st_Rectangle->w);
}
#endif //#if MAX_RECT_NB > 0


//void init_togglesw (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t f, uint16_t o, const char *str)//
//Description : Function will init a st_Togglesw[number] object with input values
//
//Function prototype : void init_togglesw(uint8_t number, uint16_t x, y, w, f, o, const char *str)
//
//Enter params       : uint8_t : number : number of st_Togglesw object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          w      : width of center circle
//                          f      : object font (refer to FT datasheet for valid font)
//                          o      : object options (refer to FT datasheet for valid options)
//                     const char *   : on/off toggle text, seperated by \xff
//
//Exit params        : none
//
//Function call      : init_togglesw (0, 50, 50, 20, 23, OPT_NONE, "OFF\xffON");
//                     //init togglesw at pos 50,50, with a center circle width
//                     of 20, text at font 23, NO options and both toggle state
//                     text are OFF and ON
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_TOGGLE_NB > 0
void FT8XX_CMD_toggle (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t f, uint16_t o, uint8_t state, const char *str)
{
    uint8_t cnt = 0, temp = 0;//set struct default values
    st_Toggle[number].x1 = x;
    st_Toggle[number].y1 = y;
    st_Toggle[number].w = w;
    st_Toggle[number].f = f;
    st_Toggle[number].opt = o;
    st_Toggle[number].state = state; //0 by default, FFFF = 1
    //This while loop fills st_Button struct with input string
    while (*str != 0x00)
    {
        st_Toggle[number].str[cnt] = *str;
        str++;
        cnt++;
    }

    //this part of the code ensures that transactions are 4 bytes wide. if
    //string byte # is not a multiple of 4, data is appended with null bytes
    st_Toggle[number].str[cnt] = 0x00;
    cnt++;
    temp = cnt & 0x0F;
    if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
    {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
        st_Toggle[number].str[cnt] = 0x00;
        cnt++;
        temp = cnt & 0x0F;
        }
    }
    st_Toggle[number].len = cnt;//write length to struct
}


//***************void FT_draw_togglesw (STTogglesw *st_Togglesw)**************//
//Description : Function draws a toggle switch, specified by STTogglesw properties
//
//Function prototype : void FT_draw_togglesw (STTogglesw *st_Togglesw)
//
//Enter params       : *st_Togglesw : togglesw struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_togglesw (&st_Togglesw[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_toggle (STRUCT_BT8XX *eve, STToggle *st_Toggle)
{
    uint8_t cnt = 0;
    FT8XX_write_dl_long(eve, CMD_TOGGLE);
    FT8XX_write_dl_int(eve, st_Toggle->x1);    // x
    FT8XX_write_dl_int(eve, st_Toggle->y1);    // y
    FT8XX_write_dl_int(eve, st_Toggle->w);     //
    FT8XX_write_dl_int(eve, st_Toggle->f);     // font
    FT8XX_write_dl_int(eve, st_Toggle->opt);   // option
    FT8XX_write_dl_int(eve, st_Toggle->state); // state
    while (cnt < st_Toggle->len)         // write text until eos
    {
        FT8XX_write_dl_char(eve, st_Toggle->str[cnt]);
        cnt++;
    }
}

void FT8XX_change_toggle_state (STToggle *st_Toggle, uint8_t state)
{
    st_Toggle->state = state;
}

#endif //#if MAX_TOGGLE_NB > 0


//*******void init_dial (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t val)********//
//Description : Function will init a st_Dial[number] object with input values
//
//Function prototype : void init_dial (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t val)
//
//Enter params       : uint8_t : number : number of st_Dial object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          r      : circle radius
//                          opt    : object font (refer to FT datasheet for valid font)
//                          val    : dial initial value (refer to FT datasheet for val/position relationship)
//
//Exit params        : none
//
//Function call      : init_dial (0, 50, 50, 20, OPT_NONE, 0x8000);
//                     //init dial at pos 50,50, with a center circle radius
//                     of 20, NO options and dial is at 0 oClock
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_DIAL_NB > 0
void FT8XX_CMD_dial (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t val)
{
    st_Dial[number].x = x;    //set struct default values
    st_Dial[number].y = y;
    st_Dial[number].r = r;
    st_Dial[number].opt = opt;
    st_Dial[number].val = val;
}

//********************void FT_draw_dial (STDial *st_Dial)***********************//
//Description : FT function to draw a dial, specified through st_Dial
//
//Function prototype : void FT_draw_dial (STDial *st_Dial)
//
//Enter params       : *st_Dial : st_Dial struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_dial (&st_Dial[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_dial (STRUCT_BT8XX *eve, STDial *st_Dial)
{
    FT8XX_write_dl_long(eve, CMD_DIAL);     //write FT command to draw a dial
    FT8XX_write_dl_int(eve, st_Dial->x);//write values to command
    FT8XX_write_dl_int(eve, st_Dial->y);
    FT8XX_write_dl_int(eve, st_Dial->r);
    FT8XX_write_dl_int(eve, st_Dial->opt);
    FT8XX_write_dl_int(eve, st_Dial->val);
    FT8XX_write_dl_int(eve, 0);
}

void FT8XX_modify_dial (STDial *st_Dial, uint8_t type, uint16_t value)
{
    switch (type)
    {
        case DIAL_X:
            st_Dial->x = value;
        break;

        case DIAL_Y:
            st_Dial->y = value;
        break;

        case DIAL_R:
            st_Dial->r = value;
        break;

        case DIAL_OPT:
            st_Dial->opt = value;
        break;

        case DIAL_VALUE:
            st_Dial->val = value;
        break;

        default:
        break;
    }
}

#endif //#if MAX_DIAL_NB > 0


//void init_progress (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t range)//
//Description : Function will init a st_Progress[number] object with input values
//
//Function prototype : void init_progress (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t range)
//
//Enter params       : uint8_t : number : number of st_Progress object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          w      : width of bar
//                          h      : height of bar
//                          opt    : options for object
//                          val    : bar value
//                          range  : bar min-max range
//
//Exit params        : none
//
//Function call      : init_progress (0, 50, 50, 20, 100, OPT_NONE, 50, 100);
//                   //init a progress bar from pos (50, 50), width of 20 and
//                   height of 20, with no special options and initialize it
//                   at mid position (value of 50 over a range of 100)
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_PROGRESS_NB > 0
void FT8XX_CMD_progress (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t range)
{
	st_Progress[number].x = x;        //set struct default values
	st_Progress[number].y = y;
	st_Progress[number].w = w;
	st_Progress[number].h = h;
	st_Progress[number].opt = opt;
	st_Progress[number].val = val;
	st_Progress[number].range = range;
}

//**************void FT_draw_progress (STProgress *st_Progress)***************//
//Description : FT function to draw a progress bar, specified through st_Progress
//
//Function prototype : void FT_draw_progress (STProgress *st_Progress)
//
//Enter params       : *st_Progress : st_Progress struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_progress (&st_Progress[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_progress (STRUCT_BT8XX *eve, STProgress *st_Progress)
{
    FT8XX_write_dl_long(eve, CMD_PROGRESS);        //write FT command to draw a progress bar
    FT8XX_write_dl_int(eve, st_Progress->x);       //write values to command
    FT8XX_write_dl_int(eve, st_Progress->y);
    FT8XX_write_dl_int(eve, st_Progress->w);
    FT8XX_write_dl_int(eve, st_Progress->h);
    FT8XX_write_dl_int(eve, st_Progress->opt);
    FT8XX_write_dl_int(eve, st_Progress->val);
    FT8XX_write_dl_int(eve, st_Progress->range);
    FT8XX_write_dl_int(eve, 0);
}

void FT8XX_modify_progress (STProgress *st_Progress, uint8_t val)
{
    st_Progress->val = val;
}

#endif //#if MAX_PROGRESS_NB > 0



//void init_scroller (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t size, uint16_t range)//
//Description : Function will init a st_Scroller[number] object with input values
//
//Function prototype : void init_scroller (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t size, uint16_t range)
//
//Enter params       : uint8_t : number : number of st_Scroller object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          w      : width of bar
//                          h      : height of bar
//                          opt    : options for object
//                          val    : bar value
//                          size   : size value (scroller width)
//                          range  : bar min-max range
//
//Exit params        : none
//
//Function call      : init_scroller (0, 50, 50, 20, 100, OPT_NONE, 50, 10 100);
//                   //init a scroller from pos (50, 50), width of 20 and
//                   height of 20, with no special options and initialize it
//                   at mid position (value of 50 over a range of 100) with a
//                   scroller width of 10
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_SCROLLBAR_NB > 0
void FT8XX_CMD_scrollbar (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t opt, uint16_t val, uint16_t size, uint16_t range)
{
    st_Scrollbar[number].x = x;        //set struct default values
    st_Scrollbar[number].y = y;
    st_Scrollbar[number].w = w;
    st_Scrollbar[number].h = h;
    st_Scrollbar[number].opt = opt;
    st_Scrollbar[number].val = val;
    st_Scrollbar[number].size = size;
    st_Scrollbar[number].range = range;
}

//**************void FT_draw_scroller (STScroller *st_Scroller)***************//
//Description : FT function to draw a scroller, specified through st_Scroller
//
//Function prototype : void FT_draw_scroller (STScroller *st_Scroller)
//
//Enter params       : *st_Scroller : scroller struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_scroller (&st_Scroller[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_scrollbar (STRUCT_BT8XX *eve, STScrollbar *st_Scrollbar)
{
    FT8XX_write_dl_long(eve, CMD_SCROLLBAR);    //write FT command to draw scrollbar
    FT8XX_write_dl_int(eve, st_Scrollbar->x);//write values to command
    FT8XX_write_dl_int(eve, st_Scrollbar->y);
    FT8XX_write_dl_int(eve, st_Scrollbar->w);
    FT8XX_write_dl_int(eve, st_Scrollbar->h);
    FT8XX_write_dl_int(eve, st_Scrollbar->opt);
    FT8XX_write_dl_int(eve, st_Scrollbar->val);
    FT8XX_write_dl_int(eve, st_Scrollbar->size);
    FT8XX_write_dl_int(eve, st_Scrollbar->range);
}

void FT8XX_modify_scrollbar (STScrollbar *st_Scrollbar, uint8_t type, uint16_t value)
{
    switch (type)
    {
        case SCROLLBAR_X:
            st_Scrollbar->x = value;
        break;

        case SCROLLBAR_Y:
            st_Scrollbar->y = value;
        break;

        case SCROLLBAR_WIDTH:
            st_Scrollbar->w = value;
        break;

        case SCROLLBAR_HEIGHT:
            st_Scrollbar->h = value;
        break;

        case SCROLLBAR_OPT:
            st_Scrollbar->opt = value;
        break;

        case SCROLLBAR_VAL:
            st_Scrollbar->val = value;
        break;
        
        case SCROLLBAR_SIZE:
            st_Scrollbar->size = value;
        break;

        case SCROLLBAR_RANGE:
            if (value > st_Scrollbar->range)
            {
                st_Scrollbar->val = st_Scrollbar->range;
            }
            else
            st_Scrollbar->range = value;
        break;

        default:
        break;
    }
}

#endif //#if MAX_SCROLLBAR_NB > 0


//void init_gauge (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t maj, uint16_t min, uint16_t val, uint16_t range)//
//Description : Function will init a st_Gauge[number] object with input values
//
//Function prototype : void init_gauge (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t maj, uint16_t min, uint16_t val, uint16_t range)
//
//Enter params       : uint8_t : number : number of st_Gauge object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          r      : radius of gauge
//                          opt    : options for object
//                          maj    : major divisions value
//                          min    : minor divisions value
//                          val    : initial value
//                          range  : gauge range
//
//Exit params        : none
//
//Function call      : init_gauge (0, 50, 50, 20, OPT_NONE, 10, 5, 50, 100);
//                   //init a gauge at pos (50,50) with a radius of 20, no opt,
//                   10 major divisions, each major division is divided in 5
//                   minor division, with a value of 50 over a range of 100
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_GAUGE_NB > 0
void FT8XX_CMD_gauge (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint16_t maj, uint16_t min, uint16_t val, uint16_t range)
{
    st_Gauge[number].x = x;        //set struct default values
    st_Gauge[number].y = y;
    st_Gauge[number].r = r;
    st_Gauge[number].opt = opt;
    st_Gauge[number].maj = maj;
    st_Gauge[number].min = min;
    st_Gauge[number].val = val;
    st_Gauge[number].range = range;
}

//******************void FT_draw_gauge (STGauge *st_Gauge)********************//
//Description : FT function to draw a gauge, specified through st_Gauge
//
//Function prototype : void FT_draw_gauge (STGauge *st_Gauge)
//
//Enter params       : *st_Gauge : st_Gauge struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_gauge (&st_Gauge[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_gauge (STRUCT_BT8XX *eve, STGauge *st_Gauge)
{
    FT8XX_write_dl_long(eve, CMD_GAUGE);     //write FT command to draw a gauge
    FT8XX_write_dl_int(eve, st_Gauge->x);//write values to command
    FT8XX_write_dl_int(eve, st_Gauge->y);
    FT8XX_write_dl_int(eve, st_Gauge->r);
    FT8XX_write_dl_int(eve, st_Gauge->opt);
    FT8XX_write_dl_int(eve, st_Gauge->maj);
    FT8XX_write_dl_int(eve, st_Gauge->min);
    FT8XX_write_dl_int(eve, st_Gauge->val);
    FT8XX_write_dl_int(eve, st_Gauge->range);
}

void FT8XX_modify_gauge (STGauge *st_Gauge, uint8_t type, uint16_t value)
{
    switch(type)
    {
        case GAUGE_X:
            st_Gauge->x = value;
        break;

        case GAUGE_Y:
            st_Gauge->y = value;
        break;

        case GAUGE_RADIUS:
            st_Gauge->r = value;
        break;

        case GAUGE_OPT:
            st_Gauge->opt = value;
        break;

        case GAUGE_MAJ:
            st_Gauge->maj = value;
        break;

        case GAUGE_MIN:
            st_Gauge->min = value;
        break;

        case GAUGE_VAL:
            st_Gauge->val = value;
        break;

        case GAUGE_RANGE:
            st_Gauge->range = value;
        break;

        default:
        break;
    }
}

#endif //#if MAX_GAUGE_NB > 0


//void init_clock (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint8_t h, uint8_t m, uint8_t s, uint8_t ms)//
//Description : Function will init a st_Clock[number] object with input values
//
//Function prototype : void init_clock (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint8_t h, uint8_t m, uint8_t s, uint8_t ms)
//
//Enter params       : uint8_t : number : number of st_Clock object(0 to MAX_PRIM_XX)
//                     uint16_t : x      : x position on screen
//                          y      : y position on screen
//                          r      : radius of clock
//                          opt    : options for object
//                          h      : hours value
//                          m      : minutes value
//                          s      : seconds value
//                          ms     : milliseconds value
//
//Exit params        : none
//
//Function call      : init_clock(0, 50, 50, 50, OPT_NONE, 8, 15, 0, 0);
//                     init a clock at position (50,50) with a radius of 50,
//                     no options and time set to 8:15:00:00
//
//Intellitrol  08/07/2016
//******************************************************************************
#if MAX_CLOCK_NB > 0
void FT8XX_CMD_clock (uint8_t number, uint16_t x, uint16_t y, uint16_t r, uint16_t opt, uint8_t h, uint8_t m, uint8_t s, uint8_t ms)
{
    st_Clock[number].x = x;        //set struct default values
    st_Clock[number].y = y;
    st_Clock[number].r = r;
    st_Clock[number].opt = opt;
    st_Clock[number].h = h;
    st_Clock[number].m = m;
    st_Clock[number].s = s;
    st_Clock[number].ms = ms;
}

//*******************void FT_draw_clock (STClock *st_Clock)*********************//
//Description : FT function to draw a clock, specified through STClock struct
//
//Function prototype : void FT_draw_clock (STClock *st_Clock)
//
//Enter params       : *st_Clock : clock struct including gfx parameters
//
//Exit params        : none
//
//Function call      : FT_draw_clock (&st_Clock[0]);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_clock (STRUCT_BT8XX *eve, STClock *st_Clock)
{
    FT8XX_write_dl_long(eve, CMD_CLOCK);       //write FT command to draw a clock
    FT8XX_write_dl_int(eve, st_Clock->x);  //write values to command
    FT8XX_write_dl_int(eve, st_Clock->y);
    FT8XX_write_dl_int(eve, st_Clock->r);
    FT8XX_write_dl_int(eve, st_Clock->opt);
    FT8XX_write_dl_int(eve, st_Clock->h);
    FT8XX_write_dl_int(eve, st_Clock->m);
    FT8XX_write_dl_int(eve, st_Clock->s);
    FT8XX_write_dl_int(eve, st_Clock->ms);
}

//*******void FT_modify_clock_hms (STClock *st_Clock, uint8_t h, uint8_t m, uint8_t s)*******//
//Description : FT function updates h,m,s value of input st_Clock
//
//Function prototype : void FT_modify_clock_hms (STClock *st_Clock, uint8_t h, uint8_t m, uint8_t s)
//
//Enter params       : *st_Clock : clock struct including gfx parameters
//                   : uint8_t h     : hour value
//                        m     : minute value
//                        s     : second value
//
//Exit params        : none
//
//Function call      : FT_modify_clock_hms(&st_Clock[0], 8, 15, 0);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_modify_clock_hms (STClock *st_Clock, uint8_t h, uint8_t m, uint8_t s)
{
    st_Clock->h = h;
    st_Clock->m = m;
    st_Clock->s = s;
}

#endif //#if MAX_CLOCK_NB > 0   

#if MAX_KEYS_NB > 0
void FT8XX_CMD_keys (uint8_t number, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t f, uint16_t opt, char *str)
{
    uint8_t cnt = 0, temp = 0;//set struct default values
    st_Keys[number].x = x;
    st_Keys[number].y = y;
    st_Keys[number].w = w;
    st_Keys[number].h = h;
    st_Keys[number].f = f;
    st_Keys[number].opt = opt;
    //This while loop fills st_Button struct with input string
    while (*str != 0x00)
    {
        st_Keys[number].str[cnt] = *str;
        str++;
        cnt++;
    }

    //this part of the code ensures that transactions are 4 bytes wide. if
    //string byte # is not a multiple of 4, data is appended with null bytes
    st_Keys[number].str[cnt] = 0x00;
    cnt++;
    temp = cnt & 0x0F;
    if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
    {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
        st_Keys[number].str[cnt] = 0x00;
        cnt++;
        temp = cnt & 0x0F;
        }
    }
    st_Keys[number].len = cnt;//write length to struct
}

void FT8XX_modify_keys (STKeys *st_Keys, uint8_t type, uint16_t value)
{
    switch (type)
    {
        case KEYS_X:
            st_Keys->x = value;
        break;

        case KEYS_Y:
            st_Keys->y = value;
        break;

        case KEYS_WIDTH:
            st_Keys->w = value;
        break;

        case KEYS_HEIGHT:
            st_Keys->h = value;
        break;

        case KEYS_OPT:
            st_Keys->opt = value;
        break;

        case KEYS_FONT:
            st_Keys->f = value;
        break;        

        default:
        break;
    }
}

void FT8XX_draw_keys(STRUCT_BT8XX *eve, STKeys *st_Keys)
{
    uint8_t cnt = 0;
    FT8XX_write_dl_long(eve, CMD_KEYS);
    FT8XX_write_dl_int(eve, st_Keys->x);
    FT8XX_write_dl_int(eve, st_Keys->y);
    FT8XX_write_dl_int(eve, st_Keys->w);
    FT8XX_write_dl_int(eve, st_Keys->h);
    FT8XX_write_dl_int(eve, st_Keys->f);
    FT8XX_write_dl_int(eve, st_Keys->opt);
    while (cnt < st_Keys->len)         // write text until eos
    {
        FT8XX_write_dl_char(eve, st_Keys->str[cnt]);
        cnt++;
    }
}

#endif

void FT8XX_CMD_tracker(STRUCT_BT8XX *eve, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t tag)
{
    FT8XX_write_dl_long(eve, CMD_TRACK);   
    FT8XX_write_dl_int(eve, x);  //write values to command
    FT8XX_write_dl_int(eve, y);
    FT8XX_write_dl_int(eve, w);
    FT8XX_write_dl_int(eve, h);
    FT8XX_write_dl_int(eve, tag); 
    FT8XX_write_dl_int(eve, 0);
}

void FT8XX_CMD_sketch (STRUCT_BT8XX *eve, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format)
{
    FT8XX_write_dl_long(eve, CMD_SKETCH);
    FT8XX_write_dl_int(eve, x);  //write values to command
    FT8XX_write_dl_int(eve, y);
    FT8XX_write_dl_int(eve, w);
    FT8XX_write_dl_int(eve, h);
    FT8XX_write_dl_long(eve, ptr); 
    FT8XX_write_dl_int(eve, format); 
    FT8XX_write_dl_int(eve, 0);
}

void FT8XX_CMD_memzero (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t num)
{
    FT8XX_write_dl_long(eve, CMD_MEMZERO);  
    FT8XX_write_dl_long(eve, ptr);      
    FT8XX_write_dl_long(eve, num);      
}

void FT8XX_CMD_interrupt (STRUCT_BT8XX *eve, uint32_t ms)
{
    FT8XX_write_dl_long(eve, CMD_INTERRUPT); 
    FT8XX_write_dl_long(eve, ms);                // Delay before interrupt triggers
}

void FT8XX_CMD_append (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t num)
{
    FT8XX_write_dl_long(eve, CMD_APPEND);  
    FT8XX_write_dl_long(eve, ptr);      
    FT8XX_write_dl_long(eve, num);     
}

uint32_t FT8XX_CMD_memcrc (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t num)
{
    uint16_t x = FT8XX_rd16(eve, REG_CMD_WRITE);
    FT8XX_write_dl_long(eve, CMD_APPEND);  
    FT8XX_write_dl_long(eve, ptr);      
    FT8XX_write_dl_long(eve, num);  
    do
    {
        eve->cmdBufferRd = FT8XX_rd16(eve, REG_CMD_READ);
        eve->cmdBufferWr = FT8XX_rd16(eve, REG_CMD_WRITE);
    }   while ((eve->cmdBufferWr != 0) && (eve->cmdBufferWr != eve->cmdBufferRd));
    return FT8XX_rd32(eve, RAM_CMD + x + 12);
}

void FT8XX_CMD_memset (STRUCT_BT8XX *eve, uint32_t ptr, uint32_t value, uint32_t num)
{
    FT8XX_write_dl_long(eve, CMD_MEMSET);  
    FT8XX_write_dl_long(eve, ptr);    
    FT8XX_write_dl_long(eve, value);   
    FT8XX_write_dl_long(eve, num); 
    do
    {
        eve->cmdBufferRd = FT8XX_rd16(eve, REG_CMD_READ);
        eve->cmdBufferWr = FT8XX_rd16(eve, REG_CMD_WRITE);
    }   while ((eve->cmdBufferWr != 0) && (eve->cmdBufferWr != eve->cmdBufferRd));
}


//********************void FT_clear_screen (uint32_t color)************************//
//Description : FT function to clear primitives on screen and update backgrnd
//              color with specified input color
//
//Function prototype : void FT_clear_screen (uint32_t color)
//
//Enter params       : uint32_t color : background color (0,R,G,B)(r,g,b = 1 byte each)
//
//Exit params        : none
//
//Function call      : FT_clear_screen (RED);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_clear_screen (STRUCT_BT8XX *eve, uint32_t color)
{
    FT8XX_write_dl_long(eve, CLEAR_COLOR_RGB(color >> 16, color >> 8, color)); //Write color
    FT8XX_write_dl_long(eve, CLEAR(1, 1, 1));       //clear primitives, cache and backgrnd
}

//***************void FT_draw_line_h (uint16_t x1, uint16_t x2, uint16_t y, uint16_t w)**************//
//Description : FT function to draw a horizontal line from x1 to x2, at y pos,
//              with a width of w
//
//Function prototype : void FT_draw_line_h (uint16_t x1, uint16_t x2, uint16_t y, uint16_t w)
//
//Enter params       : uint16_t x1 : x start value
//                        x2 : x end value
//                         y : y pos value
//                         w : line width value
//
//Exit params        : none
//
//Function call      : FT_draw_line_h (20, 70, 120, 10);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_line_h (STRUCT_BT8XX *eve, uint16_t x1, uint16_t x2, uint16_t y, uint16_t w)
{
    FT8XX_write_dl_long(eve, BEGIN(LINES));         //Begin primitive
    FT8XX_write_dl_long(eve, LINE_WIDTH(w));        //write line width
    FT8XX_write_dl_long(eve, VERTEX2F(x1 * 16, y * 16)); //draw line
    FT8XX_write_dl_long(eve, VERTEX2F(x2 * 16, y * 16)); //draw line
}

//***************void FT_draw_line_v (uint16_t y1, uint16_t y2, uint16_t x, uint16_t w)**************//
//Description : FT function to draw a vertical line from y1 to y2, at x pos,
//              with a width of w
//
//Function prototype : void FT_draw_line_v (uint16_t y1, uint16_t y2, uint16_t x, uint16_t w)
//
//Enter params       : uint16_t y1 : y start value
//                        y2 : y end value
//                         x : x pos value
//                         w : line width value
//
//Exit params        : none
//
//Function call      : FT_draw_line_v (20, 70, 120, 10);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_draw_line_v (STRUCT_BT8XX *eve, uint16_t y1, uint16_t y2, uint16_t x, uint16_t w)
{
    FT8XX_write_dl_long(eve, BEGIN(LINES));         //Begin primitive
    FT8XX_write_dl_long(eve, LINE_WIDTH(w));        //write line width
    FT8XX_write_dl_long(eve, VERTEX2F(x * 16, y1 * 16)); //draw line
    FT8XX_write_dl_long(eve, VERTEX2F(x * 16, y2 * 16)); //draw line
}

//*****************STTouch FT_touchpanel_read (STTouch touch_read)***********//
//Description : Function read touch panel (touch 1 to 5)
//
//Function prototype : STTouch FT_touchpanel_read (STTouch touch_read)
//
//Enter params       : STTouch : struct which contains touch values
//
//Exit params        : STTouch : struct which contains new touch values
//
//Function call      : touch_data = FT_touchpanel_read (touch_data);
//
//Intellitrol  08/07/2016
//******************************************************************************
STRUCT_TOUCH FT8XX_touchpanel_read (STRUCT_BT8XX *eve, STRUCT_TOUCH touch_read)
{
    //Function scroll through each touch register, read them and convert them
    uint32_t temp_data;
    #ifdef TOUCH_PANEL_CAPACITIVE
        
        temp_data = FT8XX_rd32(REG_CTOUCH_TOUCH0_XY);
        touch_read.X0 = (temp_data >> 16);
        touch_read.Y0 = temp_data;


        temp_data = FT8XX_rd32(REG_CTOUCH_TOUCH1_XY);
        touch_read.X1 = (temp_data >> 16);
        touch_read.Y1 = temp_data;


        temp_data = FT8XX_rd32(REG_CTOUCH_TOUCH2_XY);
        touch_read.X2 = (temp_data >> 16);
        touch_read.Y2 = temp_data;


        temp_data = FT8XX_rd32(REG_CTOUCH_TOUCH3_XY);
        touch_read.X3 = (temp_data >> 16);
        touch_read.Y3 = temp_data;


        touch_read.X4 = FT8XX_rd16(REG_CTOUCH_TOUCH4_X);
        touch_read.Y4 = FT8XX_rd16(REG_CTOUCH_TOUCH4_Y);
        return touch_read;   //return new struct with new values
    #endif

    #ifdef TOUCH_PANEL_RESISTIVE
        temp_data = FT8XX_rd32(eve, REG_TOUCH_SCREEN_XY);
        touch_read.X1 = (temp_data >> 16);
        touch_read.Y1 = temp_data;
        return touch_read;   //return new struct with new values
    #endif
}

//******void FT_modify_element_string (uint8_t number, uint8_t type, const char *str)*****//
//Description : Function modify input primitives string, calculate its new
//              length and append the required bytes if necessary
//
//Function prototype : void FT_modify_element_string (uint8_t number, uint8_t type, const char *str)
//
//Enter params       : number : number of the element (stBoite[number])
//                     type   : type of primitive (defined in FT8XX.h)
//                     *str   : string to modify
//
//Exit params        : None
//
//Function call      : FT_modify_element_string(0, FT_PRIM_TOGGLESW, "on\xffoff);
//
//Intellitrol  08/07/2016
//******************************************************************************
void FT8XX_modify_element_string (uint8_t number, uint8_t type, char * str)
{
  uint8_t cnt = 0, temp = 0;

  //Based on primitive type, same code is executed to append text / calculate
  //offset for display list to be a multiple of 5
  switch (type)
  {
    #if MAX_TEXT_NB > 0
    case FT_PRIM_TEXT:
      while (*str != 0)
      {
        st_Text[number].str[cnt] = *str; //write string to struct
        str++;
        cnt++;
      }
      st_Text[number].str[cnt] = 0x00;     //last byte must be a 0
      cnt++;
      temp = cnt & 0x0F;
      //if string byte # is not a multiple of 4 bytes, append null bytes
      if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
      {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
          st_Text[number].str[cnt] = 0x00;
          cnt++;
          temp = cnt & 0x0F;
        }
      }
      //get string length into struct
      st_Text[number].len = cnt;
      break;
#endif //#if MAX_TEXT_NB > 0

#if MAX_BUTTON_NB > 0
    case FT_PRIM_BUTTON:
      while (*str != 0)
      {
        st_Button[number].str[cnt] = *str;
        str++;
        cnt++;
      }
      st_Button[number].str[cnt] = 0x00;
      cnt++;
      temp = cnt & 0x0F;
      if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
      {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
          st_Button[number].str[cnt] = 0x00;
          cnt++;
          temp = cnt & 0x0F;
        }
      }
      st_Button[number].len = cnt;
      break;
#endif //#if MAX_BUTTON_NB > 0

#if MAX_TOGGLESW_NB > 0
    case FT_PRIM_TOGGLESW:
      while (*str != 0)
      {
        st_Togglesw[number].str[cnt] = *str;
        str++;
        cnt++;
      }
      st_Togglesw[number].str[cnt] = 0x00;
      cnt++;
      temp = cnt & 0x0F;
      if ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
      {
        while ((temp != 0) && (temp != 4) && (temp != 8) && (temp != 12))
        {
          st_Togglesw[number].str[cnt] = 0x00;
          cnt++;
          temp = cnt & 0x0F;
        }
      }
      st_Togglesw[number].len = cnt;
      break;
#endif //#if MAX_TOGGLESW_NB > 0
  }
}