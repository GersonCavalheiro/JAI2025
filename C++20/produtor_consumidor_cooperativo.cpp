/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, implementa o problema clássico Produtor-Consumidor utilizando `std::jthread` e cancelamento cooperativo com `std::stop_token`. Produtores geram números primos, inserindo-os em um buffer compartilhado. Consumidores consomem os itens produzidos. O término dos consumidores é coordenado de forma cooperativa, após todos os produtores finalizarem sua tarefa.

Parâmetros de Lançamento

O programa recebe três argumentos obrigatórios:

1. `total`: quantidade de números primos a serem produzidos por cada produtor.
2. `num_produtores`: número de threads produtoras.
3. `num_consumidores`: número de threads consumidoras.

Exemplo de uso:
./produtor_consumidor_cooperativo 5 2 2


Esse comando cria 2 produtores, cada um gerando 5 números primos, e 2 consumidores para processar os dados.

Recursos de Programação Concorrente Utilizados

- **`std::jthread`**: threads com gerenciamento automático e suporte embutido a cancelamento cooperativo via `stop_token`.
- **`std::mutex` e `std::condition_variable`**: controle de acesso ao buffer compartilhado (`std::queue<int> buffer`) e sincronização entre produtores e consumidores.
- **Cancelamento cooperativo**:
  - Os consumidores verificam periodicamente `stop_requested()` e também são liberados de `cv.wait()` pela chamada a `cv.notify_all()` após o término dos produtores.
  - Os produtores verificam o `stop_token` dentro de seus laços principais.

Essa implementação demonstra um modelo moderno de cancelamento cooperativo em C++20 com `jthread`, simplificando a gerência de ciclo de vida das threads e oferecendo um exemplo robusto de comunicação entre threads com variável de condição.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <cmath>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> buffer;

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i <= std::sqrt(n); ++i)
        if (n % i == 0) return false;
    return true;
}

void produtor(std::stop_token st, int id, int total) {
    int count = 0;
    int num = 2;
    while (count < total && !st.stop_requested()) {
        if (is_prime(num)) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                buffer.push(num);
                std::cout << "Produtor " << id << " produziu item " << num << std::endl;
            }
            cv.notify_one();
            count++;
        }
        num++;
    }
    std::cout << "Produtor " << id << " concluiu." << std::endl;
}

void consumidor(std::stop_token st, int id) {
    while (!st.stop_requested()) {
        int val = -1;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !buffer.empty() || st.stop_requested(); });

            if (st.stop_requested()) break;

            val = buffer.front();
            buffer.pop();
        }
        std::cout << "Consumidor " << id << " consumiu item " << val << std::endl;
    }
    std::cout << "Consumidor " << id << " concluiu." << std::endl;
}

int main(int argc, char* argv[]) {
    int total = std::stoi(argv[1]);
    int num_produtores = std::stoi(argv[2]);
    int num_consumidores = std::stoi(argv[3]);

    std::vector<std::jthread> produtores;
    for (int i = 0; i < num_produtores; ++i)
        produtores.emplace_back(produtor, i + 1, total);

    std::vector<std::jthread> consumidores;
    for (int i = 0; i < num_consumidores; ++i)
        consumidores.emplace_back(consumidor, i + 1);

    for (auto& p : produtores) {
        if (p.joinable())
            p.join();  // jthread também dá join() automaticamente no destrutor, mas aqui é explícito
    }

    for (auto& c : consumidores) {
        c.request_stop();
    }

    cv.notify_all();

    return 0;// jthread faz join automaticamente
}

