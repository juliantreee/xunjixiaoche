#include "ti_msp_dl_config.h"
#include "encoder.h"
#include <ti/driverlib/m0p/dl_interrupt.h>
#include <ti/driverlib/dl_timerg.h>
#include "uart.h"
#include <stdio.h>
#include <stdbool.h>

volatile bool LF,LD,RF,RD,Ltimeout,Rtimeout;
volatile uint16_t LFCap,LSCap,RFCap,RSCap;
volatile uint16_t Lperiod,Rperiod;

void Encoder_Init(void)
{
    LF = false;
    LD = false;
    RF = false;
    RD = false;
    Ltimeout = false;
    Rtimeout = false;

    //一大堆清理中断的措施，以确保不抽风，我也不知道到底哪一项起了作用
    DL_TimerG_clearInterruptStatus(LMotor_INST, DL_TIMER_IIDX_ZERO);
    DL_TimerG_clearInterruptStatus(RMotor_INST, DL_TIMER_IIDX_ZERO);

    DL_GPIO_clearInterruptStatus(Encoder_L_encoder_PORT, Encoder_L_encoder_PIN);
    DL_GPIO_clearInterruptStatus(Encoder_R_encoder_PORT, Encoder_R_encoder_PIN);

    NVIC_DisableIRQ(LMotor_INST_INT_IRQN);
    NVIC_DisableIRQ(RMotor_INST_INT_IRQN);
    NVIC_DisableIRQ(Encoder_GPIOB_INT_IRQN);

    NVIC_ClearPendingIRQ(LMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(RMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOB_INT_IRQN);
    //内存屏蔽保证操作完成，deepseek大夫的手笔，我也看不懂
    __DSB();
    __ISB();
    DL_TimerG_startCounter(LMotor_INST);
    DL_TimerG_startCounter(RMotor_INST);
}

double get_l_speed(void)
{
    //重置状态标识
    LF = false;
    LD = false;
    Ltimeout = false;

    //又是清除中断
    DL_GPIO_clearInterruptStatus(Encoder_L_encoder_PORT, Encoder_L_encoder_PIN);
    NVIC_ClearPendingIRQ(LMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOB_INT_IRQN);
    __DSB();
    __ISB();

    DL_TimerG_setTimerCount(LMotor_INST, 65535);//重置定时器计数值
    //DL_TimerG_startCounter(LMotor_INST);//启动定时器
    //使能中断
    NVIC_EnableIRQ(LMotor_INST_INT_IRQN);
    NVIC_EnableIRQ(Encoder_GPIOB_INT_IRQN);
    //等待测量
    while (LD == false && Ltimeout == false)
    {
        __WFE();
    }
    //失能中断
    NVIC_DisableIRQ(Encoder_GPIOB_INT_IRQN);
    NVIC_DisableIRQ(LMotor_INST_INT_IRQN);
    //清除中断
    DL_GPIO_clearInterruptStatus(Encoder_L_encoder_PORT, Encoder_L_encoder_PIN);
    NVIC_ClearPendingIRQ(LMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOB_INT_IRQN);
    __DSB();
    __ISB();
    
    if(Ltimeout == true) //若超时
    {
        return 0.1;
    }
    else
    {
        Lperiod = LFCap = LSCap;
        return 192000.0/Lperiod;
    }
}

double get_r_speed()
{
    //重置状态标识
    RF = false;
    RD = false;
    Rtimeout = false;

    //又是清除中断
    DL_GPIO_clearInterruptStatus(Encoder_R_encoder_PORT, Encoder_R_encoder_PIN);
    NVIC_ClearPendingIRQ(RMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOA_INT_IRQN);
    __DSB();
    __ISB();

    DL_TimerG_setTimerCount(RMotor_INST, 65535);//重置定时器计数值
    //DL_TimerG_startCounter(RMotor_INST);//启动定时器
    //使能中断
    NVIC_EnableIRQ(RMotor_INST_INT_IRQN);
    NVIC_EnableIRQ(Encoder_GPIOA_INT_IRQN);
    //等待测量
    while (RD == false && Rtimeout == false)
    {
        __WFE();
    }
    //失能中断
    NVIC_DisableIRQ(Encoder_GPIOA_INT_IRQN);
    NVIC_DisableIRQ(RMotor_INST_INT_IRQN);
    //清除中断
    DL_GPIO_clearInterruptStatus(Encoder_R_encoder_PORT, Encoder_R_encoder_PIN);
    NVIC_ClearPendingIRQ(RMotor_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Encoder_GPIOA_INT_IRQN);
    __DSB();
    __ISB();
    
    if(Rtimeout == true) //若超时
    {
        return 0.1;
    }
    else
    {
        Rperiod = RFCap -RSCap;
        return 192000.0/Rperiod;
    }
}

//GPIO中断函数
void GROUP1_IRQHandler(void)
{
    switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB: //左电机中断
        {
            if(LF == false)  //第一次中断
            {
                LFCap = DL_TimerG_getTimerCount(LMotor_INST);  //读取当前计数值
                DL_GPIO_clearInterruptStatus(Encoder_L_encoder_PORT, Encoder_L_encoder_PIN);
                LF = true;
            }
            else   //第二次中断
            {
                LSCap = DL_TimerG_getTimerCount(LMotor_INST);  //读取当前计数值
                DL_GPIO_clearInterruptStatus(Encoder_L_encoder_PORT, Encoder_L_encoder_PIN);
                LD = true;
            }
            break;
        }
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA: //右电机中断
        {
            if(RF == false)  //第一次中断
            {
                RFCap = DL_TimerG_getTimerCount(RMotor_INST);  //读取当前计数值
                DL_GPIO_clearInterruptStatus(Encoder_R_encoder_PORT, Encoder_R_encoder_PIN);
                RF = true;
            }
            else   //第二次中断
            {
                RSCap = DL_TimerG_getTimerCount(RMotor_INST);  //读取当前计数值
                DL_GPIO_clearInterruptStatus(Encoder_R_encoder_PORT, Encoder_R_encoder_PIN);
                RD = true;
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

void RMotor_INST_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(RMotor_INST))
    {
        case DL_TIMER_IIDX_ZERO://超时
        {
            Rtimeout = true;
            DL_TimerG_clearInterruptStatus(RMotor_INST, DL_TIMER_IIDX_ZERO);
            break;
        }
        default:
            break;
    }
}
