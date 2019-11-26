 #include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <semphr.h>

//Definição das Macros desejáveis
#define ITERACOES       1000
#define PILHA_TP        200   //Pilha da tarefa principal
#define PRIORIDADE_TP   3     //Prioridade da tarefa principal

void vTarefaPrincipal(void *);

//Declaração de variáveis globais necessárias
SemaphoreHandle_t xMutex;

void setup(){

  /* Altera o clock da CPU de acordo com a configuração da placa.
   * A macro F_CPU contém a frequência da CPU e está declarada em
   * arduino-1.8.10/hardware/arduino/avr/boards.txt 
   */
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  

  //Espaço para criar os recursos
  xMutex = xSemaphoreCreateMutex();
  
  //Criação a tarefa principal, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaPrincipal, NULL, PILHA_TP, NULL, PRIORIDADE_TP, NULL);

  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de aquisição e entrega do mutex.
 * Essa tarefa irá executar a função de aquisição e entrega do mutex várias vezes e calcular 
 * a média do tempo de execução de cada função.
 */

void vTarefaPrincipal(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;
  float mediaTempoAquisicao = 0.0, mediaTempoEntrega = 0.0;
  
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

  /* O cálculo do tempo se dá pegando a soma de todos os tempos de aquisições e entregas
   * e dividido pelo número de interações.
   */
   
  mediaTempoAquisicao /= (float)ITERACOES;

  mediaTempoEntrega /= (float)ITERACOES;

  Serial.print("Aquisicao: ");
  Serial.print(mediaTempoAquisicao);
  Serial.print(" - Entrega: ");
  Serial.println(mediaTempoEntrega);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

void loop(){

}
