#ifndef ENCODER_H_
#define ENCODER_H_

#include "ti_msp_dl_config.h"
#include <stdbool.h>

extern volatile int LF;
extern volatile bool LD,Ltimeout;
extern volatile uint16_t LFCap,LSCap;
extern volatile uint16_t RFCap,RSCap;

double get_speed(void);
double get_l_speed(void);
double get_r_speed(void);
void Encoder_Init(void);

#endif