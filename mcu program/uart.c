//
// Created by qianj on 2026/5/3.
//
//
//#include "ti_msp_dl_config.h"
//#include "string.h"
//#include "stdio.h"
//// fputc/fputs/puts 已移至 board.c，避免重复定义冲突
//#include "uart.h"
////多串口printf重定义（保留 Set_CurrentUART 供切换使用）
//#define UART0_IDX 0//这里改上想用的名字
//#define UART1_IDX 2//这里改上想用的名字
//#define UART2_IDX 3//这里改上想用的名字
//#define UART3_IDX 4//这里改上想用的名字
//
//static UART_Regs *current_uart;
//
//void Set_CurrentUART(uint8_t UART_type) {
//    switch (UART_type) {
//    case UART0_IDX:
//        current_uart = UART_0_INST;
//        break;
//    default:
//        break;
//    }
//}
//这部分的内容已经放弃作废
