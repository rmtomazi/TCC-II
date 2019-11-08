#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias

//Definição das Macros desejáveis
#define ITERACOES 1000
#define PILHA_TP        400   //Pilha da tarefa principal
#define PILHA_TS        200   //Pilha da tarefa secundária
#define PRIORIDADE_TP   3     //Prioridade da tarefa principal
#define PRIORIDADE_TS   3     //Prioridade da tarefa secundária

void vTarefaPrincipal(void *);
void vTarefaSecundaria(void *);

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
  
  //Criação a tarefa principal, que medirá o tempo de manipulação do recurso
  xTaskCreate(vTarefaPrincipal, NULL, PILHA_TP, NULL, PRIORIDADE_TP, NULL);
  
  //Criação da tarefa secundária, que receberá o recurso e devolverá à tarefa principal
  xTaskCreate(vTarefaSecundaria, NULL, PILHA_TS, NULL, PRIORIDADE_TS, NULL);
  
  vTaskStartScheduler(); //Inicia o escalonador
  
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

/* A tarefa principal tem como objetivo medir o tempo de da troca de contexto. Para isso, ela
 * irá realizar entregar a CPU para a tarefa secundária, que por sua vez, irá devolve-la para
 * a tarefa principal.
 */
 
void vTarefaPrincipal(void *){
  volatile uint32_t i = ITERACOES;
  uint32_t inicio = 0, fim = 0;  
  float mediaTempo = 0.0, mediaTempoLaco = 0.0;

  //Medição do tempo para execução do laço
  inicio = micros();  //Salva o tempo antes da execução do laço
  do{
  }while(i--);
  fim = micros();     //Salva o tempo depois da execução do laço que entrega a CPU à tarefa secundária
  /* Calcula o tempo que levou para o laço realizar uma iteração, dividindo o tempo total de execução
   * do laço, pelo total de iterações do laço. 
   */
  mediaTempoLaco = float(fim - inicio) / float(ITERACOES);

  i = ITERACOES;
  
  inicio = micros();  //Salva o tempo antes da execução do laço que entregará a CPU à tarefa secundária
  do{
    taskYIELD();  //Entrega a CPU para a tarefa secundária
  }while(i--);
  fim = micros();     //Salva o tempo depois da execução do laço que entrega a CPU à tarefa secundária

  /* O cálculo do tempo se dá descontado do tempo final o tempo inicial e dividindo-se pelo
   * número de iterações e depois dividindo-se por 2, uma vez que para cada troca de contexto
   * realizada da tarefa principal para a tarefa secundária, foi realizada também uma ao contrário,
   * pela tarefa secundária. Por fim, desconta-se desse tempo o tempo que o laço levou para realizar
   * uma iteração, calculado acima.
   */
  mediaTempo = ((float(fim - inicio) / (float)ITERACOES) 
               / 2.0);
  mediaTempo = mediaTempo - mediaTempoLaco;
  
  Serial.println(mediaTempo);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

/* A única função da tarefa secundária é devolver a CPU para a tarefa primária, realizando assim mais
 * uma troca de contexto.
 */
void vTarefaSecundaria(void *){
  do{
    taskYIELD();  //Entrega a CPU para a tarefa principal
  }while(1);
}

void loop(){

}
