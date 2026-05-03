#include "ti_msp_dl_config.h"
#include "encoder.h"
#include <ti/driverlib/m0p/dl_interrupt.h>
#include <ti/driverlib/dl_timerg.h>

bool LF,LD,RF,RD,Ltimeout,Rtimeout;
uint16_t LFCap,LSCap,RFCap,LFCap;
uint16_t Lperiod,Rperiod;

void Encoder_Init(void)
{
    LF = false;
    LD = false;
    RF = false;
    RD = false;
    Ltimeout = false;
    Rtimeout = false;
    NVIC_DisableIRQ(RMotor_INST_INT_IRQN);
}

double get_l_speed(void)
{
    //重置状态标识
    LF = false;
    LD = false;
    Ltimeout = false;

    DL_TimerG_setTimerCount(LMotor_INST, 65535);//重置定时器计数值
    DL_TimerG_startCounter(LMotor_INST);//启动定时器
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
    if(Ltimeout == true) //若超时
    {
        return (Timerf * 60.0) / ((LFCap - LSCap)*PPR);
    }
    else
    {
        return (Timerf * 60.0) / ((LFCap - LSCap)*PPR);
    }
}


//GPIO中断函数
void GROUP1_IRQHandler(void)
{
    switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB: //左电机中断
        {
            DL_UART_transmitData(UART0, 'L');
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
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
            DL_UART_transmitData(UART0, 'R');
            break;
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

