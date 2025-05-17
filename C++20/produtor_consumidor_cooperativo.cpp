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

    // Aguarda todos os produtores terminarem
    for (auto& p : produtores) {
        if (p.joinable())
            p.join();  // jthread também dá join() automaticamente no destrutor, mas aqui é explícito
    }

    // Solicita parada dos consumidores
    for (auto& c : consumidores) {
        c.request_stop();
    }

    // Notifica todos consumidores para saírem de wait
    cv.notify_all();

    // jthread faz join automaticamente
}

