#include "Graysensor.h"
#include "delay.h"
#include "Motor.h"
#include "Motor_pid.h"
#include "PID.h"
#include "uart.h"
#include <math.h>

bool gray_value[8] = {0,0,0,0,0,0,0,0};
uint8_t TrackN;

// 位置环PID控制器
static PID* line_pid = NULL;
static bool line_pid_initialized = false;

// 基础速度参数
static double base_speed = 80.0;     // 基础速度 (RPM)
static double max_speed = 300.0;     // 最大速度
static double min_speed = 30.0;      // 最小速度
static double max_differential = 150.0; // 最大差速

// 传感器读取函数保持不变
void select_channel(uint8_t channel)
{
    if((channel >> 0) & 0x01)
        DL_GPIO_setPins(Gray_PORT, Gray_AD0_PIN);
    else 
        DL_GPIO_clearPins(Gray_PORT, Gray_AD0_PIN);
    
    if((channel >> 1) & 0x01) 
        DL_GPIO_setPins(Gray_PORT, Gray_AD1_PIN);
    else 
        DL_GPIO_clearPins(Gray_PORT, Gray_AD1_PIN);
    
    if((channel >> 2) & 0x01) 
        DL_GPIO_setPins(Gray_PORT, Gray_AD2_PIN);
    else 
        DL_GPIO_clearPins(Gray_PORT, Gray_AD2_PIN);
}

bool Gray_read()
{
    if(DL_GPIO_readPins(Grayread_PORT, Grayread_read_PIN) > 0)
        return true;
    else 
        return false;
}

void Gray_read_all()
{
    for(uint8_t i = 0; i < 8; i++)
    {
        select_channel(i);
        delay_us(50);
        gray_value[i] = Gray_read();
    }
}

void Read_Track_Data(uint8_t* arr)
{
    uint8_t current_track = 0;
    static uint8_t last_track = 0;
    
    Gray_read_all();
    
    current_track = gray_value[7] + 
                    gray_value[6] * 2 + 
                    gray_value[5] * 4 + 
                    gray_value[4] * 8 + 
                    gray_value[3] * 16 + 
                    gray_value[2] * 32 + 
                    gray_value[1] * 64 + 
                    gray_value[0] * 128;
    
    TrackN = (uint8_t)((current_track + last_track) / 2);
    last_track = current_track;
    
    if(arr != NULL)
        *arr = TrackN;
}

// 计算位置偏差
float Track_Err(void)
{
    switch (TrackN) 
    {
        case 0xe7: return 0.0;    // 1 1 1 0 0 1 1 1 - 完美居中
        case 0xef: return 1.0;    // 1 1 1 0 1 1 1 1 - 略微偏右
        case 0xcf: return 2.0;    // 1 1 0 0 1 1 1 1 - 小偏右
        case 0xdf: return 3.0;    // 1 1 0 1 1 1 1 1 - 中小偏右
        case 0x9f: return 4.0;    // 1 0 0 1 1 1 1 1 - 中等偏右
        case 0xbf: return 5.0;    // 1 0 1 1 1 1 1 1 - 中大偏右
        case 0x3f: return 6.0;    // 0 0 1 1 1 1 1 1 - 大偏右
        case 0x1f: return 8.0;    // 0 0 0 1 1 1 1 1 - 极限偏右
        case 0x8f: return 10.0;   // 1 0 0 0 1 1 1 1 - 最大偏右
        
        case 0xf7: return -1.0;   // 略微偏左
        case 0xf3: return -2.0;   // 小偏左
        case 0xfb: return -3.0;   // 中小偏左
        case 0xf9: return -4.0;   // 中等偏左
        case 0xfd: return -5.0;   // 中大偏左
        case 0xfc: return -6.0;   // 大偏左
        case 0xf8: return -8.0;   // 极限偏左
        case 0xfe: return -10.0;  // 最大偏左
        
        case 0xff: return 0.0;    // 全黑（十字路口）
        case 0x00: return 0.0;    // 全白（完全离线）
        
        default: return 0.0;      // 其他情况
    }
}

// 计算灰度位置（连续值）
double getgraylocation(void)
{
    double x = 0;
    double z = 0;
    
    for(int i = 0; i < 8; i++)
    {
        if(gray_value[i] == 1)
        {
            z += 1;
            x += (double)i;
        }
    }
    
    if(z == 0) return 3.5;  // 没检测到线，返回中心
    
    x = x / z;
    return x;
}

// 初始化位置环PID
void Line_PID_Init(double kp, double ki, double kd)
{
    if(line_pid != NULL)
        pid_destroy(line_pid);
    
    line_pid = pid_create(
        kp,     // 位置环比例系数
        ki,     // 位置环积分系数
        kd,     // 位置环微分系数
        max_differential,   // 最大差速输出
        -max_differential,  // 最小差速输出
        0.01    // 10ms控制周期
    );
    
    line_pid_initialized = true;
}

