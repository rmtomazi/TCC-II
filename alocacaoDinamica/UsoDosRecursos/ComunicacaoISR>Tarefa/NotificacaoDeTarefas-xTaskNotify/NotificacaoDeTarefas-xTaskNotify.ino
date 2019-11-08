#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias

//Definição das Macros desejáveis
#define ITERACOES       1000
#define PILHA_TP        120   //Pilha da tarefa principal
#define PILHA_TS        120   //Pilha da tarefa secundária
#define PRIORIDADE_TP   3     //Prioridade da tarefa principal
#define PRIORIDADE_TS   2     //Prioridade da tarefa secundária

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

#if defined(CORE_TEENSY)
const uint8_t pinoInterrupcao = 0;
#elif defined(__AVR_ATmega32U4__)
const uint8_t pinoInterrupcao = 3;
#else
const uint8_t pinoInterrupcao = 2;
#endif

void vTarefaPrincipal(void *);
void vTarefaSecundaria(void *);
static void ulTrataInterrupcaoISR(void);

//Declaração de variáveis globais necessárias
TaskHandle_t xTarefaPrincipal;
uint32_t somaTempo = 0;

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
  xTaskCreate(vTarefaSecundaria, NULL, PILHA_TS, NULL, PRIORIDADE_TS, NULL);

  //Criação do serviço de interrupção
  pinMode(pinoInterrupcao, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pinoInterrupcao), ulTrataInterrupcaoISR, RISING);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de envio de sinalização de um evento
 * usando notificação de tarefas. Essa tarefa irá aguardar pelo sinal, gerado pela rotina do
 * serviço de interrupção, que por sua vez, tratará a interrupção gerada pela tarefa
 * secundária. A tarefa principal apenas irá somar o tempo à variável somaTempo, para que
 * possa-se calcular o tempo médio que o sinal (recurso) leva para sair da rotina do
 * serviço de interrupção e chegar em uma tarefa.
 */

void vTarefaPrincipal(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t ulValue = 0xff;
  do{
    xTaskNotifyWait(0, 0xffffffff, &ulValue, portMAX_DELAY); //Aguarda uma notificação da ISR
    somaTempo += micros();
  }while(i--);

  /* O cálculo da média do tempo se dá pegando a soma dos tempos de todas as sinalizaçõs a
   * a partir do recurso e dividindo-o pelo total de iterações.
   */
  float mediaTempo = float(somaTempo) / (float)ITERACOES; 
  
  Serial.println(mediaTempo);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

/* A tarefa secundária irá apenas gerar um sinal no pino de interrupção, para que o tratamento
 * de interrupção seja acionado. Ela só será executada enquanto a tarefa principal estiver
 * bloqueada, pois tem prioridade menor.
 */

void vTarefaSecundaria(void *){
  do{
    digitalWrite(pinoInterrupcao, LOW);
    digitalWrite(pinoInterrupcao, HIGH);
    taskYIELD();
  }while(1);
}

/* A rotina do serviço de interrupção irá apenas diminuir o tempo inicial da soma do tempo e mandar
 * um sinal a partir do recurso para a tarefa principal.
 */
static void ulTrataInterrupcaoISR(void){
    uint32_t ulValue = 0xff;
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    somaTempo -= micros();
    //Envia a mensagem para a tarefa primária
    xTaskNotifyFromISR(xTarefaPrincipal, 0x255, eAction, &xHigherPriorityTaskWoken);
}

void loop(){

}
