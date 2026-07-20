/**
 * @file    INS.c
 * @brief   惯性导航模块：IMU 加速度积分 + 运动原语（直线/圆弧）
 *
 *          依赖：board.c（IMU 数据）、Motor_pid（电机速度闭环）、delay（毫秒延时）
 *          注意：Cortex-M0+ 无 FPU，所有 float 为软件模拟，控制更新频率。
 */

#include "INS.h"
#include "board.h"
#include "Motor_pid.h"
#include "delay.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/* ---------- 惯性导航参数 ---------- */

#define GRAVITY         9.8f    // 重力加速度 (m/s^2)
#define ACCEL_THRESHOLD 0.05f   // 加速度死区：低于此值视为静止
#define VEL_DECAY       0.999f  // 每步速度衰减：抑制零速漂移

/* ---------- 全局状态 ---------- */

INS_State ins;  // 导航状态：位置、速度、航向

/* ==================================================================
 *                      惯性导航核心
 * ================================================================== */

/**
 * @brief  初始化导航状态，位置/速度归零
 */
void INS_Init(void)
{
    ins.x   = 0.0f;
    ins.y   = 0.0f;
    ins.vx  = 0.0f;
    ins.vy  = 0.0f;
    ins.yaw = 0.0f;
    ins.dt  = 0.0f;
}

/**
 * @brief  导航更新，应在主循环中周期性调用（如每 5ms）
 * @param  dt_s  距上次调用的时间间隔 (秒)
 *
 *          处理流程：
 *          1. 读取 IMU 欧拉角 + 机体加速度
 *          2. 用 Roll/Pitch 补偿重力在机体轴上的投影
 *          3. 死区滤波 → 机体→世界坐标旋转 → 积分得到速度/位置
 */
void INS_Update(float dt_s)
{
    if (dt_s <= 0.0f) return;
    ins.dt = dt_s;

    /* 1. 获取姿态角 */
    float roll  = Roll();
    float pitch = Pitch();
    float yaw   = Yaw();
    ins.yaw = yaw;

    float roll_rad  = roll  * (float)(M_PI / 180.0);
    float pitch_rad = pitch * (float)(M_PI / 180.0);
    float yaw_rad   = yaw   * (float)(M_PI / 180.0);

    /* 2. 获取机体坐标系加速度 */
    float ax_body = AccelX();
    float ay_body = AccelY();

    /* 3. 重力补偿：从加速度中减去重力在机体轴上的投影
     *    gx = -g * sin(pitch)
     *    gy =  g * cos(pitch) * sin(roll)
     */
    float cos_r = cosf(roll_rad);
    float sin_r = sinf(roll_rad);
    float cos_p = cosf(pitch_rad);
    float sin_p = sinf(pitch_rad);

    float gx = -GRAVITY * sin_p;
    float gy =  GRAVITY * cos_p * sin_r;

    float ax_linear = ax_body - gx;   // X 轴线性加速度
    float ay_linear = ay_body - gy;   // Y 轴线性加速度

    /* 4. 死区滤波：微小加速度置零，防止静止时积分漂移 */
    if (fabsf(ax_linear) < ACCEL_THRESHOLD) ax_linear = 0.0f;
    if (fabsf(ay_linear) < ACCEL_THRESHOLD) ay_linear = 0.0f;

    /* 5. 机体坐标系 → 世界坐标系（绕 Z 轴旋转 yaw 角） */
    float cos_y = cosf(yaw_rad);
    float sin_y = sinf(yaw_rad);
    float ax_world = ax_linear * cos_y - ay_linear * sin_y;
    float ay_world = ax_linear * sin_y + ay_linear * cos_y;

    /* 6. 一次积分：加速度 → 速度 */
    ins.vx += ax_world * dt_s;
    ins.vy += ay_world * dt_s;

    /* 7. 速度衰减：每步轻微衰减，进一步抑制漂移 */
    ins.vx *= VEL_DECAY;
    ins.vy *= VEL_DECAY;

    /* 8. 二次积分：速度 → 位移 */
    ins.x += ins.vx * dt_s;
    ins.y += ins.vy * dt_s;
}

/**
 * @brief  位置/速度归零（保留 yaw）
 */
void INS_Reset(void)
{
    ins.x  = 0.0f;
    ins.y  = 0.0f;
    ins.vx = 0.0f;
    ins.vy = 0.0f;
}

/**
 * @brief  设置当前世界坐标
 */
void INS_SetPosition(float x, float y)
{
    ins.x = x;
    ins.y = y;
}

/**
 * @brief  返回距原点的直线距离 (m)
 */
