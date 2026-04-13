#include "PID.h"
#include <stdlib.h>

struct PID{
    //pid系数
    double Kp;
    double Ki;
    double Kd;

    double error;       //此次误差
    double lasterror;   //上次误差
    double ierror;      //误差积分
    double derror;      //误差微分

    double nowValue;    //此次的值
    double targetValue; //所需值

    double CtlValue;    //输出控制值
};

//PID对象创建函数
PID* pid_create(double kp, double ki, double kd)
{
    PID* p = (PID*)malloc(sizeof(PID));
    p->Kp = kp;
    p->Ki = ki;
    p->Kd = kd;
    p->lasterror = 0.0;
    p->ierror = 0.0;
    return p;
}

double pid_step(PID *in,double value)  //传入当前值,返回控制值
{
    in->nowValue = value;

    in->error = in->targetValue - in->nowValue;
    in->ierror += in->error;
    in->derror = in->error - in->lasterror;

    in->lasterror = in->error;

    in->CtlValue = in->Kp * in->error + in->Ki * in->ierror + in->Kd * in->derror;
    return in-> CtlValue;
}

//设置目标值
void pid_set_target(PID *in,double target)
{
    in->targetValue = target;
}