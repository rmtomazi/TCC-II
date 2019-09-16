#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

void vTask(void *pvParameters);
void vTask1(void *pvParameters);

#define ITERACOES 10000

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);
  
  unsigned int inicio = 0, fim = 0, i = ITERACOES;
  
  //laço para verificar o atraso da função que retorna o tempo em microssegundos
  inicio = micros();
  do{
  }while(i--);
  fim = micros();

  unsigned int mediaFuncaoTempo = fim - inicio;

  TaskHandle_t xTask1 = NULL;
  inicio = 0, fim = 0, i = ITERACOES;

  xTaskCreate(vTask1, NULL, 100, NULL, 3, &xTask1);
  vTaskDelete(xTask1);

  inicio = micros();
  do{
    xTaskCreate(vTask1, NULL, 100, NULL, 2, &xTask1);
    vTaskDelete(xTask1);
  }while(i--);
  fim = micros();
  
  float tempoMedioCriacao = ((float)(fim - inicio) - (float)mediaFuncaoTempo) / (float)ITERACOES;

  Serial.println(fim - inicio);
  Serial.println(mediaFuncaoTempo);
  Serial.println(tempoMedioCriacao);
  //vTaskStartScheduler();
  for(;;);
  
}

void vTask(void *){
  /*unsigned int inicio = 0, fim = 0, soma = 0, i = ITERACOES;
  float mediaFuncaoTempo = 0.0;
  
  //laço para verificar o atraso da função que retorna o tempo em microssegundos
  do{
    inicio = micros();
    fim = micros();
    soma += fim - inicio;
  }while(i--);

  mediaFuncaoTempo = (float)soma / (float)ITERACOES;
  
  TaskHandle_t xTask1;
  inicio = 0, fim = 0, soma = 0, i = ITERACOES;
  xTaskCreate(vTask1, NULL, 100, NULL, 2, &xTask1);
  do{
    Serial.println(i);
    xTask1 = NULL;
    inicio = micros();
    xTaskCreate(vTask1, NULL, 100, NULL, 2, &xTask1);
    fim = micros();
    if(xTask1 != NULL)
      vTaskDelete(xTask1);
    soma += fim - inicio;
    vTaskDelay(1000/ portTICK_PERIOD_MS);
  }while(i--);
  float tempoMedioCriacao = ((float)soma / (float)ITERACOES) - mediaFuncaoTempo;
  
  Serial.println(tempoMedioCriacao);*/
}

void vTask1(void *){

}

void loop(){
  
}
