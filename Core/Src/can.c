#include "main.h"
#include "can.h"
#include "uart.h"
#include "conversor.h"
#include <string.h>

void init_configs(void)
{
	canfil.IdType       = FDCAN_EXTENDED_ID;
	canfil.FilterIndex  = 0;
	canfil.FilterType   = FDCAN_FILTER_MASK;
	canfil.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	canfil.FilterID1    = 0x00000000;   /* ID   */
	canfil.FilterID2    = 0x00000000;   /* MASK: 0 = aceita qualquer ID */

	if (HAL_FDCAN_ConfigFilter(&hfdcan2, &canfil) != HAL_OK) Error_Handler();

	/* Frames que não casarem (e qualquer standard/remote) vão para a FIFO0 em vez de serem descartados */
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
	                                 FDCAN_ACCEPT_IN_RX_FIFO0,   /* standard não casado */
	                                 FDCAN_ACCEPT_IN_RX_FIFO0,   /* extended não casado */
	                                 FDCAN_FILTER_REMOTE,
	                                 FDCAN_FILTER_REMOTE);
  if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_MESSAGE_LOST, 0) != HAL_OK) Error_Handler();
  HAL_FDCAN_Start(&hfdcan2);

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
  int standard = standard_voltage * SCALE_VOLTAGE;
  writeparam_send_msg[0] = WRITEPARAM_FEEDBACK_REQ;
  writeparam_send_msg[1] = WRITEPARAM_STD_VOLTAGE_ID;
  writeparam_send_msg[2] = 0;
  writeparam_send_msg[3] = standard & 0xFF;
  writeparam_send_msg[4] = (standard >> 8) & 0xFF;
}

void send_writeparam()
{
  can_send(D_WRITE_PARAM_FB1, writeparam_send_msg, CAN_WRITEPARAM_LEN);
  can_send(D_WRITE_PARAM_FB2, writeparam_send_msg, CAN_WRITEPARAM_LEN);
}

void create_status_send(float current_limit, float measured_output, float voltage_set, float ovp_limit)
{
  int current = current_limit * SCALE_CURRENT;
  int measured = measured_output * SCALE_VOLTAGE;
  int voltage = voltage_set * SCALE_VOLTAGE;
  int ovp = ovp_limit * SCALE_VOLTAGE;

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
  output_current1 = (msg1[1] + (msg1[2] << 8)) / SCALE_CURRENT;
  output_voltage1 = (msg1[3] + (msg1[4] << 8)) / SCALE_VOLTAGE;
  input_voltage1 = msg1[5] + (msg1[6] << 8);
  measured_output1 = output_voltage1;
}

void process_second(void)
{
  output_current2 = (msg2[1] + (msg2[2] << 8)) / SCALE_CURRENT;
  output_voltage2 = (msg2[3] + (msg2[4] << 8)) / SCALE_VOLTAGE;
  input_voltage2 = msg2[5] + (msg2[6] << 8);
  measured_output2 = output_voltage2;
}

void process_messages(void)
{
  uint8_t snap1[8], snap2[8];
  int new1, new2;

  /* Copia atomica: a ISR do FDCAN pode sobrescrever msg1/msg2 a qualquer momento */
  __disable_irq();
  new1 = firstflag;  firstflag  = 0;
  new2 = secondflag; secondflag = 0;
  memcpy(snap1, msg1, 8);
  memcpy(snap2, msg2, 8);
  __enable_irq();

  if (new1)
  {
    memcpy(msg1, snap1, 8);   /* process_first() le msg1 */
    process_first();
  }

  if (new2)
  {
    memcpy(msg2, snap2, 8);
    process_second();
  }

  if (modules_disabled & MODULE1_BIT) output_current1 = 0;
  if (modules_disabled & MODULE2_BIT) output_current2 = 0;

  output_current = output_current1 + output_current2;
  output_voltage = (output_voltage1 + output_voltage2) / 2;
  actual_power = output_current * output_voltage;
}

/* ------------------------------------------------------------------ */
/* Recepcao: a ISR so esvazia a FIFO do hardware e enfileira os frames; */
/* o tratamento (can_process_rx) roda no loop principal                 */
/* ------------------------------------------------------------------ */
typedef struct
{
  uint32_t id;
  uint8_t  data[8];
} can_rx_frame_t;

static can_rx_frame_t rxq[RXQ_SIZE];
static volatile uint32_t rxq_head = 0;   /* escrito so na ISR   */
static volatile uint32_t rxq_tail = 0;   /* escrito so no loop  */

volatile uint32_t rx_count   = 0;        /* frames lidos da FIFO do hardware */
volatile uint32_t rx_lost    = 0;        /* FIFO do hardware estourou        */
volatile uint32_t rx_dropped = 0;        /* fila de software cheia           */

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  /* static: nao consome pilha dentro da ISR (a pilha do projeto e de so 1 KB) */
  static FDCAN_RxHeaderTypeDef hdr;
  static uint8_t d[64];

  if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) rx_lost++;

  if (!(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) return;

  /* Esvazia TUDO: pode haver mais de um frame pendente por interrupcao */
  while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0U)
  {
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &hdr, d) != HAL_OK) break;
    rx_count++;

    if (hdr.IdType != FDCAN_EXTENDED_ID) continue;

    uint32_t next = (rxq_head + 1U) % RXQ_SIZE;

    rxq[rxq_head].id = hdr.Identifier;
    memcpy(rxq[rxq_head].data, d, 8);
    __DMB();
    rxq_head = next;
  }
}

/* ------------------------------------------------------------------ */
/* Transmissao                                                         */
/* ------------------------------------------------------------------ */
HAL_StatusTypeDef can_send(uint32_t id, const uint8_t *data, uint32_t len)
{
  if (len > CAN_DATA_LEN) len = CAN_DATA_LEN;

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
    memcpy(data, serialnumber1, CAN_SERIAL_LEN);
    if (can_send(C_LOG_ID1, data, CAN_DATA_LEN) == HAL_OK) { loggedin1 = 1; loginrequest1 = 0; println("Modulo 1: login enviado"); }
  }
  else
  {
    memcpy(data, serialnumber2, CAN_SERIAL_LEN);
    if (can_send(C_LOG_ID2, data, CAN_DATA_LEN) == HAL_OK) { loggedin2 = 1; loginrequest2 = 0; println("Modulo 2: login enviado"); }
  }
}

/* Status/setpoints enviados em broadcast (ID unico C_STATUS) */
void send_status(void)
{
  can_send(C_STATUS, status_send_msg, CAN_DATA_LEN);
}

/* Recuperacao de bus-off (AutoBusOff nao existe no HAL do H5): Stop + Start reinicia a sequencia de recuperacao */
void can_check_bus(void)
{
  FDCAN_ProtocolStatusTypeDef ps;

  if (HAL_FDCAN_GetProtocolStatus(&hfdcan2, &ps) == HAL_OK && ps.BusOff)
  {
    println("CAN bus-off: reiniciando FDCAN");
    HAL_FDCAN_Stop(&hfdcan2);
    HAL_FDCAN_Start(&hfdcan2);
  }
}
