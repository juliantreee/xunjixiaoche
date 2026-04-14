#ifndef MOTOR_PID_H_
#define MOTOR_PID_H_

extern PID *Lmotor_pid,*Rmotor_pid;
extern double LRPM,RRPM; //左右电机转速
extern bool lrun,rrun;
void motor_pid_Init(void);
void motor_pid_step(void);
void Lmotor_stop(void);
void Rmotor_stop(void);
void Lmotor_brake(void);
void Rmotor_brake(void);
#endif