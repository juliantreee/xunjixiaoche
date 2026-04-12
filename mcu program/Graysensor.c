#include "ti_msp_dl_config.h"
#include "delay.h"
#include "Graysensor.h"
#include "Motor.h"
bool gray_value[8] = {0,0,0,0,0,0,0,0};
uint8_t TrackN;
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
void Read_Track_Data(uint8_t* arr)//读取赛道数据,这个arr是一个数组,其实不用也没事
{
    uint8_t current_track = 0;//当前8个传感器的数据
    static uint8_t last_track =0;//上一次8个传感器的数据
    Gray_read_all();
    //将8位转化成整数
    current_track = gray_value[7] +gray_value[6]*2 +gray_value[5]*4 +gray_value[4]*8 +gray_value[3]*16 +gray_value[2]*32 +gray_value[1]*64 +gray_value[0]*128;
    //均值滤波
    TrackN = (uint8_t)((current_track + last_track) / 2);//将当前的赛道数据与上一次的赛道数据求均值
    last_track = current_track;//更新上一次赛道数据
    *arr = TrackN;//将赛道数据存储到传入的数组
}
float Track_Err(void)//这个源码里面是传了一个uint16_t car_state但是没有用到，看他源码应该是pid时车的什么值
{
    float Err;//给速度的值
    switch (TrackN) 
    {
        case 0xe7: Err = 0; break;//中间
        case 0xcf: Err = 3.5; break;//右侧小偏差 减小值使转向平缓
        case 0x9f: Err = 5.0; break;//右侧中等偏差
        case 0x3f: Err = 6.0; break;//右侧大偏差 减大值使转向平缓
        case 0xf3: Err = -3.5; break;//左侧小偏差 减小值使转向平缓
        case 0xf9: Err = -5.0; break;//左侧中等偏差
        case 0xfc: Err = -6.0; break;//左侧大偏差 减大值使转向平缓
        case 0xef: Err = 2.0; break;//右侧轻微偏差
        case 0xdf: Err = 3.0; break;//右侧中小偏差
        case 0xbf: Err = 3.5; break;//右侧中小偏差
        case 0x7f: Err = 7.0; break;//左侧极大偏差 大幅增大
        case 0xf7: Err = -2.0; break;//左侧轻微偏差
        case 0xfb: Err = -3.0; break;//左侧中小偏差
        case 0xfd: Err = -4.5; break;//左侧中小偏差
        case 0xfe: Err = -7.0; break;//左侧极大偏差 大幅增大
        case 0x1f: Err = 8.0; break;//右侧极限偏差 特别大
        case 0xf8: Err = -3.0; break;//左侧中小偏差
        case 0x8f: Err = 9.0; break;//右侧极限偏差 特别大的值
        default: Err = 0; break;//没定义情况
    }
}