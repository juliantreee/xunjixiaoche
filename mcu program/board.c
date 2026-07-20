#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "board.h"
#include "delay.h"
#include "ti/driverlib/m0p/dl_core.h"

volatile unsigned char uart_data = 0;

/*============================================================================
 * 六轴传感器数据全局变量定义
 *===========================================================================*/

// 角度数据
struct SAngle stcAngle = {0};

// 角速度数据
struct SGyro stcGyro = {0};

// 加速度数据
struct SAccel stcAccel = {0};

// 四元数数据
struct SQuat stcQuat = {0};

void board_init(void)
{
	SYSCFG_DL_init();

	// 关闭 DMA RX 事件，改用 UART 中断接收
	DL_UART_Main_disableDMAReceiveEvent(UART_1_INST, DL_UART_DMA_INTERRUPT_RX);
	// 使能 UART RX 中断（外设级）
	DL_UART_Main_enableInterrupt(UART_1_INST, DL_UART_MAIN_INTERRUPT_RX);

	// 使能 NVIC 中断
	NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
}

//串口发送单个字符
void uart1_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_1_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_1_INST, ch);
}
//串口发送字符串
void uart1_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(str != NULL && *str != '\0')
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart1_send_char(*str++);
    }
}


void uart1_send_SendByte(uint8_t* data, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
    {
        uart1_send_char(data[i]);  // 直接发送原始字节
    }
}

#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	(void)x;
}
#endif



//串口的中断服务函数
void UART_1_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_1_INST) )
    {
        case DL_UART_IIDX_RX:
            uart_data = DL_UART_Main_receiveData(UART_1_INST);
            // 非阻塞回发：TX 忙则跳过，不在 ISR 中死等
            if (DL_UART_isBusy(UART_1_INST) == false) {
                DL_UART_Main_transmitData(UART_1_INST, uart_data);
            }
            break;

        default:
            break;
    }
}


/******************************************************************************
 * 返回当前Yaw角（Z轴角度），单位°，范围 -180 ~ 180
******************************************************************************/
float Yaw(void)
{
   return stcAngle.Yaw;
}
/******************************************************************************
 * 返回横滚角（单位：°），范围 -180 ~ 180
******************************************************************************/
float Roll(void)
{
    return stcAngle.Roll;
}

/******************************************************************************
 * 返回俯仰角（单位：°），范围 -180 ~ 180
******************************************************************************/
float Pitch(void)
{
    return stcAngle.Pitch;
}

/******************************************************************************
 * 返回X轴角速度（单位：°/s）
******************************************************************************/
float GyroX(void)
{
    return stcGyro.wx;
}

/******************************************************************************
 * 返回Y轴角速度（单位：°/s）
******************************************************************************/
float GyroY(void)
{
    return stcGyro.wy;
}

/******************************************************************************
 * 返回Z轴角速度（单位：°/s）
******************************************************************************/
float GyroZ(void)
{
    return stcGyro.wz;
}

/******************************************************************************
 * 返回X轴加速度（单位：m/s²）
******************************************************************************/
float AccelX(void)
{
    return stcAccel.ax;
}

/******************************************************************************
 * 返回Y轴加速度（单位：m/s²）
******************************************************************************/
float AccelY(void)
{
    return stcAccel.ay;
}

/******************************************************************************
 * 返回Z轴加速度（单位：m/s²）
******************************************************************************/
float AccelZ(void)
{
    return stcAccel.az;
}

/*============================================================================
 * 四元数数据获取接口函数
 *===========================================================================*/

/******************************************************************************
 * 获取四元数 q0
 * @return 四元数 q0 值
******************************************************************************/
float QuatQ0(void)
{
    return stcQuat.q0;
}

/******************************************************************************
 * 获取四元数 q1
 * @return 四元数 q1 值
******************************************************************************/
float QuatQ1(void)
{
    return stcQuat.q1;
}

/******************************************************************************
 * 获取四元数 q2
 * @return 四元数 q2 值
******************************************************************************/
float QuatQ2(void)
{
    return stcQuat.q2;
}

/******************************************************************************
 * 获取四元数 q3
 * @return 四元数 q3 值
******************************************************************************/
float QuatQ3(void)
{
    return stcQuat.q3;
}


/*============================================================================
 * 硬件 I2C 陀螺仪 (I2C_Gyro_INST = I2C0, PA8=SDA, PA14=SCL)
 *===========================================================================*/
#define GYRO_I2C_ADDR   0x48

uint8_t Gyro_Key[3]      = {0x13, 0x8E, 0x5F};
uint8_t Gyro_Yaw_Zero[3] = {0x0A, 0x04, 0x00};
uint8_t Gyro_Save[3]     = {0x00, 0x00, 0x00};
uint8_t Gyro_BIAS_CAL[3] = {0x0A, 0x01, 0x00};

#define GYRO_REG_GYRO   0xAA
#define GYRO_REG_ANGLE  0xBB
#define GYRO_REG_ACCEL  0xCC
#define GYRO_REG_QUAT   0xDD

