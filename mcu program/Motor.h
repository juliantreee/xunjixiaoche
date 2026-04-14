#ifndef MOTOR_H_
#define MOTOR_H_

void MotorL_forward();
void MotorL_backward();
void MotorL_stop();
void MotorL_brake();
void MotorL_speed(double speed);  //from 0 to 100

void MotorR_forward();
void MotorR_backward();
void MotorR_stop();
void MotorR_brake();
void MotorR_speed(double speed);  //from 0 to 100

void Go_forward();
void Go_backward();
void stopnow();
void hitbrake();
void cospeed(double speed);

#endif