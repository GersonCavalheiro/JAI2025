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

