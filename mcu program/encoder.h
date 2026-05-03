#ifndef ENCODER_H_
#define ENCODER_H_

#include "ti_msp_dl_config.h"
#define PPR 10000  //线数
#define Timerf 32000000.0  //计时器频率

extern bool LF,LD,RF,RD,Ltimeout,Rtimeout;
extern uint16_t LFCap,LSCap,RFCap,LFCap;
extern uint16_t Lperiod,Rperiod;

double get_l_speed(void);
double get_r_speed(void);
void Encoder_Init(void);

#endif