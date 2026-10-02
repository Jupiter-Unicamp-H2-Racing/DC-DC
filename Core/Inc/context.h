#ifndef CONTEXT_H
#define CONTEXT_H

#include "main.h"
#include <stdint.h>

/* Handles / CAN */
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_FilterTypeDef canfil;
extern FDCAN_TxHeaderTypeDef TxHeader;
extern FDCAN_RxHeaderTypeDef RxHeader;

extern uint8_t RxData[8];
extern uint8_t msg1[8], msg2[8];
extern uint8_t status_send_msg[8];
extern uint8_t writeparam_send_msg[8];
extern uint8_t serialnumber1[8], serialnumber2[8];

/* Flags (alteradas em interrupcao) */
extern volatile int firstflag, secondflag;
extern volatile int loggedin1, loggedin2;
extern volatile int loginrequest1, loginrequest2;
extern volatile int modules_disabled;     /* bit0 = modulo 1, bit1 = modulo 2 */
extern volatile uint8_t tim2_flag;        /* 1 s   - debug UART        */
extern volatile uint8_t tim3_flag;        /* 100ms - malha de controle */
extern volatile uint8_t tim4_flag;        /* 500ms - parametro padrao  */

extern int contador;

/* Medidas */
extern int input_voltage1, input_voltage2;
extern float output_voltage1, output_voltage2;
extern float output_current1, output_current2;
extern float output_current, output_voltage;
extern float actual_power;
extern float measured_output1, measured_output2;

/* Parametros / controle */
extern float current_limit;
extern float voltage_set;
extern float ovp_limit;
extern float standard_voltage;
extern float LastITerm, LastPowerDiff;

#endif /* CONTEXT_H */
