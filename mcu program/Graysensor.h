#ifndef __GRAYSENSOR_H
#define __GRAYSENSOR_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

extern bool gray_value[8];
extern uint8_t TrackN;

void select_channel(uint8_t channel);
bool Gray_read(void);
void Gray_read_all(void);
void Read_Track_Data(uint8_t* arr);
float Track_Err(void);
double getgraylocation(void);

// 巡线PID相关
void Line_PID_Init(double kp, double ki, double kd);
void Set_Base_Speed(double speed);
void Line_Tracking_Step(void);

#endif