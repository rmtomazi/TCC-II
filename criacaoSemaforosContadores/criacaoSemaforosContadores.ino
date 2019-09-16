#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <avr/power.h>

#define CONTAGEM_MAX 1
#define INICIO_CONTAGEM 0

#define ITERACOES 100000

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  
  
  uint32_t inicio = 0, fim = 0, soma = 0, i = ITERACOES;
  float mediaFuncaoTempo = 0.0, tempoMedioCriacao = 0.0;

  //laço para verificar o atraso da função que retorna o tempo em microssegundos
  do{
    inicio = micros();
    fim = micros();
    soma += fim - inicio;
  }while(i--);
  mediaFuncaoTempo = (float)soma / (float)ITERACOES;

  Serial.println(mediaFuncaoTempo);
  
  i = ITERACOES;
  soma = 0;
  
  SemaphoreHandle_t xSemaphCounting;
  
  do{
    inicio = micros();
    xSemaphCounting =  xSemaphoreCreateCounting(CONTAGEM_MAX, INICIO_CONTAGEM);
    fim = micros();
    soma += fim - inicio;
    vSemaphoreDelete(xSemaphCounting);
  }while(i--);
  
  tempoMedioCriacao = ((float)soma / (float)ITERACOES) - mediaFuncaoTempo;
  
  Serial.println(tempoMedioCriacao);
}

void loop(){
  
}
