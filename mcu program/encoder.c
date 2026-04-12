#include "ti_msp_dl_config.h"
#include "encoder.h"


static volatile bool     LgCapDone;          // 测量完成标志
static volatile bool     LgFirstEdge;        // 是否已捕获第一个边沿

const static uint32_t gLoadValue = 0;        // 定时器重载值

static volatile bool     RgCapDone;          // 测量完成标志
static volatile bool     RgFirstEdge;        // 是否已捕获第一个边沿

static uint32_t Lmotor_period;
static uint32_t Rmotor_period;

void Encoder_Init(void)
{
    LgFirstEdge = false;
    LgCapDone   = false;
    RgFirstEdge = false;
    RgCapDone   = false;


    //调试时停止定时器
    DL_TimerA_setCoreHaltBehavior(LMotor_INST, DL_TIMER_CORE_HALT_IMMEDIATE);
    DL_TimerA_setCoreHaltBehavior(RMotor_INST, DL_TIMER_CORE_HALT_IMMEDIATE);


}

void get_period(void)
{
    LgFirstEdge = false;
    LgCapDone = false;
    RgFirstEdge = false;
    RgCapDone = false;
    

    //使能中断
    NVIC_EnableIRQ(LMotor_INST_INT_IRQN);
    NVIC_EnableIRQ(RMotor_INST_INT_IRQN);
    //清空定时器
    DL_TimerA_setTimerCount(LMotor_INST, gLoadValue);
    DL_TimerA_setTimerCount(RMotor_INST, gLoadValue);
    //启动定时器
    DL_TimerA_startCounter(LMotor_INST);
    DL_TimerA_startCounter(RMotor_INST);

    while (LgCapDone == false || RgCapDone == false)
    {
        __NOP();
    }

    //关闭定时器
    DL_TimerA_stopCounter(LMotor_INST);
    DL_TimerA_stopCounter(RMotor_INST);
    
    //失能中断
    NVIC_DisableIRQ(LMotor_INST_INT_IRQN);
    NVIC_DisableIRQ(RMotor_INST_INT_IRQN);

    //计算

    Lmotor_angspeed = (PII/PPR)/(Lmotor_period/Timerf);
    Rmotor_angspeed = (PII/PPR)/(Rmotor_period/Timerf);

}


//中断函数
void LMotor_INST_IRQHandler(void)
{
    if(LgFirstEdge == false)
    {
        DL_TimerA_setTimerCount(LMotor_INST, gLoadValue);
        LgFirstEdge = true;
    }
    else
    {
        Lmotor_period = DL_TimerA_getCaptureCompareValue(LMotor_INST, DL_TIMER_CC_0_INDEX);
        LgCapDone = true;
        LgFirstEdge = false;
    }
}

void RMotor_INST_IRQHandler(void)
{
    if(RgFirstEdge == false)
    {
        DL_TimerA_setTimerCount(RMotor_INST, gLoadValue);
        RgFirstEdge = true;
    }
    else
    {
        Rmotor_period = DL_TimerA_getCaptureCompareValue(RMotor_INST, DL_TIMER_CC_0_INDEX);
        RgCapDone = true;
        RgFirstEdge = false;
    }
}