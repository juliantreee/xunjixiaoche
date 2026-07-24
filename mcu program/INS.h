#ifndef INS_H
#define INS_H

// ===== 机器人尺寸参数（需根据实际小车标定）=====
#define WHEEL_TRACK         0.15f   // 左右轮间距 (m)
#define WHEEL_CIRCUMFERENCE 0.20f   // 轮子周长 (m)

typedef struct {
    float x, y;       // 世界坐标系位置 (m)
    float vx, vy;     // 世界坐标系速度 (m/s)
    float yaw;        // 当前航向角 (deg)
    float dt;         // 上次更新的时间间隔 (s)
} INS_State;

extern INS_State ins;

// 惯性导航核心
void INS_Init(void);
void INS_Update(float dt_s);
void INS_Reset(void);
void INS_SetPosition(float x, float y);
float INS_Distance(void);
float INS_HeadingRad(void);

// 运动原语（阻塞式）
void INS_GoStraight(float distance_m, float speed_rpm);
void INS_GoArc(float radius_m, float angle_deg, float speed_rpm);

#endif
