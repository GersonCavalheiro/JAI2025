/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, implementa o problema Produtor-Consumidor utilizando `std::promise` e `std::future` como mecanismo de sincronização para identificar o momento em que cada thread produtora termina sua execução. Cada produtor gera números primos e os insere em um buffer compartilhado, enquanto consumidores os processam até receberem um sinal de término.

Parâmetros de Lançamento

O programa recebe três argumentos obrigatórios:

1. `total`: quantidade de números primos a serem produzidos por cada produtor.
2. `num_produtores`: número de threads produtoras.
3. `num_consumidores`: número de threads consumidoras.

Exemplo de uso:
./produtor_consumidor_futures 5 2 2

Esse comando cria 2 produtores, cada um gerando 5 números primos, e 2 consumidores para processar os dados.

Recursos de Programação Concorrente Utilizados

- **`std::thread`**: criação de threads manuais para produtores e consumidores.
- **`std::promise` e `std::future`**: cada produtor recebe uma `std::promise<void>`, permitindo que a thread principal aguarde sua conclusão por meio do respectivo `future`.
- **`std::mutex`**: proteção de acesso ao buffer compartilhado (`std::queue<int> buffer`).
- **Sinalização de término**: após todos os produtores finalizarem (sincronizados via `future::wait()`), valores especiais (-1) são inseridos no buffer para indicar o encerramento das threads consumidoras.

Essa abordagem exemplifica uma técnica clássica de sincronização entre threads usando promessas e futuros, sem o uso de variáveis de condição ou cancelamento cooperativo. O controle de término dos consumidores é feito de forma explícita com um marcador de finalização no buffer.
*/

#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <queue>
#include <cmath>
#include <vector>

std::mutex mtx;
std::queue<int> buffer;

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i <= std::sqrt(n); ++i)
        if (n % i == 0) return false;
    return true;
}

void produtor(int id, int total, std::promise<void> prom) {
    int count = 0;
    int num = 2;
    while (count < total) {
        if (is_prime(num)) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                buffer.push(num);
                std::cout << "Produtor " << id << " produziu item " << num << std::endl;
            }
            count++;
        }
        num++;
    }
    std::cout << "Produtor " << id << " concluiu." << std::endl;
    prom.set_value();
}

void consumidor(int id) {
    while (true) {
        int val = -2;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!buffer.empty()) {
                val = buffer.front();
                buffer.pop();
            }
        }
        if (val == -1) break;
        if (val >= 0) {
            std::cout << "Consumidor " << id << " consumiu item " << val << std::endl;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    std::cout << "Consumidor " << id << " concluiu." << std::endl;
}

int main(int argc, char* argv[]) {
    int total = std::stoi(argv[1]);
    int num_produtores = std::stoi(argv[2]);
    int num_consumidores = std::stoi(argv[3]);

    std::vector<std::thread> produtores;
    std::vector<std::promise<void>> promessas(num_produtores);
    std::vector<std::future<void>> futuros;

    for (auto& p : promessas)
        futuros.push_back(p.get_future());

    for (int i = 0; i < num_produtores; ++i)
        produtores.emplace_back(produtor, i + 1, total, std::move(promessas[i]));

    std::vector<std::thread> consumidores;
    for (int i = 0; i < num_consumidores; ++i)
        consumidores.emplace_back(consumidor, i + 1);

    for (auto& f : futuros) f.wait();

    {
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < num_consumidores; ++i)
            buffer.push(-1);
    }

    for (auto& p : produtores) p.join();
    for (auto& c : consumidores) c.join();

    return 0;
}

