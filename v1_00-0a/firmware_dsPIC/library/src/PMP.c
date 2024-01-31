#include "PMP.h"

STRUCT_PMP PMP_struct;

void PMP_init (uint8_t mode)
{   
    PMP_pins_init();        // Configure ALL PMP pins
    PMP_set_bus_output();
}

void PMP_write_single (uint8_t mode, uint16_t adr, uint16_t data)
{
    PMP_set_bus_output();
    switch (mode)
    {
        case PMP_MODE_SRAM:
            // Place address on bus
            PMP_WR_STROBE = 1;      // PMENB, set to 1, #OE inactive
            LATJ = (LATJ & 0xC000) | (adr & 0x3FFF); 
            
            // CS controlled writes to the SRAM
            // Place data(7..0) on data bus first
            PMP_RD_STROBE = 1;      // Direction of latches : dsPIC33E -> memory
            
            PMP_PMBE_STROBE = 0;    // Low byte output enable to memory 
            PMP_CS2_STROBE = 0;     // CS2 = 0   

            LATH = ((LATH & 0xFF00) | (data & 0x00FF));
            PMP_CS2_STROBE = 1;     // CS2 = 1
            
            // Place data(15..8) on data bus 
            PMP_PMBE_STROBE = 1;    // High byte output enable to memory  
            PMP_CS2_STROBE = 0;     // CS2 = 0  

            LATH = ((LATH & 0xFF00) | ((data & 0xFF00)>>8));
            PMP_CS2_STROBE = 1;     // CS2 = 1            
            break;
            
        case PMP_MODE_TFT:
            PMP_CS1_STROBE = 0;    // CS1 = 0
            PMP_RD_STROBE = 1;    // RD = 1;
            PMP_WR_STROBE = 0;    // WR = 0;

            LATH = ((LATH & 0xFF00) | (data & 0x00FF));

            PMP_WR_STROBE = 1;    // WR = 1;
            PMP_CS1_STROBE = 1;    // CS1 = 1            
            break;
            
        default:
            break;
    }
}

void PMP_write_multiple (uint8_t mode, uint16_t adr, uint16_t *ptr, uint16_t length)
{
    unsigned int i = 0;
    PMP_set_bus_output();
    switch (mode)
    {
        case PMP_MODE_SRAM:            
            PMP_WR_STROBE = 1;      // PMENB, set to 1, #OE inactive
            PMP_RD_STROBE = 1;      // Direction of latches : dsPIC33E -> memory            
            for (; i < length; i++)
            {               
                LATJ = (LATJ & 0xC000) | ((adr + i) & 0x3FFF);    // Place address on bus 
                // CS controlled writes to the SRAM                 
                // Place data(7..0) on data bus first            
                PMP_PMBE_STROBE = 0;                        // Low byte output latch enable
                PMP_CS2_STROBE = 0;                         // CS2 = 0   
                LATH = ((LATH & 0xFF00) | (*ptr & 0x00FF));
                PMP_CS2_STROBE = 1;                         // CS2 = 1            
                // Place data(15..8) on data bus 
                PMP_PMBE_STROBE = 1;                        // High byte output latch enable
                PMP_CS2_STROBE = 0;                         // CS2 = 0  
                LATH = ((LATH & 0xFF00) | ((*ptr++ & 0xFF00)>>8));
                PMP_CS2_STROBE = 1;                         // CS2 = 1                 
            }                       
            break;
            
        case PMP_MODE_TFT:
            PMP_CS1_STROBE = 0;                             // CS1 = 0
            PMP_RD_STROBE = 1;                              // RD = 1;
            for (; i < length; i++)
            {                                
                PMP_WR_STROBE = 0;                          // WR = 0;
                LATH = ((LATH & 0xFF00) | (*ptr++ & 0x00FF));
                PMP_WR_STROBE = 1;                          // WR = 1;                
            }
            PMP_CS1_STROBE = 1;                             // CS1 = 1  
            break;
            
        default:
            break;
    }    
}

uint16_t PMP_read_single (uint8_t mode, uint16_t adr)
{
    uint16_t result = 0, result_l = 0;
    PMP_set_bus_input();      
    switch (mode)
    {
        case PMP_MODE_SRAM:
            // Place address on bus            
            LATJ = (LATJ & 0xC000) | (adr & 0x3FFF); 
          
            // Place data(7..0) on data bus first
            PMP_RD_STROBE = 0;          // Direction of latches : dsPIC33E <- memory             
            PMP_CS2_STROBE = 0;         // CS2 = 0         
            PMP_PMBE_STROBE = 0;        // Low byte output enable from memory   
            PMP_WR_STROBE = 0;          // SRAM #OE = 0 
            result_l = PORTH & 0x00FF;  // Output data byte 7..0 on bus
            PMP_WR_STROBE = 1;          // SRAM #OE = 1         
            PMP_CS2_STROBE = 1;         // CS2 = 1
            
            // Place data(15..8) on data bus 
            PMP_CS2_STROBE = 0;         // CS2 = 0
            PMP_PMBE_STROBE = 1;        // High byte output enable to memory               
            PMP_WR_STROBE = 0;          // SRAM #OE = 0
            result = (((PORTH & 0x00FF) << 8) | result_l);    
            PMP_WR_STROBE = 1;      // SRAM #OE = 1                        
            PMP_CS2_STROBE = 1;     // CS2 = 1      
            break;
            
        case PMP_MODE_TFT:
            PMP_CS1_STROBE = 0;    // CS1 = 0
            PMP_WR_STROBE = 1;    // WR = 1;       

            PMP_RD_STROBE = 0;    // RD = 0;
            PMP_RD_STROBE = 1;    // RD = 1;
            PMP_RD_STROBE = 0;    // RD = 0;
            result = PORTH & 0x00FF; 
            PMP_RD_STROBE = 1;    // RD = 1;     

            PMP_CS1_STROBE = 1;    // CS1 = 1            
            break;
            
        default:
            result = 0;
            break;
    }
    
    return result;
}

void PMP_pins_init (void)
{    
    // PMP adress bus pins setup to output
    TRISJ &= 0xC000;
    TRISK &= 0x07FE;
    LATK |= 0xF801;
}

void PMP_set_bus_output (void)
{
    TRISH = TRISH & 0xFF00; 
}

void PMP_set_bus_input (void)
{
    TRISH = TRISH | 0x00FF;
}