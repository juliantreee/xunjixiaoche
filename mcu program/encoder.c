#include"encoder.h"

//全局变量：
float mech_angel_rad=0.0f;
float speed_radps=0.0f;
int32_t encoder_position=0;
//静态变量
static int32_t last_position=0;
static uint32_t last_tick=0;


//初始化编码器
void init_Encoder(void)
{
    DL_TimerG_enableClock(ENCODER_TIMER);//时钟使能

    DL_TimerG_setOperatingMode(ENCODER_TIMER,DL_TIMERG_OPERATION_MODE_ENCODER);//编码器模式

    DL_TimerG_setLoadValue(ENCODER_TIMER,ENCODER_CPR-1);//到达值时自动复位
    DL_TimerG_setEncoderMode(ENCODER_TIMER,DL_TimerG_ENCODER-MODE_X4);//X4倍频编码器模式
    DL_TimerG_reset(ENCODER_TIMER);//清空计数器
    DL_TimerG_enable(ENCODER_TIMER);//定时器使能
}
  
//获取当前计数值
int32_t Encoder_Position(void){
    return (int32_t)DL_TimerG_getCaptureCount(ENCODER_TIMER);

}

//获取弧度
float get_Encoder_Angle(void){
    int32_t pos=Encoder_Position();
    return ((float)pos/ENCODER_CPR)*TWO_PI;
}

//计算角速度
float calculate_Speed(void)
{
    int32_t current_position=get_Encoder_Position();
    int32_t delta_pulse;
    delta_pulse=current_position-last_position;//计算脉冲变化量
    //溢出回绕处理
    if(delta_pulse>ENCODER_CPR/2){
        delta_pulse-=ENCODER_CPR;
    }else if (delta_pulse<ENCODER_CPR/2){
        delta_pulse+=ENCODER_CPR;
    
    }

    last_position=current_position;//重置位置
    return((float)delta_pulse/ENCODER_CPR)*TWO_PI/SPEED_CALC_PERIOD ;//角速度计算

}
//更新编码器
void update_EncoderData(void){
    encoder_position=get_Encoder_Position();
    mech_angle_rad=get_Encoder_Angle();
    speed_radps+calculate_Speed();
}
