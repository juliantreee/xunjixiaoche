#include "menu.h"
#include "oled.h"
extern int flag;
typedef enum {
    KEY1_IDLE,     // 空闲状态
    KEY1_PRESSED,  // 按键按下（已消抖）
    KEY1_RELEASED,  // 按键释放（已消抖）
    KEY2_IDLE,     
    KEY2_PRESSED,  
    KEY2_RELEASED  
} KeyState;

KeyState key1_state = KEY1_IDLE;
KeyState key2_state = KEY2_IDLE;

int getnum() {
    static uint8_t debounce_counter1 = 0;
    static uint8_t debounce_counter2 = 0;
    const uint8_t DEBOUNCE_THRESH = 5;  // 5次采样（假设1ms扫描周期）

    // 检测 KEY1
    if (DL_GPIO_readPins(KEY1_PORT, KEY1_PIN_23_PIN) > 0) {  // 低电平有效
        if (key1_state == KEY1_IDLE) {
            key1_state = KEY1_PRESSED;
        } else if (key1_state == KEY1_PRESSED) {
            if (++debounce_counter1 >= DEBOUNCE_THRESH) {
                key1_state = KEY1_RELEASED;
                return 1;
            }
        }
    } else {
        key1_state = KEY1_IDLE;
        debounce_counter1 = 0;
    }

    // 检测 KEY2（逻辑类似）
    if (DL_GPIO_readPins(KEY2_PORT, KEY2_PIN_26_PIN) > 0) {
        if (key2_state == KEY2_IDLE) {
            key2_state = KEY2_PRESSED;
        } else if (key2_state == KEY2_PRESSED) {
            if (++debounce_counter2 >= DEBOUNCE_THRESH) {
                key2_state = KEY2_RELEASED;
                return 2;
            }
        }
    } else {
        key2_state = KEY2_IDLE;
        debounce_counter2 = 0;
    }

    return 0;
}//1为key1，2为key2，0为未检测到
