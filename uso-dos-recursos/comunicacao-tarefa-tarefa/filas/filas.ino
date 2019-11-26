#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <queue.h>

//Definição das Macros desejáveis
#define ITERACOES       1000
#define PILHA_TP        120   //Pilha da tarefa principal
#define PILHA_TS        120   //Pilha da tarefa secundária
#define PRIORIDADE_TP   3     //Prioridade da tarefa principal
#define PRIORIDADE_TS   3     //Prioridade da tarefa secundária

#define TAM_MENSAGEM    128

void vTarefaPrincipal(void *);
void vTarefaSecundaria(void *);

//Declaração de variáveis globais necessárias
QueueHandle_t xFila1, xFila2;
char mensagem[TAM_MENSAGEM];

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
  xFila1 = xQueueCreate(1, TAM_MENSAGEM);
  xFila2 = xQueueCreate(1, TAM_MENSAGEM);

  //Criação a tarefa principal, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaPrincipal, NULL, PILHA_TP, NULL, PRIORIDADE_TP, NULL);
  
  //Criação da tarefa secundária, que receberá o recurso e devolverá à tarefa principal
  xTaskCreate(vTarefaSecundaria, NULL, PILHA_TS, NULL, PRIORIDADE_TS, NULL);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de envio de uma mensagem através da
 * fila. Essa tarefa irá colocar uma mensagem na xFila1. A tarefa secundária estará aguardando
 * por essa mensagem e ao receber irá adicionar a mesma na xFila2, fila essa que a tarefa 
 * principal estará aguardando. Ou seja, a cada mensagem enviada pela tarefa principal, haverá
 * uma também enviada pela tarefa secundária.
 */

void vTarefaPrincipal(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;  
  float mediaTempo = 0.0;

  inicio = micros();  //Salva o tempo antes da execução do loop que manipulará os recursos
  do{
    //Espaço para executar as funções de manipulação do recurso
    xQueueSend(xFila1, mensagem, portMAX_DELAY);     //Envia a mensagem para a tarefa secundária
    xQueueReceive(xFila2, mensagem, portMAX_DELAY);  //Aguarda pela mensagem da tarefa secundária
  }while(i--);
  fim = micros();   //Salva o tempo depois da execução do loop que manipulará os recursos

  /* O cálculo do tempo se dá descontado do tempo final o tempo inicial e dividindo-se pelo
   * número de iterações e depois dividindo-se por 2, uma vez que para cada iteração que a
   * tarefa principal executou, foi executada outra, de forma idêntica, pela tarefa secundária.
   */
  mediaTempo = float(fim - inicio) / (float)ITERACOES;
  mediaTempo /= 2.;
  
  Serial.println(mediaTempo);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

/* A tarefa secundária irá permanecer em estado bloqueado até que a tarefa 1 envie uma mensagem
 * pela xFila1. Ao receber a mensagem ela irá ser desbloqueada e também enviará uma mensagem
 * idêntica à tarefa principal pela xFila2. Ficando nesse laço por tempo indeterminado.
 */
void vTarefaSecundaria(void *){
  do{
    //Recebe o recurso e devolve para a tarefa principal
    xQueueReceive(xFila1, mensagem, portMAX_DELAY);  //Aguarda pela mensagem da tarefa primária
    xQueueSend(xFila2, mensagem, portMAX_DELAY);     //Envia a mensagem para a tarefa primária
  }while(1);
}

void loop(){

}
