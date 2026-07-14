# xunjixiaoche MCU Program

## 硬件

- MCU: MSPM0G3507 (Cortex-M0+, 32MHz)
- IDE: CCS, tiarmclang 4.0.4.LTS
- SDK: mspm0_sdk_2_10_00_04

## UART 架构

| 功能 | 硬件 | 引脚 | 代码宏 | 波特率 | DMA |
|------|------|------|--------|--------|-----|
| 陀螺仪 | UART1 | PA18(RX)/PA17(TX) | `UART_0_INST` | 912600 | 无 |
| printf | UART0 | PA11(RX)/PA10(TX) | `UART_1_INST` | 115200 | CH0/CH1 |

- 陀螺仪 ISR: `UART_0_INST_IRQHandler` → `CopeSerial2Data()` 解析 0x5A 帧
- printf: `board.c` 的 `fputc` → `UART_1_INST`
- 校准命令 `sendCaliYawCommand` / `performCaliBias` 走 UART_0

## SysConfig

- 配置文件: `empty.syscfg`
- 生成文件: `Debug/ti_msp_dl_config.h` (不要手动编辑)
- **命名陷阱**: syscfg 变量名 ≠ 生成的宏名，`$name` 决定宏名，`peripheral.$assign` 决定硬件
- 修改 syscfg 后用 SysConfig GUI 保存重新生成
- 枚举值敏感，不要在 syscfg 里手写枚举，用 GUI dropdown

## 关键文件

- `empty.c` — main，调用 `board_init()`
- `board.c/h` — UART 收发、陀螺仪解析、printf 重定向
- `uart.c/h` — 旧 printf 模块，已废弃（fputc 移至 board.c）
- `encoder.c` — 编码器测速，自行管理 LMotor(TIMG7) 中断
- `Motor.c` — 电机控制，PWM 用 TIMG0
- `INS.c/h` — 惯性导航模块

## INS 惯性导航

### 核心功能
- `INS_Init()` / `INS_Update(dt_s)` — 初始化 + 周期更新（加速度积分 → 速度 → 位置）
- 全局状态 `ins` (INS_State): x, y, vx, vy, yaw, dt
- 重力补偿：用 Roll/Pitch 从机体加速度中减掉重力投影
- 死区滤波 (0.05 m/s²) + 速度衰减 (0.999) 抑制漂移

### 运动原语（阻塞式）
- `INS_GoStraight(distance_m, speed_rpm)` — 走直线，含航向 PI 修正
- `INS_GoArc(radius_m, angle_deg, speed_rpm)` — 走圆弧，半径正=左转，`radius=0`=原地旋转

### 关键注意
- 依赖 `Motor_pid.h`、`delay.h`、`<math.h>`（需 `-lm` 链接）
- `WHEEL_TRACK` / `WHEEL_CIRCUMFERENCE` 宏需按实车标定
- `board.h` 未声明的函数 (`AccelX/Y/Z`, `QuatQ0-3`) 在 INS.c 中 extern
- Cortex-M0+ 无 FPU，所有 float 运算为软件模拟，注意更新频率
- 圆弧: 差速公式 `VL/R = base*(R ± W/2)/R`，3倍限幅，累积 yaw 判断到达
