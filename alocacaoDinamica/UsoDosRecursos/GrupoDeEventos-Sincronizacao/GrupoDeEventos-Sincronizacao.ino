#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <event_groups.h>

//Definição das Macros desejáveis
#define ITERACOES       1000
#define QNT_TAREFAS     7     //Quantidade de tarefas de sincronização
#define PILHA_T         91    //Pilha das tarefas de sincronização
#define PRIORIDADE_T    2     //Prioridade das tarefas de sincronização

#define PILHA_TX        50            //Pilha da tarefa X
#define PRIORIDADE_TX   PRIORIDADE_T  //Prioridade da tarefa X
#define QNT_TX          0             //Quantidade de tarefas X

//Define quais bits cada tarefa irá alterar
#define BIT_TAREFA (1UL << b)

void vTarefaSincronizacao(void *pvParameters);
void vTarefaX(void *);

//Declaração de variáveis globais necessárias
EventGroupHandle_t xGrupoDeEventos;

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
  xGrupoDeEventos = xEventGroupCreate();

  /* Criação das tarefas que irão medir o tempo de sincronização, passando como parâmetro o bit
   * que a tarefa irá alterar.
   */
  for(unsigned long b = 0; b < QNT_TAREFAS; b++)
    xTaskCreate(vTarefaSincronizacao, NULL, PILHA_T, (void *)BIT_TAREFA, PRIORIDADE_T, NULL);

  //Criação das tarefas X, que irão apenas consumir CPU, disputando-a com a tarefa principal
  for(int i = 0; i < QNT_TX; i++)
    xTaskCreate(vTarefaX, NULL, PILHA_TX, NULL, PRIORIDADE_TX, NULL);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* As tarefas desse teste serão todas iguais. Cada tarefa irá alterar seu respectivo bit no
 * xGrupoDeEventos e aguardar até que todas as tarefas alterem seus bits.
 * 
 */
void vTarefaSincronizacao(void *pvParameters){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;  
  float mediaTempo = 0.0;

  //Captura o bit da tarefa, passado como parâmetro
  EventBits_t uxBitDaTarefa = (EventBits_t) pvParameters;

  //Cria o grupo de bits que a tarefa deve aguardar
  unsigned long bits;
  for(unsigned long b = 0; b < QNT_TAREFAS; b++)
    bits |= BIT_TAREFA;

  //Cria a variável que contém os bits que a tarefa deve aguardar
  const EventBits_t uxTodosBitsDeSicronizacao = bits;

  //Aguarda até que todas as tarefas estejam prontas e cheguem nesse ponto
  xEventGroupSync(xGrupoDeEventos, uxBitDaTarefa, uxTodosBitsDeSicronizacao, portMAX_DELAY);
  
  inicio = micros();  //Salva o tempo antes da execução do loop que manipulará os recursos
  do{
    //Espaço para executar as funções de manipulação do recurso
    //Aguarda até que todas as tarefas alterem seus bits
    xEventGroupSync(xGrupoDeEventos, uxBitDaTarefa, uxTodosBitsDeSicronizacao, portMAX_DELAY);
  }while(i--);
  fim = micros();   //Salva o tempo depois da execução do loop que manipulará os recursos

  /* O cálculo do tempo se dá descontado do tempo final o tempo inicial e dividindo-se pelo
   *  número de iterações.
   */
  mediaTempo = float(fim - inicio) / (float)ITERACOES;
  
  vTaskSuspendAll();
  Serial.println(mediaTempo);
  xTaskResumeAll();
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

/* A tarefa X tem como função apenas consumir a CPU e que vai disputá-la com a tarefa principal.
 * Ela vai permanecer em um laço sem nenhuma instrução por tempo indeterminado.
 */
void vTarefaX(void *){
  for(;;);
}

void loop(){

}
