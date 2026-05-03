#include "ti_msp_dl_config.h"
#include "encoder.h"
#include <ti/driverlib/m0p/dl_interrupt.h>
#include <ti/driverlib/dl_timerg.h>
#include "uart.h"
#include <stdio.h>
#include <stdbool.h>

volatile int LF;
volatile bool LD,Ltimeout;
volatile uint16_t LFCap,LSCap;

void Encoder_Init(void)
{
    LF = 0;
    LD = false;
    Ltimeout = false;

    //一大堆清理中断的措施，以确保不抽风，我也不知道到底哪一项起了作用
    DL_TimerG_clearInterruptStatus(LMotor_INST, DL_TIMER_IIDX_ZERO);

    DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_L_encoder_PIN);

    NVIC_DisableIRQ(LMotor_INST_INT_IRQN);
    NVIC_DisableIRQ(Encoder_INT_IRQN);

    NVIC_ClearPendingIRQ(LMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_INT_IRQN);
    //内存屏蔽保证操作完成，deepseek大夫的手笔，我也看不懂
    __DSB();
    __ISB();
    DL_TimerG_startCounter(LMotor_INST);
}

double get_l_speed(void)
{
    //重置状态标识
    LF = 0;
    LD = false;
    Ltimeout = false;

    //又是清除中断
    DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_L_encoder_PIN);
    NVIC_ClearPendingIRQ(LMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_INT_IRQN);
    __DSB();
    __ISB();

    DL_TimerG_setTimerCount(LMotor_INST, 65535);//重置定时器计数值
    //DL_TimerG_startCounter(LMotor_INST);//启动定时器
    //使能中断
    NVIC_EnableIRQ(LMotor_INST_INT_IRQN);
    NVIC_EnableIRQ(Encoder_INT_IRQN);
    //等待测量
    while (LD == false && Ltimeout == false)
    {
        __WFE();
    }
    //失能中断
    NVIC_DisableIRQ(Encoder_INT_IRQN);
    NVIC_DisableIRQ(LMotor_INST_INT_IRQN);
    //清除中断
    DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_L_encoder_PIN);
    NVIC_ClearPendingIRQ(LMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_INT_IRQN);
    __DSB();
    __ISB();
    
    if(Ltimeout == true) //若超时
    {
        return 0.1;
    }
    else
    {
        return 1920000.0/(LFCap -LSCap);
    }
}


//GPIO中断函数
void GROUP1_IRQHandler(void)
{
    switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB: //左电机中断
        {
            if(LF == 0)  //第一次中断
            {
                LFCap = DL_TimerG_getTimerCount(LMotor_INST);  //读取当前计数值
                DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_L_encoder_PIN);
                LF+=1;
            }
            else if(LF <= 9)
            {
                LF+=1;
            }
            else   //第十一次中断
            {
                LSCap = DL_TimerG_getTimerCount(LMotor_INST);  //读取当前计数值
                DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_L_encoder_PIN);
                LD = true;
            }
            break;
        }
        default:
            break;
    }
}

//定时器中断函数
void LMotor_INST_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(LMotor_INST))
    {
        case DL_TIMER_IIDX_ZERO://超时
        {
            Ltimeout = true;
            DL_TimerG_clearInterruptStatus(LMotor_INST, DL_TIMER_IIDX_ZERO);
            break;
        }
        default:
            break;
    }
}