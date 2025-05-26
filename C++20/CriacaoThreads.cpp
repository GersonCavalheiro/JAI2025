/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa escrito em C++20 tem como objetivo ilustrar três formas distintas de criação de threads por meio da biblioteca padrão da linguagem. Ao ser executado, o programa inicia três threads distintas, cada uma associada a um tipo diferente de função:

- Uma função livre (foo)
- Um functor (objeto com operador operator())
- Uma lambda function

Cada thread imprime uma mensagem indicando sua forma de construção. Após sua criação, o programa principal aguarda o término das três threads utilizando o método join().

Parâmetros de Lançamento

O programa não exige parâmetros de entrada. Basta compilá-lo com um compilador que ofereça suporte ao padrão C++20 e executá-lo diretamente:

g++ -std=c++20 -o CriacaoThreads CriacaoThreads.cpp
./CriacaoThreads

Recursos de Programação Concorrente Utilizados

O programa utiliza a classe std::thread, introduzida no C++11 e consolidada nas versões posteriores como parte fundamental do suporte nativo à programação concorrente. Os principais recursos demonstrados são:

- Criação de threads: Cada instância de std::thread representa uma thread de execução. O construtor da classe recebe qualquer entidade que possa ser chamada como uma função (função, objeto com operador (), lambda).
- Execução concorrente: As três threads são iniciadas em paralelo ao fluxo principal do programa, demonstrando execução concorrente.
- Sincronização via join(): O método join() bloqueia a thread chamadora até que a thread associada termine sua execução, assegurando que o programa principal espere o término de todas as threads antes de encerrar.

Este exemplo serve como introdução ao modelo de threading nativo do C++, enfatizando a flexibilidade na forma de definir a função de entrada de uma thread e a necessidade de gerenciar sua finalização explicitamente.
*/
 

#include <iostream>
#include <thread>

void foo() {
  std::cout << "Executando na thread com função foo." << std::endl;
}

class Functor {
  public:
    void operator()() {
      std::cout << "Executando na thread com functor." << std::endl;
    }
};

int main() {
  Functor f;
  std::thread t1(foo); // Criação e execução imediata com função
  std::thread t2(f); // Criação e execução imediata com functor
  std::thread t3([]() { // Criação e execução imediata com lambda
    std::cout << "Executando na thread com lambda." << std::endl;
  });

  // ...

  t1.join(); // Aguarda o término da thread t1
  t2.join(); // Aguarda o término da thread t2
  t3.join(); // Aguarda o término da thread t3

  return 0;
}
