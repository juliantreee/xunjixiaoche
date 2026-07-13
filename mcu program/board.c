#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "board.h"
#include "ti/driverlib/m0p/dl_core.h"

volatile unsigned int delay_times = 0;
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
	// SYSCFG初始化
	SYSCFG_DL_init();
	//清除串口中断标志
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	//使能串口中断
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	
	//清除串口中断标志
	NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
	//使能串口中断
	NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
	
	
}

//串口发送单个字符
void uart0_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_0_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_0_INST, ch);
}
//串口发送字符串
void uart0_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart0_send_char(*str++);
    }
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
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart1_send_char(*str++);
    }
}


void uart0_send_SendByte(uint8_t* data, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
    {
        uart0_send_char(data[i]);  // 直接发送原始字节
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
	x = x;
}
#endif



/******************************************************************************
 * 数据解析函数：接收0x5A开头的数据帧
 * 支持：角速度(0xAA)、角度(0xBB)、加速度(0xCC)、四元数(0xDD)
 ******************************************************************************/
void CopeSerial2Data(unsigned char ucData)
{
    static unsigned char ucRxBuffer[11];
    static unsigned char ucRxCnt = 0;
    unsigned char sum = 0;
    int i;

    // 缓存数据
    ucRxBuffer[ucRxCnt++] = ucData;

    // 帧头校验
    if (ucRxBuffer[0] != 0x5A)
    {
        ucRxCnt = 0;
        return;
    }

    // 帧类型判断，确定帧长度
    // 加速度/角速度/角度帧: 11字节 (0x5A + TYPE + 4组数据 + SUM)
    // 四元数帧: 11字节 (0x5A + TYPE + 4组数据 + SUM)
    // 寄存器读报包: 11字节
    if (ucRxCnt < 11) return;  // 等待完整帧

    // 根据TYPE计算校验和
    switch (ucRxBuffer[1])
    {
        case 0xAA:  // 角速度
            sum = ucRxBuffer[0] + ucRxBuffer[1] +
                  ucRxBuffer[2] + ucRxBuffer[3] +   // WxL, WxH
                  ucRxBuffer[4] + ucRxBuffer[5] +   // WyL, WyH
                  ucRxBuffer[6] + ucRxBuffer[7] +   // WzL, WzH
                  ucRxBuffer[8] + ucRxBuffer[9];    // 0x00, 0x00
            
            if (sum != ucRxBuffer[10])
            {
                ucRxCnt = 0;
                return;
            }
            
            // 解析角速度
            {
                short wx = (short)((ucRxBuffer[3] << 8) | ucRxBuffer[2]);
                short wy = (short)((ucRxBuffer[5] << 8) | ucRxBuffer[4]);
                short wz = (short)((ucRxBuffer[7] << 8) | ucRxBuffer[6]);
                
                stcGyro.wx = (float)wx / 32768.0f * 2000.0f;  // °/s
                stcGyro.wy = (float)wy / 32768.0f * 2000.0f;
                stcGyro.wz = (float)wz / 32768.0f * 2000.0f;
            }
            break;
            
        case 0xBB:  // 角度
            sum = ucRxBuffer[0] + ucRxBuffer[1] +
                  ucRxBuffer[2] + ucRxBuffer[3] +   // RollL, RollH
                  ucRxBuffer[4] + ucRxBuffer[5] +   // PitchL, PitchH
                  ucRxBuffer[6] + ucRxBuffer[7] +   // YawL, YawH
                  ucRxBuffer[8] + ucRxBuffer[9];    // 0x00, 0x00
            
            if (sum != ucRxBuffer[10])
            {
                ucRxCnt = 0;
                return;
            }
            
            // 解析角度
            {
                short roll  = (short)((ucRxBuffer[3] << 8) | ucRxBuffer[2]);
                short pitch = (short)((ucRxBuffer[5] << 8) | ucRxBuffer[4]);
                short yaw   = (short)((ucRxBuffer[7] << 8) | ucRxBuffer[6]);
                
                stcAngle.Roll  = (float)roll  / 32768.0f * 180.0f;  // °
                stcAngle.Pitch = (float)pitch / 32768.0f * 180.0f;
                stcAngle.Yaw   = (float)yaw   / 32768.0f * 180.0f;
            }
            break;
            
        case 0xCC:  // 加速度
            sum = ucRxBuffer[0] + ucRxBuffer[1] +
                  ucRxBuffer[2] + ucRxBuffer[3] +   // AxL, AxH
                  ucRxBuffer[4] + ucRxBuffer[5] +   // AyL, AyH
                  ucRxBuffer[6] + ucRxBuffer[7] +   // AzL, AzH
                  ucRxBuffer[8] + ucRxBuffer[9];    // 0x00, 0x00
            
            if (sum != ucRxBuffer[10])
            {
                ucRxCnt = 0;
                return;
            }
            
            // 解析加速度
            {
                short ax = (short)((ucRxBuffer[3] << 8) | ucRxBuffer[2]);
                short ay = (short)((ucRxBuffer[5] << 8) | ucRxBuffer[4]);
                short az = (short)((ucRxBuffer[7] << 8) | ucRxBuffer[6]);
                
                const float G = 9.8f;  // 重力加速度
                stcAccel.ax = (float)ax / 32768.0f * 16.0f * G;  // m/s²
                stcAccel.ay = (float)ay / 32768.0f * 16.0f * G;
                stcAccel.az = (float)az / 32768.0f * 16.0f * G;
            }
            break;
            
        case 0xDD:  // 四元数
            sum = ucRxBuffer[0] + ucRxBuffer[1] +
                  ucRxBuffer[2] + ucRxBuffer[3] +   // Q0L, Q0H
                  ucRxBuffer[4] + ucRxBuffer[5] +   // Q1L, Q1H
                  ucRxBuffer[6] + ucRxBuffer[7] +   // Q2L, Q2H
                  ucRxBuffer[8] + ucRxBuffer[9];    // Q3L, Q3H
            
            if (sum != ucRxBuffer[10])
            {
                ucRxCnt = 0;
                return;
            }
            
            // 解析四元数
            {
                short q0 = (short)((ucRxBuffer[3] << 8) | ucRxBuffer[2]);
                short q1 = (short)((ucRxBuffer[5] << 8) | ucRxBuffer[4]);
                short q2 = (short)((ucRxBuffer[7] << 8) | ucRxBuffer[6]);
                short q3 = (short)((ucRxBuffer[9] << 8) | ucRxBuffer[8]);
                
                stcQuat.q0 = (float)q0 / 32768.0f;
                stcQuat.q1 = (float)q1 / 32768.0f;
                stcQuat.q2 = (float)q2 / 32768.0f;
                stcQuat.q3 = (float)q3 / 32768.0f;
            }
            break;
            
        case 0xEE:  // 寄存器读报包（可根据需要解析）
            // 寄存器读报包的格式与上述类似，可按需处理
            sum = ucRxBuffer[0] + ucRxBuffer[1] +
                  ucRxBuffer[2] + ucRxBuffer[3] +
                  ucRxBuffer[4] + ucRxBuffer[5] +
                  ucRxBuffer[6] + ucRxBuffer[7] +
                  ucRxBuffer[8] + ucRxBuffer[9];
            
            if (sum != ucRxBuffer[10])
            {
                ucRxCnt = 0;
                return;
            }
            // 寄存器数据读取处理
            break;
            
        default:
            // 未知类型，复位
            ucRxCnt = 0;
            return;
    }
    
    // 解析成功，复位接收计数器
    ucRxCnt = 0;
}

//串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_0_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
						// 接收发送过来的数据
            uart_data = DL_UART_Main_receiveData(UART_0_INST);
            // 调用数据解析函数
            CopeSerial2Data(uart_data);
            break;

        default://其他的串口中断
            break;
    }
}

