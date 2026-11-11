#include "main.h"
#include "uart.h"
#include "context.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;

/* Forca o newlib-nano a incluir o suporte a float no printf (%f), sem mexer no linker */
__asm__(".global _printf_float");

void print(const char *format, ...) {
    va_list args;
    char output[UART_BUF_SIZE];

    va_start(args, format);
    vsnprintf(output, sizeof(output), format, args);
    va_end(args);

    HAL_UART_Transmit(&huart2, (uint8_t *)output, strlen(output), UART_TX_TIMEOUT_MS);
}

void println(const char *format, ...) {
    va_list args;          // declara a lista de argumentos variáveis
    char output[UART_BUF_SIZE];      // buffer que vai guardar a string montada
    va_start(args, format); // inicializa a lista, começando após 'format'
    vsnprintf(output, sizeof(output), format, args); // monta a string formatada em output
    va_end(args);          // limpa a lista de argumentos
    HAL_UART_Transmit(&huart2, (uint8_t *)output, strlen(output), UART_TX_TIMEOUT_MS); // envia a string
    HAL_UART_Transmit(&huart2, (uint8_t *)"\n", 1, UART_TX_TIMEOUT_MS); // envia a nova linha
}