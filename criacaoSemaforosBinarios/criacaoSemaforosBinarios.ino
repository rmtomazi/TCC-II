#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

#include <semphr.h>

#define ITERACOES 20000

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
  
  i = ITERACOES;
  soma = 0;
  
  SemaphoreHandle_t xSemaphBinary;

  do{
    inicio = micros();
    xSemaphBinary =  xSemaphoreCreateBinary();
    fim = micros();
    soma += fim - inicio;
    vSemaphoreDelete(xSemaphBinary);
  }while(i--);
  
  tempoMedioCriacao = ((float)soma / (float)ITERACOES) - mediaFuncaoTempo;
  
  Serial.println(tempoMedioCriacao);
}

void loop(){
  
}
