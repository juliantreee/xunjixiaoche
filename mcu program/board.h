
#ifndef __BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"
#include <stdio.h>

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#ifndef u64
#define u64 uint64_t
#endif

void board_init(void);
void uart0_send_char(char ch);
void uart0_send_string(char* str);
void uart0_send_SendByte(uint8_t* data, uint32_t len);
void uart1_send_char(char ch);
void uart1_send_string(char* str);
void uart1_send_SendByte(uint8_t* data, uint32_t len);
int fputc(int ch, FILE *f);
/* 延时函数（delay_us/delay_ms 见 delay.h） */
void delay_1us(int __us);
void delay_1ms(int __ms);


/*============================================================================
 * 数据结构定义 - 六轴传感器数据
 *===========================================================================*/

/**
 * @brief 角度结构体 (单位: 度)
 */
struct SAngle
{
    float Roll;   // 横滚角  (-180° ~ +180°)
    float Pitch;  // 俯仰角  (-180° ~ +180°)
    float Yaw;    // 航向角  (-180° ~ +180°)
};

/**
 * @brief 角速度结构体 (单位: °/s)
 */
struct SGyro
{
    float wx;     // X轴角速度  (±2000°/s)
    float wy;     // Y轴角速度  (±2000°/s)
    float wz;     // Z轴角速度  (±2000°/s)
    
    // 原始数据(用于调试)
    short rawWx;
    short rawWy;
    short rawWz;
};

/**
 * @brief 加速度结构体 (单位: m/s²)
 */
struct SAccel
{
    float ax;     // X轴加速度  (±16g)
    float ay;     // Y轴加速度  (±16g)
    float az;     // Z轴加速度  (±16g)
    
    // 原始数据(用于调试)
    short rawAx;
    short rawAy;
    short rawAz;
};

/**
 * @brief 四元数结构体 (归一化单位)
 */
struct SQuat
{
    float q0;     // 四元数 q0
    float q1;     // 四元数 q1
    float q2;     // 四元数 q2
    float q3;     // 四元数 q3
};

/*============================================================================
 * 全局变量声明
 *===========================================================================*/

extern struct SAngle  stcAngle;   // 角度数据
extern struct SGyro   stcGyro;    // 角速度数据
extern struct SAccel  stcAccel;   // 加速度数据
extern struct SQuat   stcQuat;    // 四元数数据

float GyroX(void);
float GyroY(void);
float GyroZ(void);

float Yaw(void);
float Roll(void);
float Pitch(void);

void sendCaliYawCommand(void);
void performCaliBias(void);
	
#endif
