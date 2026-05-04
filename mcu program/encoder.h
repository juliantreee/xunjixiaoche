#ifndef ENCODER_H_
#define ENCODER_H_

#include "ti_msp_dl_config.h"
#include <stdbool.h>

extern volatile int LF;
extern volatile bool LD,Ltimeout;
extern volatile uint16_t LFCap,LSCap;

double get_l_speed(void);
void Encoder_Init(void);

void Encoder_QEI_Init(void);          // 初始化 QEI
int32_t Encoder_QEI_GetPosition(void); // 获取当前计数值（位置）
void Encoder_QEI_ResetPosition(void);  // 清零计数值
bool Encoder_QEI_GetDirection(void);   // 获取方向（true = 正转/向上）

#endif