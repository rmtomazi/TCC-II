#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias

//Definição das Macros desejáveis
#define ITERACOES       1000
#define PILHA_TP        120   //Pilha da tarefa principal
#define PILHA_TS        120   //Pilha da tarefa secundária
#define PRIORIDADE_TP   3     //Prioridade da tarefa principal
#define PRIORIDADE_TS   3     //Prioridade da tarefa secundária

#define PILHA_TX        50            //Pilha da tarefa X
#define PRIORIDADE_TX   PRIORIDADE_TP //Prioridade da tarefa X
#define QNT_TX          8             //Quantidade de tarefas X

/* Opções de eAction:
 *    -> eNoAction - Apenas informa a tarefa que há uma notificação, mas não altera
 *          o valor de notificação da tarefa. ulValue não é usado.
 *    -> eSetBits - Atribui UM nos bits informados em ulValue, no valor de notificação
 *          da tarefa.
 *    -> eIncrement - O valor de notificação da tarefa de recebimento é incrementado
 *          em 1. ulValue não é usado.
 *    -> eSetValueWithoutOverwrite - Caso a tarefa de recebimento tinha uma 
 *          notificação pendente, nenhuma ação será executada, caso contrário, o valor
 *          de notificação da tarefa de recebimento será alterado para o valor passado
 *          em ulValue.
 *    -> eSetValueWithOverwrite - O valor de notificação da tarefa de recebimento é
 *          definido como o valor transmitido por ulValue, independentemente de a
 *          tarefa de recebimento ter ou não uma notificação pendente.
 */
#define eAction       eSetValueWithOverwrite

void vTarefaPrincipal(void *);
void vTarefaSecundaria(void *);
void vTarefaX(void *);

//Declaração de variáveis globais necessárias
TaskHandle_t xTarefaPrincipal, xTarefaSecundaria;

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
  
  
  //Criação a tarefa principal, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaPrincipal, NULL, PILHA_TP, NULL, PRIORIDADE_TP, &xTarefaPrincipal);
  
  //Criação da tarefa secundária, que receberá o recurso e devolverá à tarefa principal
  xTaskCreate(vTarefaSecundaria, NULL, PILHA_TS, NULL, PRIORIDADE_TS, &xTarefaSecundaria);

  //Criação das tarefas X, que irão apenas consumir CPU, disputando-a com a tarefa principal
  for(int i = 0; i < QNT_TX; i++)
    xTaskCreate(vTarefaX, NULL, PILHA_TX, NULL, PRIORIDADE_TX, NULL);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de envio de uma mensagem através da
 * notificação de tarefas. Essa tarefa irá enviar uma mensagem para a tarefa secundária, que
 * estará bloqueada, aguardando pela notificação, e ao receber irá enviar também uma notificação
 * à tarefa principal, que estará aguardando bloqueada. Ou seja, a cada notificação enviada pela
 * tarefa principal, outra será enviada pela tarefa secundária.
 */

void vTarefaPrincipal(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0, ulValue = 0xff;  
  float mediaTempo = 0.0;

  inicio = micros();  //Salva o tempo antes da execução do loop que manipulará os recursos
  do{
    //Espaço para executar as funções de manipulação do recurso
    xTaskNotify(xTarefaSecundaria, ulValue, eAction); //Envia uma notificação para a tarefa secundária
    xTaskNotifyWait(0, 0xffffffff, &ulValue, portMAX_DELAY); //Aguarda uma notificação da tarefa secundária
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

/* A tarefa secundária irá permanecer em estado bloqueado até que a tarefa 1 envie uma
 * notificação. Ao receber a notificação ela irá ser desbloqueada e também enviará uma
 * notificação, com os mesmos valores, para a tarefa principal. Ficando nesse laço por
 * tempo indeterminado.
 */
 
void vTarefaSecundaria(void *){
  uint32_t ulValue;
  do{
    //Recebe o recurso e devolve para a tarefa principal
    xTaskNotifyWait(0, 0xffffffff, &ulValue, portMAX_DELAY);  //Aguarda uma notificação da tarefa principal
    xTaskNotify(xTarefaPrincipal, ulValue, eAction);  //Envia uma notificação para a tarefa principal
  }while(1);
}

/* A tarefa X tem como função apenas consumir a CPU e que vai disputá-la com a tarefa principal.
 * Ela vai permanecer em um laço sem nenhuma instrução por tempo indeterminado.
 */
void vTarefaX(void *){
  for(;;);
}

void loop(){

}
