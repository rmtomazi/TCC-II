#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <event_groups.h>

//Definição das Macros desejáveis
#define ITERACOES       1000
#define PILHA_TP        120   //Pilha da tarefa principal
#define PILHA_TS        120   //Pilha da tarefa secundária
#define PRIORIDADE_TP   3     //Prioridade da tarefa principal
#define PRIORIDADE_TS   3     //Prioridade da tarefa secundária

#define PILHA_TX        50            //Pilha da tarefa X
#define PRIORIDADE_TX   PRIORIDADE_TP //Prioridade da tarefa X
#define QNT_TX          0             //Quantidade de tarefas X

#define BITS            0xff //Define quais bits serão alterados no grupo de eventos

void vTarefaPrincipal(void *);
void vTarefaSecundaria(void *);
void vTarefaX(void *);

//Declaração de variáveis globais necessárias
EventGroupHandle_t xGrupoDeEventos1, xGrupoDeEventos2;

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
  xGrupoDeEventos1 = xEventGroupCreate();
  xGrupoDeEventos2 = xEventGroupCreate();
  
  //Criação a tarefa principal, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaPrincipal, NULL, PILHA_TP, NULL, PRIORIDADE_TP, NULL);
  
  //Criação da tarefa secundária, que receberá o recurso e devolverá à tarefa principal
  xTaskCreate(vTarefaSecundaria, NULL, PILHA_TS, NULL, PRIORIDADE_TS, NULL);

  //Criação das tarefas X, que irão apenas consumir CPU, disputando-a com a tarefa principal
  for(int i = 0; i < QNT_TX; i++)
    xTaskCreate(vTarefaX, NULL, PILHA_TX, NULL, PRIORIDADE_TX, NULL);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de sinalização de um evento no grupo de
 * eventos. Essa tarefa irá definir para 1 todos os bit definidos na macro BITS no
 * xGrupoDeEventos1 e irá aguardar por todos os bits da macros BITS no xGrupoDeEventos2. Já a
 * tarefa secundária estará aguardando pelos bits da macro BITS no xGrupoDeEventos1, que a
 * tarefa 1 irá atribuir e ao receber todos os bits, irá sinalizar os mesmos bits para o 
 * xGrupoDeEventos2,que a tarefa principal estará aguardando. Ou seja, a cada sinalização no
 * grupo de eventos realizados pela tarefa principal, haverá sinalização também realizada pela
 * tarefa secundária.
 */

void vTarefaPrincipal(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;  
  float mediaTempo = 0.0;

  inicio = micros();  //Salva o tempo antes da execução do loop que manipulará os recursos
  do{
    //Espaço para executar as funções de manipulação do recurso
    xEventGroupSetBits(xGrupoDeEventos1, BITS);  //Altera os bits no xGrupoDeEventos1
    xEventGroupWaitBits(xGrupoDeEventos2, BITS, pdTRUE, pdTRUE, portMAX_DELAY);
                                                 //Aguarda pelos bits no xGrupoDeEventos2
  }while(i--);
  fim = micros();   //Salva o tempo depois da execução do loop que manipulará os recursos


  /* O cálculo do tempo se dá descontado do tempo final o tempo inicial e dividindo-se pelo
   *  número de iterações e depois dividindo-se por 2, uma vez que para cada iteração que a
   *  tarefa principal executou, foi executada outra, de forma idêntica, pela tarefa secundária.
   */
  mediaTempo = float(fim - inicio) / (float)ITERACOES;
  mediaTempo /= 2.;
  
  Serial.print(mediaTempo);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

/* A tarefa secundária irá permanecer em estado bloqueado até que a tarefa 1 altere os bits,
 * definidos pela macro BITS do xGrupoDeEventos1. Quando a tarefa principal alterar os bits,
 * a tarefa secundária também irá alterar os mesmos bits do xGrupoDeEventos2 e irá voltar para
 * o início da tarefa, ficando nesse laço por tempo indeterminado.
 */
void vTarefaSecundaria(void *){
  do{
    //Recebe o recurso e devolve para a tarefa principal
    xEventGroupWaitBits(xGrupoDeEventos1, BITS, pdTRUE, pdTRUE, portMAX_DELAY);
                                                //Aguarda pelos bits no xGrupoDeEventos1
    xEventGroupSetBits(xGrupoDeEventos2, BITS); //Altera os bits no xGrupoDeEventos2
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