static void Gyro_I2C_Write(uint8_t *data, uint8_t len)
{
    volatile uint32_t to;

    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--to == 0) return;
    }
    DL_I2C_fillControllerTXFIFO(I2C_Gyro_INST, data, len);
    DL_I2C_startControllerTransfer(I2C_Gyro_INST, GYRO_I2C_ADDR, DL_I2C_CONTROLLER_DIRECTION_TX, len);
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)) {
        if (--to == 0) return;
    }
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--to == 0) return;
    }
}

static uint8_t Gyro_I2C_ReadReg(uint8_t reg, uint8_t *data, uint8_t len)
{
    volatile uint32_t to;

    // 写寄存器地址
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--to == 0) return 0;
    }
    DL_I2C_fillControllerTXFIFO(I2C_Gyro_INST, &reg, 1);
    DL_I2C_startControllerTransfer(I2C_Gyro_INST, GYRO_I2C_ADDR, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)) {
        if (--to == 0) return 0;
    }
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--to == 0) return 0;
    }

    // 读数据
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--to == 0) return 0;
    }
    DL_I2C_startControllerTransfer(I2C_Gyro_INST, GYRO_I2C_ADDR, DL_I2C_CONTROLLER_DIRECTION_RX, len);
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)) {
        if (--to == 0) return 0;
    }
    to = 100000;
    while (!(DL_I2C_getControllerStatus(I2C_Gyro_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--to == 0) return 0;
    }

    for (uint8_t i = 0; i < len; i++) {
        data[i] = DL_I2C_receiveControllerData(I2C_Gyro_INST);
    }
    return 1;
}

void Get_senserdata(void)
{
    uint8_t buf[8] = {0};

    if (Gyro_I2C_ReadReg(GYRO_REG_GYRO, buf, 8)) {
        short wx = (short)((buf[1] << 8) | buf[0]);
        short wy = (short)((buf[3] << 8) | buf[2]);
        short wz = (short)((buf[5] << 8) | buf[4]);
        stcGyro.wx = (float)wx / 32768.0f * 2000.0f;
        stcGyro.wy = (float)wy / 32768.0f * 2000.0f;
        stcGyro.wz = (float)wz / 32768.0f * 2000.0f;
    }

    if (Gyro_I2C_ReadReg(GYRO_REG_ANGLE, buf, 8)) {
        short roll  = (short)((buf[1] << 8) | buf[0]);
        short pitch = (short)((buf[3] << 8) | buf[2]);
        short yaw   = (short)((buf[5] << 8) | buf[4]);
        stcAngle.Roll  = (float)roll  / 32768.0f * 180.0f;
        stcAngle.Pitch = (float)pitch / 32768.0f * 180.0f;
        stcAngle.Yaw   = (float)yaw   / 32768.0f * 180.0f;
    }

    if (Gyro_I2C_ReadReg(GYRO_REG_ACCEL, buf, 8)) {
        short ax = (short)((buf[1] << 8) | buf[0]);
        short ay = (short)((buf[3] << 8) | buf[2]);
        short az = (short)((buf[5] << 8) | buf[4]);
        const float G = 9.8f;
        stcAccel.ax = (float)ax / 32768.0f * 16.0f * G;
        stcAccel.ay = (float)ay / 32768.0f * 16.0f * G;
        stcAccel.az = (float)az / 32768.0f * 16.0f * G;
    }

    if (Gyro_I2C_ReadReg(GYRO_REG_QUAT, buf, 8)) {
        short q0 = (short)((buf[1] << 8) | buf[0]);
        short q1 = (short)((buf[3] << 8) | buf[2]);
        short q2 = (short)((buf[5] << 8) | buf[4]);
        short q3 = (short)((buf[7] << 8) | buf[6]);
        stcQuat.q0 = (float)q0 / 32768.0f;
        stcQuat.q1 = (float)q1 / 32768.0f;
        stcQuat.q2 = (float)q2 / 32768.0f;
        stcQuat.q3 = (float)q3 / 32768.0f;
    }
}

void sendCaliYawCommand(void)
{
    Gyro_I2C_Write(Gyro_Key, 3);
    delay_ms(100);
    Gyro_I2C_Write(Gyro_Yaw_Zero, 3);
    delay_ms(100);
    Gyro_I2C_Write(Gyro_Save, 3);
}

void performCaliBias(void)
{
    Gyro_I2C_Write(Gyro_Key, 3);
    delay_ms(100);
    Gyro_I2C_Write(Gyro_BIAS_CAL, 3);
    delay_ms(6000);
    Gyro_I2C_Write(Gyro_Save, 3);
}



//printf函数重定义
int fputc(int ch, FILE *stream)
{
	(void)stream;
	//当串口0忙的时候等待，不忙的时候再发送传进来的字符
	while( DL_UART_isBusy(UART_1_INST) == true );
	
	DL_UART_Main_transmitData(UART_1_INST, ch);
	
	return ch;
}






/* ================ 延时函数封装 =================== */

void delay_1us(int __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
void delay_1ms(int __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }
