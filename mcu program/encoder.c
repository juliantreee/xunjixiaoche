#include "ti_msp_dl_config.h"
#include "encoder.h"


static volatile uint32_t LgCapVal1;          // 第一个变化沿捕获值
static volatile uint32_t LgCapVal2;          // 第二个变化沿捕获值
static volatile bool     LgCapDone;          // 测量完成标志
static volatile bool     LgFirstEdge;        // 是否已捕获第一个边沿

static volatile uint32_t gLoadValue;        // 定时器重载值

static volatile uint32_t RgCapVal1;          // 第一个变化沿捕获值
static volatile uint32_t RgCapVal2;          // 第二个变化沿捕获值
static volatile bool     RgCapDone;          // 测量完成标志
static volatile bool     RgFirstEdge;        // 是否已捕获第一个边沿


static volatile uint32_t Lmotor_period;
static volatile uint32_t Rmotor_period;

static double Lmotor_angspeed; //弧度速度
static double Rmotor_angspeed; //弧度速度

void Encoder_Init(void)
{
    LgFirstEdge = false;
    LgCapDone   = false;
    RgFirstEdge = false;
    RgCapDone   = false;

    gLoadValue = DL_TimerG_getLoadValue(LMotor_INST);

    //调试时停止定时器
    DL_TimerG_setCoreHaltBehavior(LMotor_INST, DL_TIMER_CORE_HALT_IMMEDIATE);
    DL_TimerG_setCoreHaltBehavior(RMotor_INST, DL_TIMER_CORE_HALT_IMMEDIATE);


}

void get_period(void)
{
    LgFirstEdge = false;
    LgCapDone = false;
    RgFirstEdge = false;
    RgCapDone = false;
 
    //清空中断
    DL_GPIO_clearInterruptStatus(GPIOA, 17);
    DL_GPIO_clearInterruptStatus(GPIOB, 8);
    DL_TimerG_clearInterruptStatus(LMotor_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    DL_TimerG_clearInterruptStatus(RMotor_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    

    
    //清空定时器
    DL_TimerG_setTimerCount(LMotor_INST, gLoadValue);
    DL_TimerG_setTimerCount(RMotor_INST, gLoadValue);
    //启动定时器
    DL_TimerG_startCounter(LMotor_INST);
    DL_TimerG_startCounter(RMotor_INST);
    //使能中断
    NVIC_EnableIRQ(LMotor_INST_INT_IRQN);
    NVIC_EnableIRQ(RMotor_INST_INT_IRQN);
    NVIC_EnableIRQ(Encoder_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(Encoder_GPIOB_INT_IRQN);

    while (LgCapDone == false || RgCapDone == false)
    {
        __WFI();
    }
    

    //计算
    Lmotor_period = LgCapVal1 - LgCapVal2;
    Rmotor_period = RgCapVal1 - RgCapVal2;
    Lmotor_angspeed = (PII/PPR)/(Lmotor_period/Timerf);
    Rmotor_angspeed = (PII/PPR)/(Rmotor_period/Timerf);

}


//中断函数

void GROUP1_IRQHandler()
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case Encoder_GPIOA_INT_IIDX:
            if(RgFirstEdge == false)
            {
                RgCapVal1 = DL_TimerG_getTimerCount(RMotor_INST);
                DL_GPIO_clearInterruptStatus(GPIOA, 17);
                RgFirstEdge = true;
                break;
            }
            else 
            {
                RgCapVal2 = DL_TimerG_getTimerCount(RMotor_INST);
                NVIC_DisableIRQ(Encoder_GPIOA_INT_IRQN);
                NVIC_DisableIRQ(RMotor_INST_INT_IRQN);
                DL_TimerG_stopCounter(RMotor_INST);
                DL_GPIO_clearInterruptStatus(GPIOA, 17);
                RgCapDone = true;
                break;
            }
        case Encoder_GPIOB_INT_IIDX:
            if(LgFirstEdge == false)
            {
                LgCapVal1 = DL_TimerG_getTimerCount(LMotor_INST);
                DL_GPIO_clearInterruptStatus(GPIOB, 8);
                LgFirstEdge = true;
                break;
            }
            else
            {
                LgCapVal2 = DL_TimerG_getTimerCount(LMotor_INST);
                NVIC_DisableIRQ(Encoder_GPIOB_INT_IRQN);
                NVIC_DisableIRQ(LMotor_INST_INT_IRQN);
                DL_TimerG_stopCounter(LMotor_INST);
                DL_GPIO_clearInterruptStatus(GPIOB, 8);
                LgCapDone = true;
                break;
            }
        default:
            break;
    }
}

void LMotor_INST_IRQHandler(void)
{
    LgCapDone = true;
    Lmotor_period = 4294967295;
    DL_TimerG_clearInterruptStatus(LMotor_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
}
void RMotor_INST_IRQHandler(void)
{
    RgCapDone = true;
    Rmotor_period = 4294967295;
    DL_TimerG_clearInterruptStatus(RMotor_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
}
