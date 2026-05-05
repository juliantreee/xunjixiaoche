#ifndef MOTOR_PID_H_
#define MOTOR_PID_H_

#include "PID.h"

extern PID *Lmotor_pid,*Rmotor_pid;
extern double LRPM,RRPM; //左右电机转速
extern bool lrun,rrun;
void Motor_pid_Init(void);
void Motor_pid_step(void);
void Lmotor_run(double rpm);
void Rmotor_run(double rpm);
void Lmotor_stop(void);
void Rmotor_stop(void);
void Lmotor_brake(void);
void Rmotor_brake(void);
#endif