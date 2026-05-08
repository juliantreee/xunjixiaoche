#ifndef _TRACK_H_
#define _TRACK_H_

#include "PID.h"

extern PID *track;
extern double stop_distance;
signed char track_err(void);
void to_next_cross(double speed);
void track_Init(void);

#endif
