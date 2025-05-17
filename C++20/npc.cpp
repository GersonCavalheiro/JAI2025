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
const size_t BUFFER_SIZE = 10;
bool done = false;

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

void synthetic_calc(int n) {
    for (int i = 1; i <= n; ++i) {
        std::sqrt(i);
    }
}

void producer(int id, int n, int num_producers) {
    int num_generated = 0;
    int num = 2 + id - 1;  // Sequência intercalada como na versão Elixir

    while (num_generated < n / num_producers) {
        if (is_prime(num)) {
            std::unique_lock lock(mtx);
            cv.wait(lock, [] { return buffer.size() < BUFFER_SIZE; });
            buffer.push(num);
            std::cout << "Produtor " << id << " produziu: " << num << std::endl;
            ++num_generated;
            cv.notify_all();
        }
        num += num_producers;
    }
}

void consumer(int id, int n) {
    int num_consumed = 0;

    while (num_consumed < n) {
        std::unique_lock lock(mtx);
        cv.wait(lock, [] { return !buffer.empty() || done; });
        if (!buffer.empty()) {
            int item = buffer.front();
            buffer.pop();
            lock.unlock();

            synthetic_calc(item);
            std::cout << "Consumidor " << id << " consumiu: " << item << std::endl;
            ++num_consumed;

            cv.notify_all();
        } else if (done) {
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <n> <num_produtores> <num_consumidores>\n";
        return 1;
    }

    int n = std::stoi(argv[1]);
    int num_produtores = std::stoi(argv[2]);
    int num_consumidores = std::stoi(argv[3]);

    std::vector<std::thread> produtores;
    std::vector<std::thread> consumidores;

    for (int i = 1; i <= num_produtores; ++i) {
        produtores.emplace_back(producer, i, n, num_produtores);
    }

    for (int i = 1; i <= num_consumidores; ++i) {
        consumidores.emplace_back(consumer, i, n);
    }

    for (auto& t : produtores) {
        t.join();
    }

    {
        std::unique_lock lock(mtx);
        done = true;
        cv.notify_all();
    }

    for (auto& t : consumidores) {
        t.join();
    }

    std::cout << "Todos os produtores e consumidores finalizaram." << std::endl;
    return 0;
}

