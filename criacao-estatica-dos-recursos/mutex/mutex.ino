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


  //Criação a tarefa principal, que medirá o tempo de criação do recurso
  xTaskCreate(vTarefaPrincipal, NULL, PILHA_TP, NULL, PRIORIDADE_TP, NULL);

  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de criação e exclusão do recurso. Essa
 * tarefa irá executar as funções de criação e exclusão do recurso várias vezes e calcular a
 * média do tempo de execução de cada função.
 */
void vTarefaPrincipal(void *){
  unsigned int inicio = 0, fim = 0, i = ITERACOES;  
  float mediaTempoCriacao = 0.0, mediaTempoExclusao = 0.0;
  
  do{
    SemaphoreHandle_t xMutex = NULL;
    StaticSemaphore_t xBufferDoMutex;
    inicio = micros();  //Salva o tempo antes da execução da função de criação do recurso
    //Espaço para executar a função de criação do recurso
    xMutex = xSemaphoreCreateMutexStatic(&xBufferDoMutex);
    fim = micros();     //Salva o tempo depois da execução da função de criação do recurso
    if(xMutex != NULL){ //Caso o recurso tenha sido devidamente criado:
      //Guarda o tempo que levou para criar o recurso, descontando o tempo inicial do tempo final
      mediaTempoCriacao += (float)(fim - inicio);
      inicio = micros();  //Salva o tempo antes da execução da função de exclusão do recurso
      //Espaço para executar as função de exclusão do recurso
      vSemaphoreDelete(xMutex);;
      fim = micros();     //Salva o tempo depois da execução da função de exclusão do recurso
      //Guarda o tempo que levou para excluir o recurso, descontando o tempo inicial do tempo final
      mediaTempoExclusao += (float)(fim - inicio);
      xMutex = NULL;
    }else
    /* Caso o recurso não tenha sido devidamente criado, incrementa-se 1 (um) na variável i, para 
     * que o laço possa executar novamente essa iteração.
     */
      i++;
  }while(i--);

  /* O cálculo do tempo se dá pegando a soma de todas as criações ou exclusões e dividindo-as pelo
   * número de interações.
   */ 
  
  mediaTempoCriacao /= (float)ITERACOES;

  mediaTempoExclusao /= (float)ITERACOES; 
  
  Serial.print("Tempo de criação: ");
  Serial.println(mediaTempoCriacao);
  Serial.print("Tempo de exclusão: ");
  Serial.println(mediaTempoExclusao);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

void loop(){

}
