#include <Arduino_FreeRTOS.h>
#include <avr/power.h>

//Inclusão das outras Bibliotecas necessárias

#define ITERACOES 10000

//Definição das outras Macros desejáveis

void setup(){
  
  if (F_CPU == 8000000L) clock_prescale_set(clock_div_2);
  if (F_CPU == 4000000L) clock_prescale_set(clock_div_4);
  if (F_CPU == 2000000L) clock_prescale_set(clock_div_8);
  if (F_CPU == 1000000L) clock_prescale_set(clock_div_16);
  
  Serial.begin(9600);  
  
  uint32_t inicio = 0, fim = 0, soma = 0, i = ITERACOES;
  float mediaTempoLoop = 0.0, mediaLoopCriacao = 0.0, tempoMedioCriacao = 0.0;

  //O trecho de código abaixo serve para verificar o tempo de execução de um laço vazio com i iterações, definidas pela macro ITERACOES,
  //com o objetivo de verificar o tempo que o sistema leva para fazer a decrementação da variável, a comparação e o retorno para o inicio
  //do escopo do laço
  inicio = micros();  //Salva o tempo antes da execução do laço
  do{
  }while(i--);
  fim = micros();     //Salva o tempo depois da execução do laço
  mediaTempoLoop = (float)(fim - inicio) / (float)ITERACOES;  //Calcula o tempo médio para executar o laço, onde:
                                                              //Pega-se o tempo final e diminui dele o tempo inicial, restando o 
                                                              //tempo em microssegundos que levou-se para executar o laço, após
                                                              //divide-se pelo total de iterações do laço, resultando no tempo médio
                                                              //para executar uma única iteração do laço
  
  i = ITERACOES;

  //Criação de outras variáveis necessárias para a execução da criação e exclusão do recurso
  
  inicio = micros();  //Salva o tempo antes da execução do laço
  do{
    //Espaço para executar as funções de criação e exclusão do recurso
  }while(i--);
  fim = micros();     //Salva o tempo depois da execução do laço
  
  tempoLoopCriacao = ((float)(fim - inicio) / (float)ITERACOES);  //Calcula o tempo médio para executar o laço de criação e exclusão do recurso, onde:
                                                                  //Pega-se o tempo final e diminui dele o tempo inicial, restando o 
                                                                  //tempo em microssegundos que levou-se para executar o laço, após
                                                                  //divide-se pelo total de iterações do laço, resultando no tempo médio
                                                                  //para executar uma única iteração do laço

  tempoMedioCriacao = tempoLoopCriacao - mediaTempoLoop;    //O tempo médio final é calculado fazendo o tempo de cada iteração do laço de criação e
                                                            //diminuindo desse tempo o tempo médio para executar uma iteração do laço
  
  Serial.println(tempoMedioCriacao);
}

void loop(){

}
