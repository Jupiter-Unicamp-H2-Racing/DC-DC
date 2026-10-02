#include "main.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;

void print(const char *format, ...) {
    va_list args;
    char output[200];

    va_start(args, format);
    vsnprintf(output, sizeof(output), format, args);
    va_end(args);

    HAL_UART_Transmit(&huart2, (uint8_t *)output, strlen(output), 100);
}

void println(const char *format, ...) {
    va_list args;          // declara a lista de argumentos variáveis
    char output[200];      // buffer que vai guardar a string montada
    va_start(args, format); // inicializa a lista, começando após 'format'
    vsnprintf(output, sizeof(output), format, args); // monta a string formatada em output
    va_end(args);          // limpa a lista de argumentos
    HAL_UART_Transmit(&huart2, (uint8_t *)output, strlen(output), 100); // envia a string
    HAL_UART_Transmit(&huart2, (uint8_t *)"\n", 1, 100); // envia a nova linha
}