//串口的中断服务函数
void UART_1_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_1_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //接发送过来的数据保存在变量中
            uart_data = DL_UART_Main_receiveData(UART_1_INST);
            //将保存的数据再发送出去
            uart1_send_char(uart_data);
            break;

        default://其他的串口中断
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


//解锁指令
uint8_t Key[5] = {0x55, 0xAA, 0x13, 0x8E, 0x5F};
//Z轴角度归零指令
uint8_t Yaw_Zero[5] = {0x55, 0xAA, 0x0A, 0x04, 0x00};
//保存指令
uint8_t Save[5] = {0x55, 0xAA, 0x00, 0x00, 0x00};
//获取零偏指令
uint8_t BIAS_CAL[5] = {0x55, 0xAA, 0x0A, 0x01, 0x00};

/****************************************************************************** 
 * 发送 Z轴角度归零命令
******************************************************************************/ 
void sendCaliYawCommand(void) 
{ 
   uart0_send_SendByte(Key, 5);
	 delay_ms(100);
	 uart0_send_SendByte(Yaw_Zero, 5);
	 delay_ms(100);
	 uart0_send_SendByte(Save, 5);
}


/****************************************************************************** 
* 发送校准指令(校准过程中请勿移动，否则会校准失败或者校准效果不好)
******************************************************************************/ 
void performCaliBias(void) 
{ 
   uart0_send_SendByte(Key, 5);
	 delay_ms(100);
	 uart0_send_SendByte(BIAS_CAL, 5);
	 delay_ms(6000);
	 uart0_send_SendByte(Save, 5);
}



//printf函数重定义
int fputc(int ch, FILE *stream)
{
	//当串口0忙的时候等待，不忙的时候再发送传进来的字符
	while( DL_UART_isBusy(UART_1_INST) == true );
	
	DL_UART_Main_transmitData(UART_1_INST, ch);
	
	return ch;
}






/* ================ 延时函数封装 =================== */

void delay_1us(int __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
void delay_1ms(int __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }
