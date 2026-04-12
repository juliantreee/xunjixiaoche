#include "oled_menu.h"
#include "oledfont.h"       // 包含点阵字库
#include "clock.h"
#include "delay.h"
#define I2C_TIMEOUT_MS  (10)

uint8_t OLED_GRAM[128][8] = {0};  // 初始化为全0

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127



static int mspm0_i2c_disable(void)
{
    DL_I2C_reset(I2C_OLED_INST);
    DL_GPIO_initDigitalOutput(GPIO_I2C_OLED_IOMUX_SCL);
    DL_GPIO_initDigitalInputFeatures(GPIO_I2C_OLED_IOMUX_SDA,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
    DL_GPIO_enableOutput(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
    return 0;
}

static int mspm0_i2c_enable(void)
{
    DL_I2C_reset(I2C_OLED_INST);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_OLED_IOMUX_SDA,
        GPIO_I2C_OLED_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_OLED_IOMUX_SCL,
        GPIO_I2C_OLED_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_OLED_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_OLED_IOMUX_SCL);
    DL_I2C_enablePower(I2C_OLED_INST);
    SYSCFG_DL_I2C_OLED_init();
    return 0;
}

void oled_i2c_sda_unlock(void)
{
    uint8_t cycleCnt = 0;
    mspm0_i2c_disable();
    do
    {
        DL_GPIO_clearPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
        mspm0_delay_ms(1);
        DL_GPIO_setPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
        mspm0_delay_ms(1);

        if(DL_GPIO_readPins(GPIO_I2C_OLED_SDA_PORT, GPIO_I2C_OLED_SDA_PIN))
            break;
    }while(++cycleCnt < 100);
    mspm0_i2c_enable();
}


void OLED_Refresh(uint8_t x, uint8_t page_start, uint8_t width, uint8_t page_count)
{
    uint8_t i, j;
    
    for (i = page_start; i < page_start + page_count; i++)
    {
        if (i >= 8) break;
        
        OLED_WR_Byte(0xB0 + i, OLED_CMD);      // 设置页地址
        OLED_WR_Byte((x & 0xF0) >> 4 | 0x10, OLED_CMD); // 设置列高地址
        OLED_WR_Byte(x & 0x0F, OLED_CMD);      // 设置列低地址
        
        for (j = x; j < x + width; j++)
        {
            if (j < 128)
            {
                OLED_WR_Byte(OLED_GRAM[j][i], OLED_DATA);
            }
        }
    }
}

// 示例：基于显存副本的局部反显函数
// 假设 x, y 为区域左上角的像素坐标，width, height 为区域尺寸
void OLED_ColorTurn(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    // 1. 参数安全检查
    if (x >= 128 || y >= 64) return;
    if ((x + width) > 128) width = 128 - x;
    if ((y + height) > 64) height = 64 - y;

    // 2. 计算影响的页（Page）
    uint8_t page_start = y / 8;
    uint8_t page_end = (y + height - 1) / 8;

    for (uint8_t page = page_start; page <= page_end; page++) {
        // 3. 计算当前页内需要修改的像素行（位掩码）
        uint8_t bit_start = (page == page_start) ? (y % 8) : 0;
        uint8_t bit_end = (page == page_end) ? ((y + height - 1) % 8) : 7;
        uint8_t bit_mask = ((0xFF << bit_start) & (0xFF >> (7 - bit_end)));

        // 4. 遍历当前页的每一列，对目标区域内的字节进行按位取反
        for (uint8_t col = x; col < (x + width); col++) {
            // 只反显在指定区域内的部分
            OLED_GRAM[col][page] ^= bit_mask; // 使用异或操作进行按位取反
        }
    }

    // 5. 【关键】将修改过的这一小块显存区域更新到OLED硬件
    // 你需要一个能局部刷新的函数，例如 OLED_Refresh_Area(x, page_start, width, (page_end - page_start + 1))
    // 如果只有全屏刷新函数，调用 OLED_Refresh() 也可行，但可能效率较低
    OLED_Refresh(x, page_start, width, (page_end - page_start + 1)); 
}


//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
if(i==0)
    {
        OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
        OLED_WR_Byte(0xA1,OLED_CMD);
    }
    if(i==1)
    {
        OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
        OLED_WR_Byte(0xA0,OLED_CMD);
    }
}

