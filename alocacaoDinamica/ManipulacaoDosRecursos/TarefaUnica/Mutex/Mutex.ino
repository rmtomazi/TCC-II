#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <semphr.h>

#define ITERACOES 1000
#define PILHA_TE 120
#define PRIORIDADE_TE 3

//Definição das outras Macros desejáveis

void vTarefaEmissora(void *);

//Declaração de variáveis globais necessárias
SemaphoreHandle_t xMutex;

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  

  //Espaço para criar os recursos
  xMutex = xSemaphoreCreateMutex();
  
  xTaskCreate(vTarefaEmissora, NULL, PILHA_TE, NULL, PRIORIDADE_TE, NULL);  //Cria a tarefa emissora, que medirá o tempo de manipulação do recurso
  vTaskStartScheduler(); //Inicia o escalonador
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

void vTarefaEmissora(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;
  float tempoFuncaoMicros = 0.0, mediaTempoAquisicao = 0.0, mediaTempoEntrega = 0.0;

  do{
    inicio = micros();
    fim = micros();
    tempoFuncaoMicros += float(fim - inicio);
  }while(i--);
  tempoFuncaoMicros /= float(ITERACOES);
  
  i = ITERACOES;
  
  do{
    //Espaço para executar as funções de manipulação do recurso
    inicio = micros();  //Salva o tempo antes da execução da função de aquisição do mutex
    xSemaphoreTake(xMutex, portMAX_DELAY);
    fim = micros();     //Salva o tempo depois da execução da função de aquisição do mutex
    mediaTempoAquisicao += float(fim - inicio);
    inicio = micros();  //Salva o tempo antes da execução da função de entrega do mutex
    xSemaphoreGive(xMutex);
    fim = micros();     //Salva o tempo depois da execução da função de entrega do mutex
    mediaTempoEntrega += float(fim - inicio);
  }while(i--);
  
  mediaTempoAquisicao = ((mediaTempoAquisicao / (float)ITERACOES) 
                        - tempoFuncaoMicros);           //Calcula o tempo médio para a aquisição do mutex, onde divide-se a soma de todos os
                                                        //tempos de aquisição pelo número de iterações.
 
  mediaTempoEntrega = ((mediaTempoEntrega / (float)ITERACOES) 
                        - tempoFuncaoMicros);           //Similar ao cálculo do tempo de aquisição, para calcular o tempo médio para a entrega do mutex,
                                                        //divide-se a soma de todos os tempos de entrega pelo número de iterações.

  Serial.print("Aquisicao: ");
  Serial.print(mediaTempoAquisicao);
  Serial.print(" - Entrega: ");
  Serial.println(mediaTempoEntrega);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

void loop(){

}
