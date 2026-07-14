#ifndef MCU_PROGRAM_MENU_H
#define MCU_PROGRAM_MENU_H
#include <stdint.h>

#define MENU_ITEMS 4

void Menu_Init(void);
void Menu_Show(void);
uint8_t Menu_Update(void);  // 返回 1~MENU_ITEMS 表示选中, 0 表示无操作

#endif
