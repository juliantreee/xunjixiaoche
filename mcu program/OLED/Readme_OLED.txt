 int next;
    SYSCFG_DL_init();
    SysTick_Init();
    OLED_Init();
    //初始化时钟
    while (1) {
        next = menu0();
    }
    主函数放这个,其他看注释