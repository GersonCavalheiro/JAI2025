/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, demonstra o uso de cancelamento cooperativo com a classe std::jthread e o mecanismo associado de std::stop_token. Duas threads são lançadas para executar a mesma tarefa repetidamente, até que uma solicitação de cancelamento seja feita. Cada thread imprime uma mensagem indicando que está trabalhando, e ao receber o pedido de interrupção, encerra sua execução de forma controlada e segura.

Parâmetros de Lançamento

O programa não exige parâmetros de entrada. Para compilar e executar, utilize:

g++ -std=c++20 -o cancelamento_cooperativo cancelamento_cooperativo.cpp
./cancelamento_cooperativo

Recursos de Programação Concorrente Utilizados

Este exemplo utiliza recursos introduzidos no C++20 para facilitar o gerenciamento da vida de threads e a implementação de cancelamento cooperativo. Os principais recursos são:

- std::jthread: Alternativa moderna à std::thread, que integra automaticamente a chamada a join() em seu destrutor e oferece suporte ao cancelamento cooperativo por meio de std::stop_token.
- std::stop_token: Fornecido à função da thread no momento da criação, permite consultar se foi solicitado o cancelamento.
- request_stop(): Método de std::jthread que envia um pedido de parada para a thread associada. A thread deve verificar periodicamente esse pedido para encerrar-se de forma limpa.
- std::this_thread::sleep_for(): Utilizado para simular trabalho e permitir a visualização do cancelamento.

O programa ilustra o uso de cancelamento cooperativo em um cenário simples, destacando como o C++20 facilita a escrita de código concorrente mais seguro e de fácil controle, com menor risco de vazamentos de recursos ou encerramentos abruptos de execução.
*/

#include <iostream>
#include <thread>
#include <chrono>

void tarefa(std::stop_token st, int i) {
  while (!st.stop_requested()) {
    std::cout << "Trabalhando...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  std::cout << "Thread " << i << " cancelada com segurança.\n";
}

int main() {
  std::jthread t1(tarefa,1);
  std::jthread t2(tarefa,2);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  t1.request_stop();
  t2.request_stop();
  return 0;
}
