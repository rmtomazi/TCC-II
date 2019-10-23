#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

#include <timers.h>

PILHA_TAREFA 100
PRIORIDADE_TAREFA 3
QUANTIDADE_TEREFAS 1

void vTarefa(void *);
void FuncaoRetorno(TimerHandle_t xTimer);

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);
  for(int i = 0; i < QUANTIDADE_TAREFAS; i++){
    xTaskCreate(vTarefa, NULL, PILHA_TAREFA, NULL, PRIORIDADE_TAREFA, NULL);  //Cria a tarefa que irá disputar a CPU com a tarefa Daemon
  }
  TimerHandle_t timer1 = xTimerCreate("", 500 / portTICK_PERIOD_MS, pdTRUE, 0, FuncaoRetorno);
  xTimerStart(timer1, 0);
  vTaskStartScheduler(); //Inicia o escalonador
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

void FuncaoRetorno(TimerHandle_t xTimer){
  Serial.println(micros());
}

void vTarefa(void *){
  for( ; ; );
}

void loop() {
  // put your main code here, to run repeatedly:

}
