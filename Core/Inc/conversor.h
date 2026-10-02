#ifndef CONVERSOR_H
#define CONVERSOR_H

#include "context.h"

#define C_LOG_ID1                   117458948UL
#define D_LOG_ID1                   117523456UL
#define C_LOG_ID2                   117458952UL
#define D_LOG_ID2                   117588992UL

#define D_NORMAL_STATUS1            117522436UL
#define D_NORMAL_STATUS2            117587972UL

#define D_DISABLE_STATUS1           117522448UL
#define D_DISABLE_STATUS2           117587984UL

#define C_STATUS                    134168580UL

#define D_WRITE_PARAM               117480450UL

#define P_CONST                     0.001f
#define I_CONST                     0.0003f
#define D_CONST                     -0.0001f

#define MAX_VOLTAGE                 52
#define MIN_VOLTAGE                 48

#define POWER_REQUEST_W             800

void process_messages(void);
void create_status_send(float current_limit, float measured_output, float voltage_set, float ovp_limit);
void init_configs(void);
void create_writeparam_send(float standard_voltage);
float power_control(float voltage_set, int power);

void conversor_run(void);   /* chamar dentro do while(1) do main */

#endif
