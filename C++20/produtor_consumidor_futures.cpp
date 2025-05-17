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
}

