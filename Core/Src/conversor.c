#include "main.h"
#include "conversor.h"
#include "can.h"
#include "uart.h"

float power_control(float voltage_set, int power)
{
  (void)voltage_set;

  float power_diff = (float)power - actual_power;

  float PTerm = P_CONST * power_diff;

  float ITerm = LastITerm + I_CONST * power_diff;
  if (ITerm > (MAX_VOLTAGE - MIN_VOLTAGE)) ITerm = (MAX_VOLTAGE - MIN_VOLTAGE);  /* anti-windup */
  if (ITerm < 0) ITerm = 0;

  float DTerm = D_CONST * (power_diff - LastPowerDiff);

  float voltage = MIN_VOLTAGE + PTerm + ITerm + DTerm;

  if (voltage > MAX_VOLTAGE) voltage = MAX_VOLTAGE;
  if (voltage < MIN_VOLTAGE) voltage = MIN_VOLTAGE;

  LastPowerDiff = power_diff;
  LastITerm = ITerm;

  return voltage;
}

void conversor_run(void)
{
  /* Login dos modulos */
  if (loginrequest1) send_login_ack(1);
  if (loginrequest2) send_login_ack(2);

  /* Malha de controle (100 ms) */
  if (tim3_flag)
  {
    tim3_flag = 0;

    process_messages();
    voltage_set = power_control(voltage_set, POWER_REQUEST_W);

    if (modules_disabled != 3)
    {
      float measured = (output_voltage1 + output_voltage2) / 2.0f;
      create_status_send(current_limit, measured, voltage_set, ovp_limit);

      if (loggedin1 || loggedin2) send_status();
    }
  }

  /* Parametro padrao (500 ms) */
  if (tim4_flag)
  {
    tim4_flag = 0;
    contador++;

    if (modules_disabled != 3)
    {
      create_writeparam_send(standard_voltage);
      if (loggedin1 || loggedin2) send_writeparam();
    }
  }

  /* Debug (1 s) */
  if (tim2_flag)
  {
    tim2_flag = 0;
    println("Iout=%.1fA Vout=%.2fV P=%.0fW Vset=%.2fV L1=%d L2=%d",
            output_current, output_voltage, actual_power, voltage_set,
            loggedin1, loggedin2);
  }
}
