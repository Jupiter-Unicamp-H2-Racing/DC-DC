#ifndef CONVERSOR_H
#define CONVERSOR_H

#include "context.h"

void process_messages(void);
void create_status_send(float current_limit, float measured_output, float voltage_set, float ovp_limit);
void init_configs(void);
void create_writeparam_send(float standard_voltage);
float power_control(float voltage_set, int power);

void conversor_run(void);   /* chamar dentro do while(1) do main */

#endif
