#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

#define ITERACOES 1000
#define PILHA_TP 400
#define PILHA_TS 200
#define PRIORIDADE_TP 3
#define PRIORIDADE_TS 2

void vTarefaPrimaria(void *);
void vTarefaSecundaria(void *);

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  
  
  xTaskCreate(vTarefaPrimaria, NULL, PILHA_TP, NULL, PRIORIDADE_TP, NULL);  //Cria a tarefa primária, que medirá o tempo da troca de contexto
  xTaskCreate(vTarefaSecundaria, NULL, PILHA_TS, NULL, PRIORIDADE_TS, NULL);  //Cria a tarefa secundária, que devolverá o processador para a tarefa primária
  vTaskStartScheduler(); //Inicia o escalonador
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

void vTarefaPrimaria(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;  
  float mediaTempo = 0.0, mediaTempoLaco = 0.0;

  //Medição do tempo para execução do laço
  inicio = micros();
  do{
  }while(i--);
  fim = micros();
  mediaTempoLaco = float(fim - inicio) / float(ITERACOES);

  i = ITERACOES;
  inicio = micros();  //Salva o tempo antes da execução do laço que entregará a CPU à tarefa secundária
  do{
    taskYIELD();  //Entrega a CPU
  }while(i--);
  fim = micros();   //Salva o tempo depois da execução do laço que entrega a CPU à tarefa secundária
  
  mediaTempo = ((float(fim - inicio) / (float)ITERACOES) 
               / 2.0);                                  //Calcula o tempo médio para a execução do laço da entrega da CPU, onde,
                                                        //divide-se o tempo total da execução do laço pelo número de iterações e divide-se
                                                        //por 2, pois houve duas trocas de contexto, uma na tarefa primaria e outra na tarefa
                                                        //secundária.
  mediaTempo = mediaTempo - mediaTempoLaco;             //Desconta-se o tempo de execução médio de cada iteração do laço
  Serial.println(mediaTempo);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

void vTarefaSecundaria(void *){
  do{
    taskYIELD();
  }while(1);
}

void loop(){

}
