#include "menu.h"
#include "oled.h"

/* ========== 菜单项标签（可自行修改） ========== */
static const char* menu_labels[MENU_ITEMS] = {
    "option1",
    "option2",
    "option3",
    "option4"
};

static uint8_t menu_index = 0;

/* ========== 按键检测（消抖 + 单次触发） ========== */
typedef enum {
    KEY1_IDLE,
    KEY1_PRESSED,
    KEY1_RELEASED,
    KEY2_IDLE,
    KEY2_PRESSED,
    KEY2_RELEASED
} KeyState;

static KeyState key1_state = KEY1_IDLE;
static KeyState key2_state = KEY2_IDLE;

static int getnum(void)
{
    static uint8_t debounce_counter1 = 0;
    static uint8_t debounce_counter2 = 0;
    const uint8_t DEBOUNCE_THRESH = 5;

    // KEY1
    if (DL_GPIO_readPins(KEY1_PORT, KEY1_PIN_23_PIN) > 0) {
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

    // KEY2
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

/* ========== 菜单显示 ========== */
void Menu_Show(void)
{
    OLED_Clear();

    // 绘制菜单项: size=16, 行高16px, 左缩进16px 给 ">" 留空
    for (uint8_t i = 0; i < MENU_ITEMS; i++) {
        uint8_t y = i * 16;
        OLED_ShowString(16, y, (uint8_t*)menu_labels[i], 16);
    }

    // 当前选中行 — ">" 指示器
    uint8_t y = menu_index * 16;
    OLED_ShowString(0, y, (uint8_t*)">", 16);

    // 整行反色高亮
    OLED_AreaInvert(0, y, 127, y + 15);
    OLED_Refresh();
}

/* ========== 菜单初始化 ========== */
void Menu_Init(void)
{
    menu_index = 0;
    Menu_Show();
}

/* ========== 菜单轮询 — 在主循环中调用 ========== */
uint8_t Menu_Update(void)
{
    int key = getnum();
    if (key == 0) return 0;

    if (key == 1) {  // KEY1: 下翻
        menu_index = (menu_index + 1) % MENU_ITEMS;
        Menu_Show();
        return 0;
    }

    if (key == 2) {  // KEY2: 确认
        return menu_index + 1;  // 1-based 便于 if (ret) 判断
    }

    return 0;
}
