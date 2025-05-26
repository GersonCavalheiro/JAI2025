/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, calcula o n-ésimo número de Fibonacci utilizando programação concorrente com `std::async`. A estratégia empregada consiste em dividir a computação recursiva entre chamadas síncronas e assíncronas, dependendo de um limite inferior definido pelo usuário. A partir desse limiar, chamadas recursivas são executadas de forma concorrente em threads distintas.

Parâmetros de Lançamento

O programa requer dois argumentos de linha de comando:

1. `n`: o número de Fibonacci a ser calculado.
2. `limite_sequencial`: valor a partir do qual a execução recursiva passa a ser concorrente.

Exemplo de execução:
./fibonaccy_async 30 10

Esse comando calcula o trigésimo número de Fibonacci, utilizando chamadas concorrentes apenas a partir de `n = 10`.

Recursos de Programação Concorrente Utilizados

O programa utiliza `std::async` com a política explícita `std::launch::async`, que força a execução imediata da função em uma nova thread. A cada chamada concorrente, a função `fib()` é executada de forma paralela para os dois ramos da recursão. Os principais recursos utilizados são:

- `std::async`: cria uma tarefa assíncrona para computação em paralelo.
- `std::future`: armazena o resultado da tarefa assíncrona, permitindo a sincronização com `get()`.
- Controle de granularidade: o parâmetro `limite_sequencial` evita a criação excessiva de threads para chamadas pequenas, o que contribui para a eficiência do programa.

Esta implementação ilustra o uso de tarefas concorrentes para paralelizar uma computação intensiva e naturalmente recursiva, destacando o modelo de futuros como alternativa ao controle explícito de threads no C++.
*/


#include <iostream>
#include <future>
#include <cstdlib>

unsigned long long fib(int n, int threshold) {
    if (n <= 1) return n;
    if (n < threshold) // recursivo direto para valores pequenos
        return fib(n - 1, threshold) + fib(n - 2, threshold);

    auto f1 = std::async(std::launch::async, fib, n - 1, threshold);
    auto f2 = std::async(std::launch::async, fib, n - 2, threshold);
    return f1.get() + f2.get();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <n> <limite_sequencial>\n";
        return 1;
    }

    int n = std::atoi(argv[1]);
    int threshold = std::atoi(argv[2]);

    std::cout << "Fibonacci(" << n << ") com limite " << threshold << " = "
              << fib(n, threshold) << '\n';

    return 0;
}

