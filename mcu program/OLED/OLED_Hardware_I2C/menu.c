#include "menu.h"
#include "oled_menu.h"

uint8_t keynum=0;
uint8_t t=0;
uint8_t x=0,y=0,z=0;
uint8_t x2=0,y2=0,z2=0;
uint8_t x3=0,y3=0,z3=0;
uint8_t x4=0,x44=0,y4=0,z4=0;
uint8_t p=0,i=0,d=0;

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
}

int menu0(void){
    uint8_t flag=0;
    uint8_t flag1=0,flag2=0,flag3=0,flag4=0;
    OLED_Clear();
    OLED_ShowChinese(0, 0, 11, 16);
    OLED_ShowChinese(18, 0, 13, 16);
    OLED_ShowChinese(36, 0, 12, 16);

    OLED_ShowChinese(0, 2, 11, 16);
    OLED_ShowChinese(0, 2, 14, 16);
    OLED_ShowChinese(0, 2, 11, 16);

    OLED_ShowChinese(0, 4, 11, 16);
    OLED_ShowChinese(0, 4, 15, 16);
    OLED_ShowChinese(0, 4, 11, 16);

    OLED_ShowChinese(0, 6, 11, 16);
    OLED_ShowChinese(0, 6, 16, 16);
    OLED_ShowChinese(0, 6, 11, 16);
     // OLED_Refresh(54, 2, 74, 2); // 假设每行菜单占16像素，即2页


/*
    while(1) {
        OLED_ShowNum(54,6,flag,1,16);
        keynum = getnum();
        if(keynum==1){//这个用来切换菜单
            flag++;
            if(flag==5) flag=1;
        } 

        if(keynum==2){//这个用来确认
            OLED_Clear();
            return flag;//flag回传给main函数中的next以此进入下一个菜单或直接执行函数
        }
        switch(flag){
            case 1:
            {
                if(flag1==0){
                    if(t==1){OLED_ColorTurn(75, 48, 53, 16);t=0;}//这个t等于1是因为第四行亮了之后,t置1表示第四行亮
                    OLED_ColorTurn(75, 0, 53, 16); // 高亮第1行区域
                    flag1=1;flag4=0;//flag1,2,3,4标志位在进入下一个时候置一同时之前的置0
                }
                break;
            }

            case 2:
            {
                if(flag2==0){
                    OLED_ColorTurn(75, 0, 53, 16);  //关闭上一行高亮
                    OLED_ColorTurn(75, 16, 53, 16); // 高亮第2行区域
                    flag2=1;flag1=0;
                }
                break;
            }

            case 3:
            {
                if(flag3==0){
                    OLED_ColorTurn(75, 16, 53, 16);
                    OLED_ColorTurn(75, 32, 53, 16); // 高亮第3行区域
                    flag3=1;flag2=0;
                }
                break;
            }

            case 4:
            {
                if(flag4==0){
                    t=1;
                    OLED_ColorTurn(75, 32, 53, 16);
                    OLED_ColorTurn(75, 48, 53, 16); // 高亮第4行区域
                    flag4=1;flag3=0;
                }
                break;
            }
        }
        
    }
}
*/