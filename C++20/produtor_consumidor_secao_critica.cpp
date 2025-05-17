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
}

