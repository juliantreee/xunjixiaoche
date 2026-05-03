//
// Created by qianj on 2026/5/3.
//

#ifndef MCU_PROGRAM_UART_H
#define MCU_PROGRAM_UART_H
#include "string.h"
#include "stdio.h"
void Set_CurrentUART(uint8_t UART_type);
static UART_Regs *current_uart;
int puts(const char* _ptr);
int fputs(const char* restrict s, FILE* restrict stream);
int fputc(int c, FILE* stream);
#endif //MCU_PROGRAM_UART_H