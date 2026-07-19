# xunjixiaoche MCU Program

## 硬件

- MCU: MSPM0G3507 (Cortex-M0+, 32MHz)
- IDE: CCS, tiarmclang 4.0.4.LTS
- SDK: mspm0_sdk_2_10_00_04

## UART 架构

| 功能 | 硬件 | 引脚 | 代码宏 | 波特率 | DMA |
|------|------|------|--------|--------|-----|
| printf | UART0 | PA11(RX)/PA10(TX) | `UART_1_INST` | 115200 | CH0/CH1 |

- printf: `board.c` 的 `fputc` → `UART_1_INST`

## 陀螺仪 I2C (硬件 I2C0)

- 硬件: I2C0, `I2C_Gyro_INST`, PA8=SDA / PA14=SCL, 400kHz Fast Mode
- 从机地址: `0x48`
- `Gyro_I2C_Write()` — 写数据 (无寄存器地址)，`Gyro_I2C_ReadReg()` — 读寄存器
- `Get_senserdata()` — 主动读取 4 个寄存器 (0xAA 角速度 / 0xBB 角度 / 0xCC 加速度 / 0xDD 四元数)
- 数据格式与 UART 帧一致：每寄存器 8 字节，每值 2 字节 short，/32768 缩放
- 校准命令 `sendCaliYawCommand()` / `performCaliBias()` → `Gyro_I2C_Write()`
- 指令数组: `Gyro_Key[3]`, `Gyro_Yaw_Zero[3]`, `Gyro_Save[3]`, `Gyro_BIAS_CAL[3]` (3 字节，无 0x55 0xAA 帧头)
- 陀螺仪数据通过 4 个全局结构体输出: `stcGyro` / `stcAngle` / `stcAccel` / `stcQuat`
- **注意**: SDK 的 I2C 控制器接收函数是 `DL_I2C_receiveControllerData()`，不是 `DL_I2C_readControllerRXFIFO()`

## 分支说明

- `main` — UART1 (PA18/PA17) 读取陀螺仪，DMA + CopeSerial2Data 帧解析
- `i2c-gyro` — 硬件 I2C0 (PA8/PA14) 读取陀螺仪，主动轮询 Get_senserdata()

## SysConfig

- 配置文件: `empty.syscfg`
- 生成文件: `Debug/ti_msp_dl_config.h` (不要手动编辑)
- **命名陷阱**: syscfg 变量名 ≠ 生成的宏名，`$name` 决定宏名，`peripheral.$assign` 决定硬件
- 修改 syscfg 后用 SysConfig GUI 保存重新生成
- 枚举值敏感，不要在 syscfg 里手写枚举，用 GUI dropdown

## 关键文件

- `empty.c` — main，6 区模板循环（I2C陀螺仪读取 / INS更新 / 循迹 / INS运动 / printf / I2C陀螺仪指令）
- `board.c/h` — UART printf 重定向、硬件 I2C 陀螺仪解析
- `uart.c/h` — 已废弃，`Set_CurrentUART()` 无效，不要 include
- `encoder.c` — 编码器测速，自行管理 LMotor(TIMG7) 中断
- `Motor.c` — 电机控制，PWM 用 TIMG0
- `menu.c/h` — OLED 菜单系统，两键操作（KEY1 翻项 / KEY2 确认）
- `INS.c/h` — 惯性导航模块
- `track.c/h` — 灰度循迹，`to_next_cross()` / `track_err()`

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

## 代码规范

- 抑制 unused parameter 警告用 `(void)x;`，不要用 `x = x;`
- 未使用的变量/结构体成员直接删除，不保留死代码
- 未实现的函数声明（头文件里的空声明）删掉

## OLED 菜单

- `menu.c/h` — 基于 0.96" OLED (128x64) 的两键菜单
- `Menu_Init()` — 初始化并显示菜单
- `Menu_Show()` — 重绘菜单（4 行 size=16，选中行 `>` + 反色高亮）
- `Menu_Update()` — 轮询按键，返回 1~MENU_ITEMS 表示选中，0 表示无操作
- KEY1 (PA23): 下翻，KEY2 (PA26): 确认
- 菜单标签在 `menu_labels[]` 数组中
