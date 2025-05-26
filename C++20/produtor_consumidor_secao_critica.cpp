/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, implementa o problema Produtor-Consumidor com uso explícito de mutex e variável de condição para sincronização de acesso ao buffer. Cada thread produtora gera números primos e os insere em uma fila compartilhada, enquanto threads consumidoras removem e processam os valores. O término dos consumidores é sinalizado por meio de valores sentinela.

Parâmetros de Lançamento

O programa recebe três argumentos obrigatórios:

1. `total`: quantidade de números primos a serem produzidos por cada produtor.
2. `num_produtores`: número de threads produtoras.
3. `num_consumidores`: número de threads consumidoras.

Exemplo de uso:
./produtor_consumidor_secao_critica 5 2 2

Esse comando cria 2 produtores, cada um gerando 5 números primos, e 2 consumidores.

Recursos de Programação Concorrente Utilizados

- **`std::thread`**: criação explícita de threads produtoras e consumidoras.
- **`std::mutex`**: proteção do acesso ao buffer compartilhado (`std::queue<int>`).
- **`std::condition_variable`**: sincronização entre produtores e consumidores via espera ativa/passiva.
- **Controle de término com valor sentinela**: após a finalização de todos os produtores, a thread principal insere um número negativo (-1) no buffer para cada consumidor. Ao receber esse valor, os consumidores encerram sua execução.

O programa demonstra um modelo clássico de concorrência baseado em exclusão mútua e sincronização explícita por condição, utilizando estruturas de baixo nível da biblioteca padrão de C++.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <cmath>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> buffer;

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i <= std::sqrt(n); ++i)
        if (n % i == 0) return false;
    return true;
}

void produtor(int id, int total) {
    int count = 0;
    int num = 2;
    while (count < total) {
        if (is_prime(num)) {
            std::unique_lock<std::mutex> lock(mtx);
            buffer.push(num);
            std::cout << "Produtor " << id << " produziu item " << num << std::endl;
            cv.notify_one();
            count++;
        }
        num++;
    }
    std::cout << "Produtor " << id << " concluiu." << std::endl;
}

void consumidor(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty(); });
        int val = buffer.front();
        buffer.pop();
        if (val < 0) {
            buffer.push(val);
            cv.notify_all();
            break;
        }
        std::cout << "Consumidor " << id << " consumiu item " << val << std::endl;
    }
    std::cout << "Consumidor " << id << " concluiu." << std::endl;
}

int main(int argc, char* argv[]) {
    int total = std::stoi(argv[1]);
    int num_produtores = std::stoi(argv[2]);
    int num_consumidores = std::stoi(argv[3]);

    std::vector<std::thread> produtores;
    for (int i = 0; i < num_produtores; ++i)
        produtores.emplace_back(produtor, i + 1, total);

    std::vector<std::thread> consumidores;
    for (int i = 0; i < num_consumidores; ++i)
        consumidores.emplace_back(consumidor, i + 1);

    for (auto& p : produtores) p.join();

    {
        std::unique_lock<std::mutex> lock(mtx);
        for (int i = 0; i < num_consumidores; ++i)
            buffer.push(-1);
        cv.notify_all();
    }

    for (auto& c : consumidores) c.join();

    return 0;
}

