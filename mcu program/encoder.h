#ifndef ENCODER_H_
#define ENCODER_H_

#define PPR 1000  //线数
#define Timerf 8000000.0  //计时器频率
#define PII 6.2831853     //2pi

static volatile uint32_t LgCapVal1;          // 第一个变化沿捕获值
static volatile uint32_t LgCapVal2;          // 第二个变化沿捕获值
static volatile bool     LgCapDone;          // 测量完成标志
static volatile bool     LgFirstEdge;        // 是否已捕获第一个边沿

static volatile uint32_t gLoadValue;        // 定时器重载值

static volatile uint32_t RgCapVal1;          // 第一个变化沿捕获值
static volatile uint32_t RgCapVal2;          // 第二个变化沿捕获值
static volatile bool     RgCapDone;          // 测量完成标志
static volatile bool     RgFirstEdge;        // 是否已捕获第一个边沿


static volatile uint32_t Lmotor_period; //周期，计数值
static volatile uint32_t Rmotor_period; //周期，计数值

static double Lmotor_angspeed; //弧度速度
static double Rmotor_angspeed; //弧度速度

void Encoder_Init(void); //初始化
void get_period(void);//获取周期并存储在Lmotor_period和Rmotor_period以及L/R Motor_angspeed中

#endif