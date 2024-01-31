//****************************************************************************//
// File      :  DMA.c
//
// Functions :  
//
// Includes  :  dma.h
//
// Purpose   :  Driver for the dsPIC33CK DMA engine
//    
// Intellitrol           MPLab X v6.05            XC16 v2.10          01/01/2024   
// Jean-Francois Bilodeau, Ing.
// jeanfrancois.bilodeau@hotmail.fr
// www.github.com/lecejeff
//****************************************************************************//
#include "DMA.h"
STRUCT_DMA DMA_struct[DMA_QTY];

void DMA_init (uint8_t channel)
{ 
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].prev_txfer_state = DMA_TXFER_DONE;
        DMA_struct[channel].txfer_state = DMA_TXFER_DONE; 
        DMACONbits.DMAEN = 1;
        switch (channel)
        {     
            case DMA_CH0:              
                IEC0bits.DMA0IE = 0;                    // Disable DMA0 interrupt
                IFS0bits.DMA0IF = 0;                    // Lower DMA0 interrupt flag           
                break;

            case DMA_CH1:

                IEC0bits.DMA1IE = 0;
                IFS0bits.DMA1IF = 0;
                break;  

            case DMA_CH2:
                IEC1bits.DMA2IE = 0;
                IFS1bits.DMA2IF = 0;
                break; 

            case DMA_CH3:
                IEC1bits.DMA3IE = 0;
                IFS1bits.DMA3IF = 0;
                break; 

            case DMA_CH4:
                IEC1bits.DMA4IE = 0;
                IFS1bits.DMA4IF = 0;
                break;

            case DMA_CH5:
                IEC2bits.DMA5IE = 0;
                IFS2bits.DMA5IF = 0;
                break;   

            case DMA_CH6:
                IEC9bits.DMA6IE = 0;
                IFS9bits.DMA6IF = 0;
                break;      

            case DMA_CH7:
                IEC9bits.DMA7IE = 0;
                IFS9bits.DMA7IF = 0;
                break;   
                
            default:
                break;
        }
    }
}

void DMA_disable (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].prev_txfer_state = DMA_TXFER_DONE;
        DMA_struct[channel].txfer_state = DMA_TXFER_DONE; 
        switch (channel)
        {
            case DMA_CH0:
                DMACH0bits.CHEN = 0;
                IEC0bits.DMA0IE = 0;                    // Disable DMA0 interrupt
                IFS0bits.DMA0IF = 0;                    // Lower DMA0 interrupt flag            
                break;

            case DMA_CH1:
                DMACH1bits.CHEN = 0;
                IEC0bits.DMA1IE = 0;                
                IFS0bits.DMA1IF = 0;                
                break;

            case DMA_CH2:
                DMACH2bits.CHEN = 0;
                IEC1bits.DMA2IE = 0;                
                IFS1bits.DMA2IF = 0;             
                break;

            case DMA_CH3:
                DMACH3bits.CHEN = 0;
                IEC1bits.DMA3IE = 0;
                IFS1bits.DMA3IF = 0;            
                break;

            case DMA_CH4:
                DMACH4bits.CHEN = 0;
                IEC1bits.DMA4IE = 0;
                IFS1bits.DMA4IF = 0;            
                break;

            case DMA_CH5:
                DMACH5bits.CHEN = 0;
                IEC2bits.DMA5IE = 0;
                IFS2bits.DMA5IF = 0;
                break;

            case DMA_CH6:
                DMACH6bits.CHEN = 0;
                IEC9bits.DMA6IE = 0;
                IFS9bits.DMA6IF = 0;
                break;

            case DMA_CH7:
                DMACH7bits.CHEN = 0;
                IEC9bits.DMA7IE = 0;
                IFS9bits.DMA7IF = 0;
                break;  

            default:
                break;
        }
    }
}

void DMA_enable (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        DMA_struct[channel].prev_txfer_state = DMA_TXFER_DONE;
        DMA_struct[channel].txfer_state = DMA_TXFER_IN_PROGRESS;        
        DMACONbits.PRSSEL = 0;      // Fixed priority scheme
        switch (channel)
        {
            case DMA_CH0:
                DMACH0bits.CHEN = 1;
                IFS0bits.DMA0IF = 0;                    // Lower DMA interrupt flag 
                IEC0bits.DMA0IE = 1;                    // Enable DMA interrupt                       
                break;

            case DMA_CH1:
                DMACH1bits.CHEN = 1;
                IEC0bits.DMA1IE = 1;                
                IFS0bits.DMA1IF = 0;                
                break;

            case DMA_CH2:
                DMACH2bits.CHEN = 1;
                IEC1bits.DMA2IE = 1;                
                IFS1bits.DMA2IF = 0;             
                break;

            case DMA_CH3:
                DMACH3bits.CHEN = 1;
                IEC1bits.DMA3IE = 1;
                IFS1bits.DMA3IF = 0;            
                break;

            case DMA_CH4:
                DMACH4bits.CHEN = 1;
                IEC1bits.DMA4IE = 1;
                IFS1bits.DMA4IF = 0;            
                break;

            case DMA_CH5:
                DMACH5bits.CHEN = 1;
                IEC2bits.DMA5IE = 1;
                IFS2bits.DMA5IF = 0;
                break;

            case DMA_CH6:
                DMACH6bits.CHEN = 1;
                IEC9bits.DMA6IE = 1;
                IFS9bits.DMA6IF = 0;
                break;

            case DMA_CH7:
                DMACH7bits.CHEN = 1;
                IEC9bits.DMA7IE = 1;
                IFS9bits.DMA7IF = 0;
                break; 

            default:
                break;
        }
    }
}

