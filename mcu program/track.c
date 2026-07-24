#include "ti_msp_dl_config.h"
#include "Motor_pid.h"
#include "Motor.h"
#include "Graysensor.h"
#include "track.h"
#include "delay.h"
#include <stdio.h>

PID *track;

double stop_distance = 0.45; //识别到路口后继续开的距离，单位为圈数

void track_Init(void)
{
    Motor_pid_Init();
    track = pid_create(5.0,1.0,0.0,100,-100,0.005);
}

signed char track_err(void)
{
    Gray_read_all();
    switch(gray_value)
    {
        case 0x18:
        {
            return 0;
            break;
        }
        case 0x08:
        {
            return 1;
            break;
        }
        case 0x10:
        {
            return -1;
            break;
        }
        case 0x0C:
        {
            return 3;
            break;
        }
        case 0x30:
        {
            return -3;
            break;
        }
        case 0x04:
        {
            return 4;
            break;
        }
        case 0x20:
        {
            return -4;
            break;
        }
        default: //其他情况
        {
            return 100;
        }
    }
}

void to_next_cross(double speed)
{
    pid_clear(track);
    while(1)
    {
        signed char err = track_err();
        if(((gray_value >> 0) & 1) == 1)
        {
        delay_ms(2);
        if(((gray_value >> 0) & 1) != 1)
        {
            continue;
        }
            if(((gray_value >> 7) & 1) == 1) //十字路口
            {
                Lmotor_run(speed);
                Rmotor_run(speed);
                for(int i = 1; i <= (int)(12000*stop_distance/speed); i++)
                {
                    Motor_pid_step();
                    delay_ms(5);
                }
                Lmotor_brake();
                Rmotor_brake();
                return;
            }
            else //左路口
            {
                Lmotor_run(speed);
                Rmotor_run(speed);
                for(int i = 1; i <= (int)(12000*stop_distance/speed); i++)//继续前进到轮子在线上
                {
                    Motor_pid_step();
                    delay_ms(5);
                }
                Lmotor_brake();
                Rmotor_brake();
                delay_ms(300);
                Lmotor_run(-100);
                Rmotor_run(100);
                DL_GPIO_setPins(To_luban_PORT, To_luban_Tx_PIN);
                for(int j=0; j <= 68; j++) //转向
                {
                    Motor_pid_step();
                    delay_ms(5);
                }
                DL_GPIO_clearPins(To_luban_PORT, To_luban_Tx_PIN);
                Lmotor_brake();
                Rmotor_brake();
                delay_ms(500);
                return;
            }
        }
        else if(((gray_value >> 7) & 1) == 1) //右路口
        {
        delay_ms(2);
        if(((gray_value >> 7) & 1) != 1)
        {
            continue;
        }
            Lmotor_run(speed);
            Rmotor_run(speed);
            for(int i = 1; i <= (int)(12000*stop_distance/speed); i++)//继续前进到轮子在线上
            {
                Motor_pid_step();
                delay_ms(5);
            }
            Lmotor_brake();
            Rmotor_brake();
            delay_ms(300);
            Lmotor_run(100);
            Rmotor_run(-100);
            DL_GPIO_setPins(To_luban_PORT, To_luban_Tx_PIN);
            for(int j=0; j <= 64; j++)
            {
                Motor_pid_step();
                delay_ms(5);
            }
            DL_GPIO_clearPins(To_luban_PORT, To_luban_Tx_PIN);
            Lmotor_brake();
            Rmotor_brake();
            delay_ms(500);
            return;
        }
        else //循线
        {
            double ctl = pid_step(track, (double)err, 0);
            Lmotor_run(speed + ctl);
            Rmotor_run(speed - ctl);
            Motor_pid_step();
            delay_ms(5);
        }        
    }
}