//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
    unsigned char ptr[2];
    unsigned long start, cur;

    if(mode)
    {
        ptr[0] = 0x40;
        ptr[1] = dat;
    }
    else
    {
        ptr[0] = 0x00;
        ptr[1] = dat;
    }

    mspm0_get_clock_ms(&start);

    DL_I2C_fillControllerTXFIFO(I2C_OLED_INST, ptr, 2);
    DL_I2C_clearInterruptStatus(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);
    while (!(DL_I2C_getControllerStatus(I2C_OLED_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_OLED_INST, 0x3C, DL_I2C_CONTROLLER_DIRECTION_TX, 2);

    while (!DL_I2C_getRawInterruptStatus(I2C_OLED_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE))
    {
        mspm0_get_clock_ms(&cur);
        if(cur >= (start + I2C_TIMEOUT_MS))
        {
            oled_i2c_sda_unlock();
            break;
        }
    }
}

//坐标设置
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD);
}

//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
	 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
    uint8_t i,n;		    
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
        for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
    } //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63				 
//sizey:选择字体 6x8  8x16
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey)
{      	
    uint8_t c=0,sizex=sizey/2;
    uint16_t i=0,size1;
    if(sizey==8)size1=6;
    else size1=(sizey/8+((sizey%8)?1:0))*(sizey/2);
    c=chr-' ';//得到偏移后的值
    OLED_Set_Pos(x,y);
    for(i=0;i<size1;i++)
    {
        if(i%sizex==0&&sizey!=8) OLED_Set_Pos(x,y++);
        if(sizey==8) OLED_WR_Byte(asc2_0806[c][i],OLED_DATA);//6X8字号
        else if(sizey==16) OLED_WR_Byte(asc2_1608[c][i],OLED_DATA);//8x16字号
        //		else if(sizey==xx) OLED_WR_Byte(asc2_xxxx[c][i],OLED_DATA);//用户添加字号
        else return;
    }
}

//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;	 
    while(n--)result*=m;    
    return result;
}

//显示数字
//x,y :起点坐标
//num:要显示的数字
//len :数字的位数
//sizey:字体大小		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t sizey)
{         	
    uint8_t t,temp,m=0;
    uint8_t enshow=0;
    if(sizey==8)m=2;
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(sizey/2+m)*t,y,' ',sizey);
                continue;
            }else enshow=1;
        }
        OLED_ShowChar(x+(sizey/2+m)*t,y,temp+'0',sizey);
    }
}

//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t sizey)
{
    uint8_t j=0;
    while (chr[j]!='\0')
    {		
        OLED_ShowChar(x,y,chr[j++],sizey);
        if(sizey==8)x+=6;
        else x+=sizey/2;
    }
}

//显示汉字
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey)
{
    uint16_t i,size1=(sizey/8+((sizey%8)?1:0))*sizey;
    for(i=0;i<size1;i++)
    {
        if(i%sizey==0) OLED_Set_Pos(x,y++);
        if(sizey==16) OLED_WR_Byte(Hzk[no][i],OLED_DATA);//16x16字号
        //		else if(sizey==xx) OLED_WR_Byte(xxx[c][i],OLED_DATA);//用户添加字号
        else return;
    }				
}

/**
 * @brief 通过索引号显示BMP图片
 * @param x, y: 起始坐标
 * @param sizex, sizey: 单张图片的像素尺寸（例如128x64）
 * @param no: 图片的索引号（从0开始）
 * @retval 无
 */
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t no) {
    uint16_t j = 0;
    uint8_t i, m;
    
    // 计算单张图片占用的字节数
    // 注意：这里假设图片高度是8的倍数，或者按页存储。如果不是，计算方式需调整。
    uint16_t single_image_size = (sizex * (sizey / 8)); 
    
    // 计算第 no 张图片的起始地址
    // 假设你的图片数组叫 `BMP_Data[]`
    uint8_t *image_start_addr = &BMP[no * single_image_size];
    
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0); // 转换为页数
    
    for(i = 0; i < sizey; i++) {
        OLED_Set_Pos(x, i + y);
        for(m = 0; m < sizex; m++) {      
            OLED_WR_Byte(image_start_addr[j++], OLED_DATA);
        }
    }
}

//初始化SSD1306					    
void OLED_Init(void)
{
    if(DL_I2C_getSDAStatus(I2C_OLED_INST) == DL_I2C_CONTROLLER_SDA_LOW)
        oled_i2c_sda_unlock();

    delay_ms(200);

    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_Clear();
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
}  
