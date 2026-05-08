#ifndef GRAYSENSOR_H_
#define GRAYSENSOR_H_

extern bool gray_value[8];
extern uint8_t TrackN;
void select_channel(uint8_t channel);
bool Gray_read();
void Gray_read_all(); //save data to gray_value[]

#endif