float INS_Distance(void)
{
    return sqrtf(ins.x * ins.x + ins.y * ins.y);
}

/**
 * @brief  返回当前航向角 (弧度)
 */
float INS_HeadingRad(void)
{
    return ins.yaw * (float)(M_PI / 180.0);
}

/* ==================================================================
 *                      运动原语（阻塞式）
 * ================================================================== */

/**
 * @brief  将角度归一化到 [-180, 180] 范围
 */
static float norm_yaw(float yaw)
{
    while (yaw >  180.0f) yaw -= 360.0f;
    while (yaw < -180.0f) yaw += 360.0f;
    return yaw;
}

/**
 * @brief  走直线
 * @param  distance_m  距离 (m)，正=前进，负=后退
 * @param  speed_rpm   目标速度 (RPM)
 *
 *          两轮等速运行，航向 PI 修正抵抗偏航。
 *          用轮子周长预估到达时间，2 倍超时保护。
 */
void INS_GoStraight(float distance_m, float speed_rpm)
{
    if (fabsf(speed_rpm) < 1.0f) return;    // 速度为零直接返回

    float start_yaw = Yaw();                // 锁定的目标航向
    float base_rpm  = fabsf(speed_rpm);
    if (distance_m < 0) base_rpm = -base_rpm;  // 反向

    Lmotor_run(base_rpm);
    Rmotor_run(base_rpm);

    float yaw_err_sum = 0.0f;               // 航向积分
    float elapsed     = 0.0f;

    /* 按轮子线速度预估行驶时间 */
    float linear_speed = (base_rpm / 60.0f) * WHEEL_CIRCUMFERENCE;  // m/s
    float travel_time  = fabsf(distance_m / linear_speed);
    float timeout      = travel_time * 2.0f;  // 1 倍余量防止滑移

    while (elapsed < travel_time && elapsed < timeout) {
        Motor_pid_step();                    // 电机速度闭环
        INS_Update(0.005f);                  // 导航更新
        delay_ms(5);
        elapsed += 0.005f;

        /* 航向 PI 修正：err*Kp + ∫err*Ki */
        float cur_yaw = Yaw();
        float err     = norm_yaw(cur_yaw - start_yaw);
        yaw_err_sum  += err * 0.005f;
        float correction = err * 15.0f + yaw_err_sum * 2.0f;

        Lmotor_run(base_rpm - correction);
        Rmotor_run(base_rpm + correction);
    }

    Lmotor_stop();
    Rmotor_stop();
}

/**
 * @brief  走圆弧
 * @param  radius_m   圆弧半径 (m)，正=左转/逆时针，负=右转/顺时针
 *                   传 0 为原地旋转
 * @param  angle_deg  圆弧角度 (deg)
 * @param  speed_rpm  基础速度 (RPM)
 *
 *          差速公式：V_L/R = base * (R ± W/2) / R
 *          最快轮限幅 3 倍 base，累积 yaw 增量判断到达。
 */
void INS_GoArc(float radius_m, float angle_deg, float speed_rpm)
{
    float base_rpm   = fabsf(speed_rpm);
    float target_deg = angle_deg;
    float VL, VR;

    /* 差速计算 */
    if (fabsf(radius_m) < 0.001f) {
        /* 原地旋转：两轮反向等速 */
        float sign = (target_deg > 0.0f) ? 1.0f : -1.0f;
        VL = -base_rpm * sign;
        VR =  base_rpm * sign;
    } else {
        /* 圆弧行驶：VL/R = base * (R ± W/2) / R */
        VL = base_rpm * (radius_m - WHEEL_TRACK / 2.0f) / radius_m;
        VR = base_rpm * (radius_m + WHEEL_TRACK / 2.0f) / radius_m;

        /* 限幅：防止小半径时差速过大 */
        float limit = base_rpm * 3.0f;
        if (VL >  limit) VL =  limit;
        if (VL < -limit) VL = -limit;
        if (VR >  limit) VR =  limit;
        if (VR < -limit) VR = -limit;
    }

    Lmotor_run(VL);
    Rmotor_run(VR);

    /* 逐帧累积 yaw 变化量，自动处理 ±180° 回绕 */
    float yaw_sum  = 0.0f;
    float last_yaw = Yaw();

    while (fabsf(yaw_sum) < fabsf(target_deg)) {
        Motor_pid_step();
        INS_Update(0.005f);
        delay_ms(5);

        float cur_yaw = Yaw();
        float dyaw    = norm_yaw(cur_yaw - last_yaw);   // 单步增量
        yaw_sum      += dyaw;                           // 累计转角
        last_yaw      = cur_yaw;
    }

    Lmotor_stop();
    Rmotor_stop();
}
