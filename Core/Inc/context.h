#ifndef CONTEXT_H
#define CONTEXT_H

#include "main.h"
#include <stdint.h>

/* --- CAN: IDs estendidos ------------------------------------------- */

#define C_LOG_ID1                   0x07000F04UL
#define D_LOG_ID1                   0x07001F00UL
#define C_LOG_ID2                   0x07000F08UL
#define D_LOG_ID2                   0x07002F00UL
#define D_NORMAL_STATUS1            0x07001B04UL
#define D_NORMAL_STATUS2            0x07002B04UL
#define D_DISABLE_STATUS1           0x07001B10UL
#define D_DISABLE_STATUS2           0x07002B10UL
#define C_STATUS                    0x08000104UL
#define D_WRITE_PARAM_FB1           0x07019C00UL /* pedido/resposta modulo 1 */
#define D_WRITE_PARAM_FB2           0x07029C00UL /* pedido/resposta modulo 2 */

/* --- CAN: formato dos quadros -------------------------------------- */
#define CAN_DATA_LEN                8U      /* bytes de um quadro CAN classico */
#define CAN_SERIAL_LEN              6U      /* bytes do numero de serie        */
#define CAN_WRITEPARAM_LEN          5U      /* bytes do quadro D_WRITE_PARAM   */
#define WRITEPARAM_FEEDBACK_REQ     0x29    /* byte 0: feedback requested      */
#define WRITEPARAM_STD_VOLTAGE_ID   0x15    /* byte 1: parametro tensao padrao */

#define SCALE_CURRENT               10.0f   /* A * 10  nos quadros             */
#define SCALE_VOLTAGE               100.0f  /* V * 100 nos quadros             */

#define RXQ_SIZE                    32U     /* fila de recepcao (software)     */

/* --- Modulos (mascara de modules_disabled) -------------------------- */
#define MODULE1_BIT                 0x01
#define MODULE2_BIT                 0x02
#define MODULES_ALL                 (MODULE1_BIT | MODULE2_BIT)

/* --- UART ---------------------------------------------------------- */
#define UART_BUF_SIZE               200U
#define UART_TX_TIMEOUT_MS          100U

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
extern volatile uint32_t last_unknown_id; /* ultimo ID CAN nao reconhecido */

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
