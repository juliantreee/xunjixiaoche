    /*
    * Copyright (c) 2021, Texas Instruments Incorporated
    * All rights reserved.
    *
    * Redistribution and use in source and binary forms, with or without
    * modification, are permitted provided that the following conditions
    * are met:
    *
    * *  Redistributions of source code must retain the above copyright
    *    notice, this list of conditions and the following disclaimer.
    *
    * *  Redistributions in binary form must reproduce the above copyright
    *    notice, this list of conditions and the following disclaimer in the
    *    documentation and/or other materials provided with the distribution.
    *
    * *  Neither the name of Texas Instruments Incorporated nor the names of
    *    its contributors may be used to endorse or promote products derived
    *    from this software without specific prior written permission.
    *
    * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
    * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
    * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
    * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    */

    #include "ti_msp_dl_config.h"
    #include "Motor.h"
    #include "delay.h"
    #include "Graysensor.h"
    #include "encoder.h"
    #include "oled.h"
    #include "Motor_pid.h"
    #include <ti/driverlib/m0p/dl_interrupt.h>

    #include <stdio.h>
    #include <string.h>
    #include "track.h"
    #include "board.h"
    #include "INS.h"



    int main(void)
    {
<<<<<<< HEAD
        DL_GPIO_clearPins(To_luban_PORT, To_luban_Tx_PIN);
        SYSCFG_DL_init();
=======
        board_init();
>>>>>>> c66ff236aa8351f8af5ccbe202c96e58a8be93e3
        Encoder_Init();
        track_Init();
<<<<<<< HEAD
        int turns = 0;
        if(DL_GPIO_readPins(Control_PORT, Control_Sel_PIN) == 0)
        {
            delay_ms(500);
            while (DL_GPIO_readPins(Control_PORT, Control_Sel_PIN) == 0) 
            {
                if(DL_GPIO_readPins(Control_PORT, Control_Tick_PIN) == 0)
                {
                    turns += 1;
                    delay_ms(500);
                }
            }
            for(int i = 1; i <= turns ; i++)
            {
                to_next_cross(150);
                to_next_cross(150);
                to_next_cross(150);
                to_next_cross(150);
            }
        }
        else
        {
        while(1)
        {
            if(DL_GPIO_readPins(To_luban_PORT, To_luban_Rx_PIN) == 0)
            {
                break;
            }
        }
        to_next_cross(150);
        while(1)
        {
            if(DL_GPIO_readPins(To_luban_PORT, To_luban_Rx_PIN) == 0)
            {
                break;
            }
        }
        to_next_cross(150);
        while(1)
        {
            if(DL_GPIO_readPins(To_luban_PORT, To_luban_Rx_PIN) == 0)
            {
                break;
            }
        }
        to_next_cross(150);
        while(1)
        {
            if(DL_GPIO_readPins(To_luban_PORT, To_luban_Rx_PIN) == 0)
            {
                break;
            }
        }
        to_next_cross(150);
=======
        INS_Init();

        while (1)
        {
            /* ================================================================ */
            /* 1. 陀螺仪数据读取                                                 */
            /* ================================================================ */
            float gyro_x = stcGyro.wx;   // 角速度 X (°/s)
            float gyro_y = stcGyro.wy;   // 角速度 Y (°/s)
            float gyro_z = stcGyro.wz;   // 角速度 Z (°/s)

            float accel_x = stcAccel.ax; // 加速度 X (m/s²)
            float accel_y = stcAccel.ay; // 加速度 Y (m/s²)
            float accel_z = stcAccel.az; // 加速度 Z (m/s²)

            float roll  = stcAngle.Roll;  // 横滚角 (°)
            float pitch = stcAngle.Pitch; // 俯仰角 (°)
            float yaw   = stcAngle.Yaw;   // 偏航角 (°)

            float q0 = stcQuat.q0; // 四元数 Q0 (实部)
            float q1 = stcQuat.q1; // 四元数 Q1 (i分量)
            float q2 = stcQuat.q2; // 四元数 Q2 (j分量)
            float q3 = stcQuat.q3; // 四元数 Q3 (k分量)

            /* ================================================================ */
            /* 2. INS 惯性导航更新                                               */
            /* ================================================================ */
            // INS_Update(0.01f);   // 周期更新 (dt=10ms)

            /* ================================================================ */
            /* 3. 灰度循迹                                                      */
            /* ================================================================ */
            // to_next_cross(200);  // 循迹到下一个路口
            // track_err();         // 获取循迹偏差

            /* ================================================================ */
            /* 4. INS 运动控制                                                   */
            /* ================================================================ */
            // INS_GoStraight(1.0f, 200);         // 直行 1m, 200rpm
            // INS_GoArc(0.5f, 90.0f, 200);       // 左转 0.5m半径 90°
            // INS_GoArc(0, 90.0f, 200);          // 原地右转 90° (radius=0)
            // INS_GoArc(-0.5f, 90.0f, 200);      // 右转 0.5m半径 90°

            /* ================================================================ */
            /* 5. 串口发送 (UART_1_INST → printf调试输出)                        */
            /* ================================================================ */
            // printf("Yaw:%.2f Pitch:%.2f Roll:%.2f\r\n", yaw, pitch, roll);

            /* ================================================================ */
            /* 6. 串口发送 (UART_0_INST → 陀螺仪指令)                            */
            /* ================================================================ */
            // uart0_send_SendByte(cmd_data, len); // 向陀螺仪发送指令

            delay_ms(10);
>>>>>>> c66ff236aa8351f8af5ccbe202c96e58a8be93e3
        }
    }

