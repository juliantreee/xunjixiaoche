#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include "Motor.h"
#include "encoder.h"
#include "PID.h"
#include "Motor_pid.h"

PID *Lmotor_pid,*Rmotor_pid;
bool lrun = false;
bool rrun = false;//电机启动标识
double LRPM,RRPM;

void Motor_pid_Init(void)
{
    stopnow();
    Encoder_Init();
    Lmotor_pid = pid_create(1.0,1.0,1.0, 90.0, 0.0, 0.005);//Kp,Ki,Kd,最大油门，最小油门，定时器周期
    Rmotor_pid = pid_create(1.0,1.0,1.0, 90.0, 0.0, 0.005);
    lrun = false;
    rrun = false;
}

//电机启动,设置速度也用这个
void Lmotor_run(double rpm)//rpm正向前，负向后
{
    if (rpm > 0)
    {
        MotorL_forward();
        LRPM = rpm;
    }
    else {
        MotorL_backward();
        LRPM = -1*rpm;
    }
    lrun = true;
}

void Rmotor_run(double rpm)//rpm正向前，负向后
{
    if (rpm > 0)
    {
        MotorR_forward();
        RRPM = rpm;
    }
    else {
        MotorR_backward();
        RRPM = -1*rpm;
    }
    rrun = true;
}

//电机停止
void Lmotor_stop(void)
{
    MotorL_stop();
    lrun = false;
}

void Rmotor_stop(void)
{
    MotorR_stop();
    rrun = false;
}

//电机刹车
void Lmotor_brake(void)
{
    MotorL_brake();
    lrun = false;
}

void Rmotor_brake(void)
{
    MotorR_brake();
    rrun = false;
}

//pid_step
void Motor_pid_step(void)
{
    if(lrun == true)
    {
        get_period();
        pid_step(Lmotor_pid, Lmotor_RPM, LRPM);
    }
    if(rrun == true)
    {
        get_period();
        pid_step(Rmotor_pid, Rmotor_RPM, RRPM);
    }
}