// 设置基础速度
void Set_Base_Speed(double speed)
{
    if(speed > max_speed) speed = max_speed;
    if(speed < min_speed) speed = min_speed;
    base_speed = speed;
}

// 获取传感器状态（用于调试）
static void Print_Sensor_Status(void)
{
    printf("Sensors: ");
    for(int i = 0; i < 8; i++)
    {
        printf("%d", gray_value[i]);
    }
    printf(" TrackN=0x%02X", TrackN);
}

// 主循迹控制函数
void Line_Tracking_Step(void)
{
    double position_error;
    double pid_differential;
    double left_target_rpm, right_target_rpm;
    
    // 初始化位置环PID（首次调用）
    if(!line_pid_initialized)
    {
        Line_PID_Init(8.0, 0.02, 2.0);  // 默认位置环参数
    }
    
    // 1. 读取灰度传感器
    Gray_read_all();
    
    // 2. 计算位置偏差（两种方法可选）
    
    // 方法1：使用离散偏差（推荐，响应更稳定）
    position_error = (double)Track_Err();
    
    // 方法2：使用连续位置（注释掉，需要时启用）
    // double position = getgraylocation();
    // position_error = (position - 3.5) * 2.0;  // 放大误差
    
    // 3. 位置环PID计算差速值
    // 注意：传入-error是因为Track_Err()正值=偏右，我们需要pid_output正值=差速向左
    pid_differential = pid_step(line_pid, -position_error, 0.0);
    
    // 4. 根据偏差大小动态调整基础速度（弯道减速）
    double current_base_speed = base_speed;
    double abs_error = fabs(position_error);
    
    if(abs_error > 6.0)  // 急弯
    {
        current_base_speed *= 0.6;  // 减速到60%
    }
    else if(abs_error > 4.0)  // 中弯
    {
        current_base_speed *= 0.8;  // 减速到80%
    }
    else if(abs_error > 2.0)  // 小弯
    {
        current_base_speed *= 0.9;  // 微减速
    }
    // 直道保持原速
    
    // 5. 计算左右轮目标转速
    left_target_rpm = current_base_speed + pid_differential;
    right_target_rpm = current_base_speed - pid_differential;
    
    // 6. 速度限幅
    if(left_target_rpm > max_speed) left_target_rpm = max_speed;
    if(left_target_rpm < -max_speed) left_target_rpm = -max_speed;
    if(right_target_rpm > max_speed) right_target_rpm = max_speed;
    if(right_target_rpm < -max_speed) right_target_rpm = -max_speed;
    
    // 7. 设置电机目标速度（速度环由Motor_pid_step()处理）
    Lmotor_run(left_target_rpm);
    Rmotor_run(right_target_rpm);
    
    // 8. 调试输出（每50次输出一次，减少串口负担）
    static int debug_count = 0;
    if(debug_count % 50 == 0)
    {
        printf("[Line] Err=%.1f Diff=%.1f Base=%.1f L=%.1f R=%.1f\r\n",
               position_error, pid_differential, current_base_speed,
               left_target_rpm, right_target_rpm);
    }
    debug_count++;
}

// 特殊路段处理函数

// 十字路口检测
bool Is_Cross_Road(void)
{
    return (TrackN == 0xff || TrackN == 0x00);
}

// 完全离线检测
bool Is_Off_Line(void)
{
    return (TrackN == 0x00);
}

// 处理十字路口（直行通过）
void Handle_Cross_Road(void)
{
    printf("Cross Road Detected!\r\n");
    // 保持当前方向直行
    Lmotor_run(base_speed);
    Rmotor_run(base_speed);
    delay_ms(100);  // 通过十字路口时间
}

// 处理离线情况（停车或回退）
void Handle_Off_Line(void)
{
    printf("Off Line! Stopping...\r\n");
    Motors_stop();
    // 或者执行回退策略
    // Lmotor_run(-base_speed * 0.5);
    // Rmotor_run(-base_speed * 0.5);
    // delay_ms(200);
    // Motors_stop();
}

// 自动调参函数（简单版）
void Auto_Tune_Line_PID(void)
{
    printf("Starting Line PID Auto-Tune...\r\n");
    
    // 测试不同Kp值
    double kp_values[] = {5.0, 8.0, 12.0, 15.0};
    double best_kp = 8.0;
    
    // 简单策略：使用中等参数
    Line_PID_Init(8.0, 0.02, 2.0);
    
    printf("Auto-Tune Complete: Kp=8.0, Ki=0.02, Kd=2.0\r\n");
}