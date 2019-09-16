#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

#include <queue.h>

#define TAM_FILA 1
#define TAM_ITEM 1

#define ITERACOES 200000000

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  
  
  uint32_t inicio = 0, fim = 0, soma = 0, i = ITERACOES;
  float mediaTempoLoop = 0.0, tempoLoopCriacao = 0.0, tempoMedioCriacao = 0.0;

  //laço para verificar o atraso da função que retorna o tempo em microssegundos
  inicio = micros();
  do{  
  }while(i--);
  fim = micros();
  mediaTempoLoop = (float)(fim - inicio) / (float)ITERACOES;
  i = ITERACOES;
  QueueHandle_t xQueue;
  Serial.print("A");
  inicio = micros();
  do{
    xQueue = xQueueCreate(TAM_FILA, TAM_ITEM);
    vQueueDelete(xQueue);
  }while(i--);
  fim = micros();
  Serial.println("B");
  tempoLoopCriacao = ((float)(fim - inicio) / (float)ITERACOES);

  tempoMedioCriacao = tempoLoopCriacao - mediaTempoLoop;
  
  Serial.println(tempoMedioCriacao);
}

void loop(){

}
