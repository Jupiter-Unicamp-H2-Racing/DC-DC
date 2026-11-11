#include "main.h"
#include "conversor.h"
#include "context.h"
#include "can.h"
#include "uart.h"

int D = 1;

void conversor_run(void)
{
  /* Login dos modulos: responde com o numero de serie recebido */
  if (loginrequest1) send_login_ack(1);
  if (loginrequest2) send_login_ack(2);

  /* Malha de controle (100 ms, TIM3) */
  if (tim3_flag)
  {
    tim3_flag = 0;

    process_messages();

    if (modules_disabled == MODULES_ALL)
    {
      voltage_set = 30;
      LastITerm = 0;
      LastPowerDiff = 0;
    }

    /* Setpoints em broadcast (C_STATUS), um quadro por modulo logado */
    if (loggedin1)
    {
      create_status_send(current_limit, measured_output1, voltage_set, ovp_limit);
      send_status();
    }
    if (loggedin2)
    {
      create_status_send(current_limit, measured_output2, voltage_set, ovp_limit);
      send_status();
    }
  }

  /* Parametro padrao (500 ms, TIM4) */
  if (tim4_flag)
  {
    tim4_flag = 0;
    contador++;

    if ((loggedin1 && loggedin2) && D == 1)
    {
    	D = 0;
      create_writeparam_send(standard_voltage);
      send_writeparam();          /* reenvia a cada 500 ms até os dois confirmarem */
    }
  }
  /* Debug (1 s, TIM2) */
  if (tim2_flag)
  {
    tim2_flag = 0;

    can_check_bus();

    if (modules_disabled == MODULES_ALL)
    {
      println("Modulo desligado");
    }
    else
    {
      println("Iout=%.1fA Vout=%.2fV P=%.0fW Vset=%.2fV L1=%d L2=%d dis=%d",
              output_current, output_voltage, actual_power, voltage_set,
              loggedin1, loggedin2, modules_disabled);
    }

    if (last_unknown_id)
    {
      println("Mensagem nao reconhecida. ID: %lu", (unsigned long)last_unknown_id);
      last_unknown_id = 0;
    }
  }
}
