#include <iostream>
#include <future>
#include <cstdlib>

unsigned long long fib(int n, int threshold) {
    if (n <= 1) return n;
    if (n < threshold) // recursivo direto para valores pequenos
        return fib(n - 1, threshold) + fib(n - 2, threshold);

    auto f1 = std::async(std::launch::async, fib, n - 1, threshold);
    auto f2 = std::async(std::launch::async, fib, n - 2, threshold);
    return f1.get() + f2.get();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <n> <limite_sequencial>\n";
        return 1;
    }

    int n = std::atoi(argv[1]);
    int threshold = std::atoi(argv[2]);

    std::cout << "Fibonacci(" << n << ") com limite " << threshold << " = "
              << fib(n, threshold) << '\n';
}

