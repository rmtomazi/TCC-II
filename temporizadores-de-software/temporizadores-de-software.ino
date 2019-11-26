#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <timers.h>

//Definição das Macros desejáveis
#define PILHA_TX        42                        //Pilha da tarefa X
#define PRIORIDADE_TX   configTIMER_TASK_PRIORITY //Prioridade da tarefa X
#define QNT_TX          1                         //Define a quantidade de tarefas X

void vTarefaX(void *);
void FuncaoRetorno(TimerHandle_t xTimer);

//Declaração de variáveis globais necessárias
volatile uint16_t TEMPO_TICKS, i;
TimerHandle_t xTimer;

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

  //Criação das tarefas que irão disputar a CPU com a tarefa Daemon
  for(int i = 0; i < QNT_TX; i++)
    xTaskCreate(vTarefaX, NULL, PILHA_TX, NULL, PRIORIDADE_TX, NULL);

  //Espaço para criar os recursos
  TEMPO_TICKS = 0;  //Variável que altera o tempo, em ticks, do temporizador
  i = 51; //Variável que conta as amostras de tempo impressas pelo temporizador
  xTimer = xTimerCreate("", 50, pdTRUE, 0, FuncaoRetorno);
  xTimerStart(xTimer, 50);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* Essa função vai ser chamada pelo temporizador a cada vez que ele expirar. Ela apenas
 * imprime o tempo atual, em microssegundos, na tela e verifica se já foram impressas 51
 * amostras de tempo. Se já foram, ela altera o tempo, em ticks, do temporizador.
 */
void FuncaoRetorno(TimerHandle_t xTimer){
  Serial.println(micros());
  i++;
  if(i >= 51){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStopFromISR(xTimer, &xHigherPriorityTaskWoken);  //Para o temporizador
    i = 0;
    TEMPO_TICKS++;  //Adiciona um tick ao tempo
    Serial.print("Tempo : "); //Imprime o novo tempo em milissegundos
    Serial.print(TEMPO_TICKS * portTICK_PERIOD_MS);
    Serial.println(" ms");
    //Altera o tempo do temporizador para o novo tempo, da variável TEMPO_TICKS
    xTimerChangePeriodFromISR(xTimer, TEMPO_TICKS, &xHigherPriorityTaskWoken);
  }
  
}

/* A funcão das tarefas é apenas consumir recursos da CPU, para analisar como os temporizadores
 * se comportam quando a tarefa Daemon não consegue adquirir a CPU à tempo.
 */
void vTarefaX(void *){
  for( ;; );
}

void loop() {
  // put your main code here, to run repeatedly:

}
