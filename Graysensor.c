#include "ti_msp_dl_config.h"
#include "delay.h"
#include "Graysensor.h"

bool gray_value[8] = {0,0,0,0,0,0,0,0};

void select_channel(uint8_t channel)
{
    if((channel >> 0) & 0x01)
    {
        DL_GPIO_setPins(Gray_PORT,Gray_AD0_PIN);
    } 
    else 
    {
        DL_GPIO_clearPins(Gray_PORT,Gray_AD0_PIN);
    }
    if((channel >> 1) & 0x01) 
    {
        DL_GPIO_setPins(Gray_PORT,Gray_AD1_PIN);
    }
    else 
    {
        DL_GPIO_clearPins(Gray_PORT,Gray_AD1_PIN);
    }
    if((channel >> 2) & 0x01) 
    {
        DL_GPIO_setPins(Gray_PORT,Gray_AD2_PIN);
    }
    else 
    {
        DL_GPIO_clearPins(Gray_PORT,Gray_AD2_PIN);
    }
}

bool Gray_read()
{
    if(DL_GPIO_readPins(Grayread_PORT, Grayread_read_PIN) > 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

void Gray_read_all()
{
    for(uint8_t i = 0;i < 8;i++)
    {
        select_channel(i);
        delay_us(50);
        gray_value[i] = Gray_read();
    }
}
