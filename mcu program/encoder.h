#ifndef ENCODER_H_
#define ENCODER_H_
#include "ti_msp_dl_config.h"
#define ENCODER_LINES    500   //编码器线数
#define ENCODER_CPR     (ENCODER_LINES*5)//2000 四倍频线数
#define SPEED_CALC_PERIOD  3.125e-8   //速度计算周期
#define TWO_PI       6.2831853

#define ENCODER_TIMER   TIMG0
#define ENCODER_TIMER_INST 0

extern float mech_angle_rad;//弧度
extern float speed_radps;//角速度
extern int32_t encoder_position;//当前脉冲计数值

void init_Encoder(void);//初始化编码器
int32_t Encoder_Position(void);//获取当前位置
float get_Encoder_Angle(void);//计算弧度
float calculate_Speed(void);//计算角速度
void update_EncoderData(void);//重置编码器


#endif