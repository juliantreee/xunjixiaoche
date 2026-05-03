//
// Created by qianj on 2026/5/3.
//

#include "ti_msp_dl_config.h"
#include "ti_msp_dl.h"//这几个好像重复了但没事编译器会优化的
#include "ti_msp_uart.h"//这几个好像重复了但没事编译器会优化的
#include "string.h"
#include "stdio.h"
//以下为printf重定义
#include "uart.h"
int fputc(int c, FILE* stream) {
    DL_UART_Main_transmitDataBlocking(current_uart, c);
    return c;
}

int fputs(const char* restrict s, FILE* restrict stream) {
    uint16_t i, len;
    len = strlen(s);
    for (i = 0; i < len; i ++) {
        DL_UART_Main_transmitDataBlocking(current_uart, s[i]);
    }
    return len;
}

int puts(const char* _ptr) {
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout);
    return count;
}
//多串口printf重定义
#define UART0_IDX 1//这里改上想用的名字
#define UART1_IDX 2//这里改上想用的名字
#define UART2_IDX 3//这里改上想用的名字
#define UART3_IDX 4//这里改上想用的名字

static UART_Regs *current_uart;

void Set_CurrentUART(uint8_t UART_type) {
    switch (UART_type) {
    case UART0_IDX:
        current_uart = UART_0_INST;
        break;
    case UART1_IDX:
        current_uart = UART_1_INST;
        break;
    case UART2_IDX:
        current_uart = UART_2_INST;
        break;
    case UART3_IDX:
        current_uart = UART_3_INST;
        break;
    default:
        break;
    }
}
//