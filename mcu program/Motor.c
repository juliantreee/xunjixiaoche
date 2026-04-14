#include "ti_msp_dl_config.h"
#include "Motor.h"


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

void MotorR_brake()
{
    DL_GPIO_setPins(MotorRCtl_PORT, MotorRCtl_Ain1_PIN);
    DL_GPIO_setPins(MotorRCtl_PORT, MotorRCtl_Ain2_PIN);
}

void MotorR_speed(double speed)
{
    DL_Timer_setCaptureCompareValue(Motor_INST, (uint32_t)speed*16, GPIO_Motor_C1_IDX);
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

void MotorL_brake()
{
    DL_GPIO_setPins(MotorLCtl_Bin1_PORT, MotorLCtl_Bin1_PIN);
    DL_GPIO_setPins(MotorLCtl_Bin2_PORT, MotorLCtl_Bin2_PIN);
}

void MotorL_speed(double speed)
{
    DL_Timer_setCaptureCompareValue(Motor_INST, (uint32_t)speed*16, GPIO_Motor_C0_IDX);
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

void hitbrake()
{
    MotorL_brake();
    MotorR_brake();
}

void cospeed(double speed)
{
    MotorL_speed(speed);
    MotorR_speed(speed);
}





























