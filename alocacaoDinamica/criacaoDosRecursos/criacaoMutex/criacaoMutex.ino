#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias
#include <semphr.h>

#define ITERACOES 30
#define TAM_PILHA 200
#define PRIORIDADE_TP 3

//Definição das outras Macros desejáveis

void vTarefaPrincipal(void *);

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  

  xTaskCreate(vTarefaPrincipal, NULL, TAM_PILHA, NULL, PRIORIDADE_TP, NULL);  //Cria a tarefa principal
  vTaskStartScheduler(); //Inicia o escalonador
  for( ;; ); //Se o escalonador foi devidamente inciado, este laço não deverá ser executado
}

void vTarefaPrincipal(void *){
  unsigned int inicio = 0, fim = 0, i = ITERACOES;  
  float mediaTempoFuncaoMicros = 0.0, mediaTempoCriacao = 0.0, mediaTempoExclusao = 0.0;

  //O trecho de código abaixo serve para verificar o tempo de execução entre as chamadas da função micros(), que retorna o tempo atual do
  //microcontrolador. Esse laço vai ser executada i vezes, onde i será definido pela macro ITERACOES. Como há tambem um atraso entre as
  //medições de tempo, esse tempo deve ser diminuido do tempo lido na criação e exclusão dos recursos, para que eles se tornem mais precisos.
  do{
    inicio = micros();  //Salva o primeiro retorno de tempo
    fim = micros();     //Salva o segundo retorno de tempo
    mediaTempoFuncaoMicros += (float)(fim - inicio);  //Faz a soma das diferenças de tempo entre a execução das duas linhas acima, de todas
                                                      //as iterações
  }while(i--);
  mediaTempoFuncaoMicros /= (float)ITERACOES; //Calcula o tempo médio entre as chamadas de função micro, onde pega-se a soma das diferenças
                                              //de tempo e divide-se pelo total de iterações
  
  i = ITERACOES;

  //Criação de outras variáveis necessárias para a execução da criação e exclusão do recurso
  SemaphoreHandle_t xMutex = NULL;
  
  do{
    inicio = micros();  //Salva o tempo antes da execução da função de criação do recurso
    //Espaço para executar a função de criação do recurso
    xMutex = xSemaphoreCreateMutex();
    fim = micros();     //Salva o tempo depois da execução da função de criação do recurso
    if(xMutex != NULL){               //Condição para verificar se o recurso foi criado
      mediaTempoCriacao += (float)(fim - inicio);   //Faz a soma das diferenças de tempo entre o tempo captado antes e depois da execução da
                                                    //função de criação, em todas as iterações, desde que o recurso tenha sido devidamente
                                                    //criado
      inicio = micros();  //Salva o tempo antes da execução da função de exclusão do recurso
      vSemaphoreDelete(xMutex);
      //Espaço para executar as função de exclusão do recurso
      fim = micros();     //Salva o tempo depois da execução da função de exclusão do recurso
      mediaTempoExclusao += (float)(fim - inicio);  //Faz a soma das diferenças de tempo entre o tempo captado antes e depois da execução da
                                                    //função de exclusão, em todas as iterações, desde que o recurso tenha sido devidamente
                                                    //criado
      xMutex = NULL;
    }else //Caso o recurso não tenha sido devidamente criado, incrementa-se 1 (um) na variável i, para que o laço possa executar novamente
          //essa iteração
      i++;
  }while(i--);
  
  mediaTempoCriacao /= (float)ITERACOES;  //Calcula o tempo médio entre as chamadas de função micro presente antes e depois da chamada da
                                          //função de criação do recurso, para isso, pega a soma de todos as iterações e divide-se pelo total
                                          //de iterações

  mediaTempoExclusao /= (float)ITERACOES; //Semelhante à operação acima, porém para calcular o tempo médio entre as chamadas de função micro
                                          //presente antes e depois da chamada da função de exclusão do recurso,

  mediaTempoCriacao -= mediaTempoFuncaoMicros;  //Para calcular o tempo médio final para criação dos recursos, basta pegar o tempo médio já 
                                                //calculado e descontar o o tempo médio entre as chamadas da função micros(), calculado 
                                                //anteriormente e armazenado na variável mediaTempoFuncaoMicros

  mediaTempoExclusao -= mediaTempoFuncaoMicros; //Semelhante à operação acima, porém para calcular o tempo médio final para exclusão dos
                                                //recursos
  
  Serial.println(mediaTempoCriacao);
  Serial.println(mediaTempoExclusao);
  vTaskDelete(NULL);  //A tarefa principal se auto exclui após atingir seu objetivo
}

void loop(){

}