void DMA_set_channel_trigger (uint8_t channel, uint8_t trigger)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {
        switch (channel)
        {
            case DMA_CH0:
                DMAINT0bits.CHSEL = trigger;                   
                break;

            case DMA_CH1:
                DMAINT1bits.CHSEL = trigger;                 
                break;

            case DMA_CH2:
                DMAINT2bits.CHSEL = trigger;             
                break;

            case DMA_CH3:
                DMAINT3bits.CHSEL = trigger;            
                break;

            case DMA_CH4:
                DMAINT4bits.CHSEL = trigger;             
                break;

            case DMA_CH5:
                DMAINT5bits.CHSEL = trigger; 
                break;

            case DMA_CH6:
                DMAINT6bits.CHSEL = trigger; 
                break;

            case DMA_CH7:
                DMAINT7bits.CHSEL = trigger; 
                break; 

            default:
                break;
        }
    }
}

uint16_t DMA_get_buffer_address (uint8_t channel, uint8_t mode)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {          
        switch (channel)
        {
            case DMA_CH0:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC0; 
                }
                else
                {
                    return DMADST0;  
                }
                break;

            case DMA_CH1:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC1; 
                }
                else
                {
                    return DMADST1;  
                }             
                break;

            case DMA_CH2:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC2; 
                }
                else
                {
                    return DMADST2;  
                }              
                break;

            case DMA_CH3:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC3; 
                }
                else
                {
                    return DMADST3;  
                }            
                break;

            case DMA_CH4:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC4; 
                }
                else
                {
                    return DMADST4;  
                }             
                break;

            case DMA_CH5:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC5; 
                }
                else
                {
                    return DMADST5;  
                }  
                break;

            case DMA_CH6:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC6; 
                }
                else
                {
                    return DMADST6;  
                } 
                break;

            case DMA_CH7:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    return DMASRC7; 
                }
                else
                {
                    return DMADST7;  
                }  
                break;  

            default:
                return 0;
                break;
        }
    }
    else
        return 0;
}

void DMA_set_channel_req (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMACH0bits.CHREQ = 1;                    
                break;

            case DMA_CH1:
                DMACH1bits.CHREQ = 1;                   
                break;

            case DMA_CH2:
                DMACH2bits.CHREQ = 1;               
                break;

            case DMA_CH3:
                DMACH3bits.CHREQ = 1;              
                break;

            case DMA_CH4:
                DMACH4bits.CHREQ = 1;               
                break;

            case DMA_CH5:
                DMACH5bits.CHREQ = 1;     
                break;

            case DMA_CH6:
                DMACH6bits.CHREQ = 1;    
                break;

            case DMA_CH7:
                DMACH7bits.CHREQ = 1;     
                break;    

            default:
                break;
        }    
    }
}

void DMA_set_control_register (uint8_t channel, uint16_t DMACHx)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                DMACH0 = 0xFFFF & DMACHx;                    
                break;

            case DMA_CH1:
                DMACH1 = 0xFFFF & DMACHx;                 
                break;

            case DMA_CH2:
                DMACH2 = 0xFFFF & DMACHx;             
                break;

            case DMA_CH3:
                DMACH3 = 0xFFFF & DMACHx;            
                break;

            case DMA_CH4:
                DMACH4 = 0xFFFF & DMACHx;             
                break;

            case DMA_CH5:
                DMACH5 = 0xFFFF & DMACHx;   
                break;

            case DMA_CH6:
                DMACH0 = 0xFFFF & DMACHx;   
                break;

            case DMA_CH7:
                DMACH7 = 0xFFFF & DMACHx;   
                break;    

            default:
                break;
        }    
    }
}

