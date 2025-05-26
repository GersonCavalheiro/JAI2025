/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, calcula o n-ésimo número de Fibonacci de forma paralela utilizando um pool de threads baseado em `std::jthread`, com cancelamento cooperativo. O programa ilustra a combinação de programação assíncrona com controle explícito de tarefas por meio de uma fila segura e sincronização por variáveis de condição.

Parâmetros de Lançamento

O programa exige dois argumentos:

1. `n`: a posição da sequência de Fibonacci a ser calculada.
2. `limite_sequencial`: valor abaixo do qual o cálculo recursivo é feito de forma sequencial, evitando overhead com criação de tarefas.

Exemplo de uso:
./fibonacci_cancelamento_colaborativo 35 10


Esse comando calcula o 35º número de Fibonacci, realizando chamadas recursivas paralelas para valores iguais ou superiores a 10.

Recursos de Programação Concorrente Utilizados

O programa utiliza uma implementação personalizada de um pool de threads para distribuir as tarefas de cálculo. Os principais recursos concorrentes explorados são:

- **`std::jthread`**: usado para criar as threads do pool, com suporte nativo a cancelamento via `std::stop_token`.
- **`std::stop_token`**: permite verificar se uma thread deve ser finalizada de forma cooperativa.
- **`std::condition_variable` e `std::mutex`**: empregados para a sincronização da fila de tarefas e para aguardar o resultado final.
- **Fila de tarefas protegida**: tarefas são representadas por objetos `std::function<void()>` enfileirados e processados pelas threads do pool.
- **Funções lambda com captura por valor compartilhado (`std::shared_ptr`)**: permitem sincronizar resultados parciais e invocar o callback final após a conclusão de ambas as sub-tarefas de Fibonacci.

O cancelamento cooperativo é possível graças ao uso de `std::jthread` e `stop_requested()`, que permite que as threads verifiquem se devem encerrar sua execução de forma segura. Essa abordagem evita interrupções forçadas e favorece um encerramento limpo, especialmente em estruturas com loops ou filas.

Este programa ilustra um modelo de paralelismo mais controlado e extensível em C++20, adequado para workloads recursivos com alto grau de paralelismo.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <atomic>

class ThreadPool {
    std::vector<std::jthread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;

public:
    ThreadPool(size_t n) {
        for (size_t i = 0; i < n; ++i) {
            threads.emplace_back([this](std::stop_token st) {
                while (!st.stop_requested()) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(mtx);
                        cv.wait(lock, [this] { return !tasks.empty() || done; });
                        if (done && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard lock(mtx);
            done = true;
        }
        cv.notify_all();
    }

    void enqueue(std::function<void()> f) {
        {
            std::lock_guard lock(mtx);
            tasks.push(std::move(f));
        }
        cv.notify_one();
    }
};

std::atomic<unsigned long long> result = 0;

unsigned long long fib_seq(int n) {
    if (n <= 1) return n;
    return fib_seq(n - 1) + fib_seq(n - 2);
}

void fib_parallel(ThreadPool& pool, int n, int threshold, std::function<void(unsigned long long)> callback) {
    if (n <= 1) {
        callback(n);
    } else if (n < threshold) {
        callback(fib_seq(n));
    } else {
        auto left = std::make_shared<unsigned long long>(0);
        auto right = std::make_shared<unsigned long long>(0);
        auto pending = std::make_shared<std::atomic<int>>(2);

        auto done = [=]() {
            if (--(*pending) == 0) {
                callback(*left + *right);
            }
        };

        pool.enqueue([=, &pool]() {
            fib_parallel(pool, n - 1, threshold, [=](unsigned long long res) {
                *left = res;
                done();
            });
        });

        pool.enqueue([=, &pool]() {
            fib_parallel(pool, n - 2, threshold, [=](unsigned long long res) {
                *right = res;
                done();
            });
        });
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <n> <limite_sequencial>\n";
        return 1;
    }

    int n = std::atoi(argv[1]);
    int threshold = std::atoi(argv[2]);

    ThreadPool pool(std::thread::hardware_concurrency());

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    unsigned long long resultado = 0;

    fib_parallel(pool, n, threshold, [&](unsigned long long res) {
        {
            std::lock_guard lock(mtx);
            resultado = res;
            done = true;
        }
        cv.notify_one();
    });

    {
        std::unique_lock lock(mtx);
        cv.wait(lock, [&] { return done; });
    }

    std::cout << "Fibonacci(" << n << ") = " << resultado << '\n';
    return 0;
}

