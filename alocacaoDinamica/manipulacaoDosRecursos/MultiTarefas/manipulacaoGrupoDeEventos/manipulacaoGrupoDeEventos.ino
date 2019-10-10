#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <event_groups.h>

#define ITERACOES 1000
#define PILHA_TE 120
#define PILHA_TR 120
#define PRIORIDADE_TE 2
#define PRIORIDADE_TR 3

//Definição das outras Macros desejáveis

void vTarefaEmissora(void *);
void vTarefaReceptora(void *);

//Declaração de variáveis globais necessárias
EventGroupHandle_t xEventGroup1, xEventGroup2;

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  

  //Espaço para criar os recursos
  xEventGroup1 = xEventGroupCreate();
  xEventGroup2 = xEventGroupCreate();
  
  xTaskCreate(vTarefaEmissora, NULL, PILHA_TE, NULL, PRIORIDADE_TE, NULL);  //Cria a tarefa emissora, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaEmissora, NULL, PILHA_TE, NULL, PRIORIDADE_TE, NULL);  //Cria a tarefa emissora, que medirá o tempo de manipulação do recurso
  //xTaskCreate(vTarefaEmissora, NULL, PILHA_TE, NULL, PRIORIDADE_TE, NULL);  //Cria a tarefa emissora, que medirá o tempo de manipulação do recurso
  //xTaskCreate(vTarefaEmissora, NULL, PILHA_TE, NULL, PRIORIDADE_TE, NULL);  //Cria a tarefa emissora, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaReceptora, NULL, PILHA_TR, NULL, PRIORIDADE_TR, NULL);  //Cria a tarefa receptora, que receberá o recurso e devolverá à tarefa emissora
  vTaskStartScheduler(); //Inicia o escalonador
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

void vTarefaEmissora(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;  
  float mediaTempo = 0.0;

  inicio = micros();  //Salva o tempo antes da execução do loop que manipulará os recursos
  do{
    //Espaço para executar as funções de manipulação do recurso
    xEventGroupSetBits(xEventGroup1, 0x01);
    xEventGroupWaitBits(xEventGroup2, 0x01, pdTRUE, pdFALSE, portMAX_DELAY);
  }while(i--);
  fim = micros();   //Salva o tempo depois da execução do loop que manipulará os recursos
  
  mediaTempo = float(fim - inicio) / (float)ITERACOES;  //Calcula o tempo médio para a execução do laço de testes dos recursos, onde divide-se o tempo total
                                                        //para a execução do laço pelo número de iterações.
                                                        //A fórmula pode ser alterada de acordo com a necessidade do recurso
  
  Serial.print(mediaTempo);
  Serial.print("\t");
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

void vTarefaReceptora(void *){
  do{
    //Recebe o recurso e devolve para a tarefa emissora
    xEventGroupWaitBits(xEventGroup1, 0x01, pdTRUE, pdFALSE, portMAX_DELAY);
    xEventGroupSetBits(xEventGroup2, 0x01);
  }while(1);
}

void loop(){

}
