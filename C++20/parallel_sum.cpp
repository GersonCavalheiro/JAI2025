/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread
: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Gerald
o H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atual
ização de Informática (JAI 2024) e se encontra disponível em https://github.com/Gers
onCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa escrito em C++20 utiliza a Parallel STL para demonstrar uma operação
 típica de paralelismo de dados. O objetivo é somar todos os inteiros entre 1 e 1 
milhão que sejam múltiplos de 3 e maiores que 100.

A Parallel STL permite aplicar transformações e reduções paralelas sobre coleções
 com apenas uma chamada de função, utilizando a política de execução std::execution::par.

Parâmetros de Lançamento

O programa não exige parâmetros de entrada. Para compilar e executar:

g++ -std=c++20 -O2 -pthread -o parallel_sum parallel_sum.cpp
./parallel_sum

Recursos de Programação Concorrente Utilizados

- std::execution::par: política de execução paralela introduzida na C++17 e formaliza
da na C++20.
- std::transform_reduce: operação combinada de transformação e redução paralela.
- Inferência de tipo com long long para evitar estouro de inteiros.

Este exemplo demonstra o modelo de paralelismo de dados aplicado à STL moderna, re
forçando sua aplicabilidade em algoritmos simples de alta performance.
*/

#include <iostream>
#include <vector>
#include <numeric>
#include <execution>

int main() {
    std::vector<long long> data(1'000'000);
    std::iota(data.begin(), data.end(), 1);

    long long result = std::transform_reduce(
        std::execution::par,
        data.begin(), data.end(),
        0LL,
        std::plus<>(),
        [](int x) { return (x > 100 && x % 3 == 0) ? x : 0; }
    );

    std::cout << "Sum = " << result << std::endl;

    return 0;
}

