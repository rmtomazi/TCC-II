#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

#include <timers.h>

#define ITERACOES 1000
#define PILHA_TP 400
#define PILHA_TS 200
#define PRIORIDADE_TP 3
#define PRIORIDADE_TS 2

void vFuncaoRetorno(TimerHandle_t xTimer);

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);

  TimerHandle_t timer1 = xTimerCreate("", 500 / portTICK_PERIOD_MS, pdTRUE, 0, vFuncaoRetorno);
  xTimerStart(timer1, 0);
  vTaskStartScheduler(); //Inicia o escalonador
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

void vFuncaoRetorno(TimerHandle_t xTimer){
  Serial.println(micros());
}
void loop() {
  // put your main code here, to run repeatedly:

}
