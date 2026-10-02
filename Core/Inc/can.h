#ifndef CAN_H_
#define CAN_H_

#include "context.h"
#include "conversor.h"

HAL_StatusTypeDef can_send(uint32_t id, const uint8_t *data, uint32_t len);
void send_login_ack(uint8_t module);   /* module = 1 ou 2 */
void send_status(void);                /* broadcast em C_STATUS */
void send_writeparam(void);            /* broadcast em D_WRITE_PARAM */

#endif /* CAN_H_ */
