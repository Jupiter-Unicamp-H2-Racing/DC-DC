#include "main.h"
#include "can.h"
#include "conversor.h"
#include <string.h>

void init_configs(void)
{
  canfil.IdType = FDCAN_EXTENDED_ID;
  canfil.FilterIndex = 0;
  canfil.FilterType = FDCAN_FILTER_MASK;
  canfil.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  canfil.FilterID1 = 0;
  canfil.FilterID2 = 0;

  if (HAL_FDCAN_ConfigFilter(&hfdcan2, &canfil) != HAL_OK) Error_Handler();

  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                   FDCAN_REJECT,
                                   FDCAN_REJECT,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK) Error_Handler();

  if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) Error_Handler();
  if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) Error_Handler();

  TxHeader.IdType = FDCAN_EXTENDED_ID;
  TxHeader.Identifier = C_LOG_ID1;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker = 0;
}

void create_writeparam_send(float standard_voltage)
{
  int standard = standard_voltage * 100;

  writeparam_send_msg[0] = 0x2B;
  writeparam_send_msg[1] = 0x15;
  writeparam_send_msg[2] = 0;
  writeparam_send_msg[3] = standard & 0xFF;
  writeparam_send_msg[4] = (standard >> 8) & 0xFF;
}

void create_status_send(float current_limit, float measured_output, float voltage_set, float ovp_limit)
{
  int current = current_limit * 10;
  int measured = measured_output * 100;
  int voltage = voltage_set * 100;
  int ovp = ovp_limit * 100;

  status_send_msg[0] = current & 0xFF;
  status_send_msg[1] = (current >> 8) & 0xFF;
  status_send_msg[2] = measured & 0xFF;
  status_send_msg[3] = (measured >> 8) & 0xFF;
  status_send_msg[4] = voltage & 0xFF;
  status_send_msg[5] = (voltage >> 8) & 0xFF;
  status_send_msg[6] = ovp & 0xFF;
  status_send_msg[7] = (ovp >> 8) & 0xFF;
}

void process_first(void)
{
  output_current1 = (msg1[1] + (msg1[2] << 8)) / 10.0f;
  output_voltage1 = (msg1[3] + (msg1[4] << 8)) / 100.0f;
  input_voltage1 = msg1[5] + (msg1[6] << 8);
  measured_output1 = output_voltage1;
}

void process_second(void)
{
  output_current2 = (msg2[1] + (msg2[2] << 8)) / 10.0f;
  output_voltage2 = (msg2[3] + (msg2[4] << 8)) / 100.0f;
  input_voltage2 = msg2[5] + (msg2[6] << 8);
  measured_output2 = output_voltage2;
}

void process_messages(void)
{
  if (firstflag)
  {
    firstflag = 0;
    process_first();
  }

  if (secondflag)
  {
    secondflag = 0;
    process_second();
  }

  output_current = output_current1 + output_current2;
  output_voltage = (output_voltage1 + output_voltage2) / 2;
  actual_power = output_current * output_voltage;
}

/* ------------------------------------------------------------------ */
/* Transmissao                                                         */
/* ------------------------------------------------------------------ */
HAL_StatusTypeDef can_send(uint32_t id, const uint8_t *data, uint32_t len)
{
  if (len > 8) len = 8;

  TxHeader.Identifier = id;
  TxHeader.DataLength = len;   /* no STM32H5, FDCAN_DLC_BYTES_n == n (0..8) */

  return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, (uint8_t *)data);
}

/* Responde ao pedido de login devolvendo o numero de serie recebido.
   TODO: confirmar o formato real da resposta de login. */
void send_login_ack(uint8_t module)
{
  uint8_t data[8] = {0};

  if (module == 1)
  {
    memcpy(data, serialnumber1, 6);
    if (can_send(C_LOG_ID1, data, 8) == HAL_OK) { loggedin1 = 1; loginrequest1 = 0; }
  }
  else
  {
    memcpy(data, serialnumber2, 6);
    if (can_send(C_LOG_ID2, data, 8) == HAL_OK) { loggedin2 = 1; loginrequest2 = 0; }
  }
}

/* Status/setpoints enviados em broadcast (ID unico C_STATUS) */
void send_status(void)
{
  can_send(C_STATUS, status_send_msg, 8);
}

/* Escrita do parametro (tensao padrao): 0x2B 0x15 0x00 lo hi */
void send_writeparam(void)
{
  can_send(D_WRITE_PARAM, writeparam_send_msg, 5);
}