void DMA_set_peripheral_address (uint8_t channel, uint8_t mode, uint16_t addr)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC0 = addr; 
                }
                else
                {
                    DMADST0 = addr;
                }
                break;

            case DMA_CH1:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC1 = addr; 
                }
                else
                {
                    DMADST1 = addr;
                }                
                break;

            case DMA_CH2:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC2 = addr; 
                }
                else
                {
                    DMADST2 = addr;
                }        
                break;

            case DMA_CH3:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC3 = addr; 
                }
                else
                {
                    DMADST3 = addr;
                }         
                break;

            case DMA_CH4:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC4 = addr; 
                }
                else
                {
                    DMADST4 = addr;
                }            
                break;

            case DMA_CH5:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC5 = addr; 
                }
                else
                {
                    DMADST5 = addr;
                }   
                break;

            case DMA_CH6:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC6 = addr; 
                }
                else
                {
                    DMADST6 = addr;
                }  
                break;

            case DMA_CH7:
                if (mode == DMA_ARRAY_SOURCE)
                {
                    DMASRC7 = addr; 
                }
                else
                {
                    DMADST7 = addr;
                } 
                break;    

            default:
                break;
        }   
    }
}

void DMA_set_txfer_length(uint8_t channel, uint16_t address, uint16_t length)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        // Saturate DMA tx length
        if (length > DMA_MAX_TX_LENGTH)
        {
            length = DMA_MAX_TX_LENGTH;
        }

        switch (channel)
        {
            case DMA_CH0:
                DMACNT0 = length; 
                break;

            case DMA_CH1:
                DMACNT1 = length;                 
                break;

            case DMA_CH2:
                DMACNT2 = length;             
                break;

            case DMA_CH3:
                DMACNT3 = length;            
                break;

            case DMA_CH4:
                DMACNT4 = length;             
                break;

            case DMA_CH5:
                DMACNT5 = length;   
                break;

            case DMA_CH6:
                DMACNT6 = length;   
                break;

            case DMA_CH7:
                DMACNT7 = length;   
                break;

            default:
                break;
        }
        
        DMAL = 0x500;
        DMAH = address + length;
    }
}

uint8_t DMA_get_txfer_state (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {    
        if (DMA_struct[channel].txfer_state == DMA_TXFER_DONE)
        {
            DMA_struct[channel].prev_txfer_state = DMA_struct[channel].txfer_state;
            DMA_struct[channel].txfer_state = DMA_TXFER_IDLE;
            return DMA_TXFER_DONE;
        }
        else if (DMA_struct[channel].txfer_state == DMA_TXFER_IN_PROGRESS)
        {
            return DMA_TXFER_IN_PROGRESS;
        }
        else
            return DMA_TXFER_IDLE;
    }
    else
        return DMA_TXFER_IDLE;
}

uint8_t DMA_get_channel_req (uint8_t channel)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {     
        switch (channel)
        {
            case DMA_CH0:
                return DMACH0bits.CHREQ;                    
                break;

            case DMA_CH1:
                return DMACH1bits.CHREQ;                        
                break;

            case DMA_CH2:
                return DMACH2bits.CHREQ;                     
                break;

            case DMA_CH3:
                return DMACH3bits.CHREQ;                    
                break;

            case DMA_CH4:
                return DMACH4bits.CHREQ;                   
                break;

            case DMA_CH5:
                return DMACH5bits.CHREQ;         
                break;

            case DMA_CH6:
                return DMACH6bits.CHREQ;        
                break;

            case DMA_CH7:
                return DMACH7bits.CHREQ;          
                break;    

            default:
                return 0;
                break;
        }    
    }
    else
        return 0;
}

void DMA_set_txfer_state (uint8_t channel, uint8_t state)
{
    if ((channel >= 0 ) && (channel < DMA_QTY))
    {    
        DMA_struct[channel].txfer_state = state;
    }
}

void __attribute__((__interrupt__, no_auto_psv))_DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;
    DMA_struct[DMA_CH0].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;
    DMA_struct[DMA_CH1].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA2Interrupt(void)
{
    IFS1bits.DMA2IF = 0;
    DMA_struct[DMA_CH2].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA3Interrupt(void)
{
    IFS1bits.DMA3IF = 0;
    DMA_struct[DMA_CH3].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA4Interrupt(void)
{
    IFS1bits.DMA4IF = 0;
    DMA_struct[DMA_CH4].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA5Interrupt(void)
{
    IFS2bits.DMA5IF = 0;
    DMA_struct[DMA_CH5].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA6Interrupt(void)
{
    IFS9bits.DMA6IF = 0;
    DMA_struct[DMA_CH6].txfer_state = DMA_TXFER_DONE;
}

void __attribute__((__interrupt__, no_auto_psv))_DMA7Interrupt(void)
{
    IFS9bits.DMA7IF = 0;
    DMA_struct[DMA_CH7].txfer_state = DMA_TXFER_DONE;
}
