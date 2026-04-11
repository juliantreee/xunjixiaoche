#include "ti_msp_dl_config.h"
#include "Motor.h"

double speed_l = 1.00;
double speed_r = 1.00;

void MotorR_forward()
{
    DL_GPIO_setPins(MotorRCtl_PORT, MotorRCtl_Ain2_PIN);
    DL_GPIO_clearPins(MotorRCtl_PORT, MotorRCtl_Ain1_PIN);
}

void MotorR_backward()
{
    DL_GPIO_setPins(MotorRCtl_PORT, MotorRCtl_Ain1_PIN);
    DL_GPIO_clearPins(MotorRCtl_PORT, MotorRCtl_Ain2_PIN);
}

void MotorR_stop()
{
    DL_GPIO_clearPins(MotorRCtl_PORT, MotorRCtl_Ain1_PIN);
    DL_GPIO_clearPins(MotorRCtl_PORT, MotorRCtl_Ain2_PIN);
}

void MotorR_break()
{
    DL_GPIO_setPins(MotorRCtl_PORT, MotorRCtl_Ain1_PIN);
    DL_GPIO_setPins(MotorRCtl_PORT, MotorRCtl_Ain2_PIN);
}

void MotorR_speed(char speed)
{
    DL_Timer_setCaptureCompareValue(Motor_INST, (uint32_t)speed*16*speed_r, GPIO_Motor_C1_IDX);
}


void MotorL_forward()
{
    DL_GPIO_setPins(MotorLCtl_Bin1_PORT, MotorLCtl_Bin1_PIN);
    DL_GPIO_clearPins(MotorLCtl_Bin2_PORT, MotorLCtl_Bin2_PIN);
}

void MotorL_backward()
{
    DL_GPIO_setPins(MotorLCtl_Bin2_PORT, MotorLCtl_Bin2_PIN);
    DL_GPIO_clearPins(MotorLCtl_Bin1_PORT, MotorLCtl_Bin1_PIN);
}

void MotorL_stop()
{
    DL_GPIO_clearPins(MotorLCtl_Bin1_PORT, MotorLCtl_Bin1_PIN);
    DL_GPIO_clearPins(MotorLCtl_Bin2_PORT, MotorLCtl_Bin2_PIN);
}

void MotorL_break()
{
    DL_GPIO_setPins(MotorLCtl_Bin1_PORT, MotorLCtl_Bin1_PIN);
    DL_GPIO_setPins(MotorLCtl_Bin2_PORT, MotorLCtl_Bin2_PIN);
}

void MotorL_speed(char speed)
{
    DL_Timer_setCaptureCompareValue(Motor_INST, (uint32_t)speed*16*speed_l, GPIO_Motor_C0_IDX);
}


void Go_forward()
{
    MotorL_forward();
    MotorR_forward();
}

void Go_backward()
{
    MotorL_backward();
    MotorR_backward();
}

void stopnow()
{
    MotorL_stop();
    MotorR_stop();
}

void hitbreak()
{
    MotorL_break();
    MotorR_break();
}

void cospeed(char speed)
{
    MotorL_speed(speed);
    MotorR_speed(speed);
}





























