#ifndef UART_H_
#define UART_H_

/* printf-like via USART2. Para %f funcionar com newlib-nano, habilitar
   "Use float with printf" (-u _printf_float) no linker. */
void print(const char *format, ...)   __attribute__((format(printf, 1, 2)));

void println(const char *format, ...) __attribute__((format(printf, 1, 2)));

#endif /* UART_H_ */
