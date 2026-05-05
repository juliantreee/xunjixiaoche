#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include "Motor.h"
#include "encoder.h"
#include "PID.h"
#include "Motor_pid.h"
#include <stdio.h>
#include <string.h>
#include "uart.h"

PID *Lmotor_pid,*Rmotor_pid;
bool lrun = false;
bool rrun = false;//电机启动标识
double LRPM,RRPM;
double LCtl,RCtl;

void Motor_pid_Init(void)
{
    stopnow();
    Encoder_Init();
    Lmotor_pid = pid_create(0.5,0.3,0.0, 95.0, -95.0, 0.005);//Kp,Ki,Kd,最大油门，最小油门，定时器周期
    Rmotor_pid = pid_create(0.6,2.0,0.05, 95.0, -95.0, 0.005);
    lrun = false;
    rrun = false;
    LCtl = 0;
    RCtl = 0;
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
        LRPM = rpm;
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
        RRPM = rpm;
    }
    rrun = true;
}

//电机速度控制

void Lmotor_speed(double rrpm)
{
    if(rrpm > 0)
    {
        MotorL_forward();
        MotorL_speed(rrpm);
    }
    else 
    {
        MotorL_backward();
        MotorL_speed(-1*rrpm);
    }
}

void Rmotor_speed(double rrpm)
{
    if(rrpm > 0)
    {
        MotorR_forward();
        MotorR_speed(rrpm);
    }
    else 
    {
        MotorR_backward();
        MotorR_speed(-1*rrpm);
    }
}

//电机停止
void Lmotor_stop(void)
{
    MotorL_stop();
    lrun = false;
    pid_clear(Lmotor_pid);
}

void Rmotor_stop(void)
{
    MotorR_stop();
    rrun = false;
    pid_clear(Rmotor_pid);
}

//电机刹车
void Lmotor_brake(void)
{
    MotorL_brake();
    lrun = false;
    pid_clear(Lmotor_pid);
}

void Rmotor_brake(void)
{
    MotorR_brake();
    rrun = false;
    pid_clear(Rmotor_pid);
}

//pid_step
void Motor_pid_step(void)
{
    if(lrun == true)
    {
        if(LRPM > 0)
        {
            Lmotor_speed(pid_step(Lmotor_pid, get_l_speed(), LRPM));
            printf("%.2f \r\n",get_l_speed());
        }
        else
        {
            Lmotor_speed(pid_step(Lmotor_pid, -1*get_l_speed(), LRPM));
            printf("%.2f\r\n",get_l_speed());
        }
    }
    if(rrun == true)
    {
        if(RRPM > 0)
        {
            Rmotor_speed(pid_step(Rmotor_pid, get_r_speed(), RRPM));
        }
        else
        {
            Rmotor_speed(pid_step(Rmotor_pid, -1*get_r_speed(), RRPM));
        }
    }
}