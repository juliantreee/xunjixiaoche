#include "ti_msp_dl_config.h"
#include "Motor_pid.h"
#include "Graysensor.h"
#include "track.h"
#include "delay.h"

PID *track;

double stop_distance = 0.59; //识别到路口后继续开的距离，单位为圈数

void track_Init(void)
{
    Motor_pid_Init();
}

signed char track_err(void)
{
    Gray_read_all();
    if(gray_value[3] == true)
    {
        if(gray_value[4] == true)
        {
            return 0;
        }
        else if(gray_value[2] == true)
        {
            return 3;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(gray_value[4] == true)
        {
            if(gray_value[5] == true)
            {
                return -3;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            if(gray_value[5] == true)
            {
                return -4;
            }
            else
            {
                return 4;
            }
        }
    }
}

void to_next_cross(double speed)
{
    pid_clear(track);
    Gray_read_all();
    while(1)
    {
        if(gray_value[0] == true)
        {
            if(gray_value[7] == true) //十字路口
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
                break;
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
                Lmotor_run(-100);
                Rmotor_run(100);
                while(gray_value[0] == false) //转向
                {
                    Motor_pid_step();
                    Gray_read_all();
                    delay_ms(5);
                }
                while(gray_value[4] == false) //转向
                {
                    Motor_pid_step();
                    Gray_read_all();
                    delay_ms(5);
                }
                Lmotor_brake();
                Rmotor_brake();
                break;
            }
        }
        else if(gray_value[7] == true) //右路口
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
            Lmotor_run(100);
            Rmotor_run(-100);
            for(int j=0; j <= 63; j++) //转向
            {
                Motor_pid_step();
                Gray_read_all();
                delay_ms(5);
            }
            Lmotor_brake();
            Rmotor_brake();
            break;
        }
        else //循线
        {
            Gray_read_all();
            double err = track_err();
            Lmotor_run(speed - err * 5);
            Rmotor_run(speed + err * 5);
            Motor_pid_step();
            delay_ms(5);
        }
    }
}