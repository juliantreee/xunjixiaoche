#ifndef MOTOR_PID_H_
#define MOTOR_PID_H_

typedef struct PID PID;

PID* pid_create(double kp, double ki, double kd); //PID对象创建函数
double pid_step(PID *in,double value);  //单次pid函数
void pid_set_target(PID *in, double target);

#endif