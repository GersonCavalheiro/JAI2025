/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, implementa uma versão concorrente do Jogo da Vida de Conway, com a divisão do grid global em blocos distribuídos entre várias threads. Cada bloco evolui independentemente, e as threads responsáveis por sua atualização operam de forma paralela. A troca de fronteiras entre blocos ainda não está implementada neste exemplo, o que limita a precisão da simulação ao longo do tempo, mas ilustra a estrutura geral de divisão de trabalho.

Parâmetros de Lançamento

O programa recebe três argumentos obrigatórios:

1. `dimensao`: dimensão \( N \) do grid global, assumido como um quadrado \( N \times N \).
2. `divisoes`: número de divisões \( D \) em cada direção, totalizando \( D^2 \) blocos e threads.
3. `iteracoes`: número de iterações (gerações) do Jogo da Vida.

Exemplo de uso:
./jogo_da_vida 40 4 10

Esse comando divide um grid de 40x40 em 16 blocos de 10x10, cada um processado por uma thread, e executa 10 iterações do autômato celular.

Recursos de Programação Concorrente Utilizados

- **`std::thread`**: cada bloco do grid é manipulado por uma thread distinta.
- **Sincronização por `std::mutex`**: utilizada para escrita simultânea nas estruturas de saída (como o grid global impresso).
- **Estrutura `Mailbox` com `std::condition_variable`**: embora preparada para permitir troca de mensagens entre blocos (como fronteiras), ainda não está integrada ao cálculo da vizinhança.
- **Funções `print_block` e `print_grid`**: permitem consolidar os resultados parciais de cada bloco no grid global de maneira segura.

Esta implementação demonstra o uso básico de paralelismo em C++ com threads explícitas, organizadas de forma estruturada, com uma infraestrutura de comunicação entre blocos que pode ser estendida para suportar sincronização completa entre vizinhos. A ausência de troca de fronteiras significa que os blocos processam suas células com base apenas no seu conteúdo interno.

Essa versão serve como ponto de partida para versões mais completas com interação entre blocos ou modelos com sobreposição de fronteiras.
*/

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <tuple>
#include <memory>
#include <cassert>
#include <sstream>

using Grid = std::vector<std::vector<int>>;

struct Message {
    int from_id;
    Grid border_data;
    std::string direction; // "TOP", "BOTTOM", "LEFT", "RIGHT"
};

class Mailbox {
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<Message> queue;
public:
    Mailbox() = default;
    Mailbox(const Mailbox&) = delete;
    Mailbox& operator=(const Mailbox&) = delete;

    void send(const Message& msg) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(msg);
        }
        cv.notify_one();
    }

    Message receive() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return !queue.empty(); });
        Message msg = queue.front();
        queue.pop();
        return msg;
    }

    bool try_receive(Message& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        msg = queue.front();
        queue.pop();
        return true;
    }
};

int count_neighbors(const Grid& grid, int y, int x) {
    static const int dy[] = {-1,-1,-1, 0,0, 1,1,1};
    static const int dx[] = {-1, 0, 1,-1,1,-1,0,1};
    int count = 0;
    int H = grid.size();
    int W = grid[0].size();
    for (int d = 0; d < 8; ++d) {
        int ny = y + dy[d];
        int nx = x + dx[d];
        if (ny >= 0 && ny < H && nx >= 0 && nx < W)
            count += grid[ny][nx];
    }
    return count;
}

void step(Grid& current, Grid& next) {
    int H = current.size();
    int W = current[0].size();
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int n = count_neighbors(current, y, x);
            if (current[y][x] == 1)
                next[y][x] = (n == 2 || n == 3) ? 1 : 0;
            else
                next[y][x] = (n == 3) ? 1 : 0;
        }
    }
}

std::mutex print_mtx;

void print_block(const Grid& grid, int offset_y, int offset_x, Grid& global) {
    std::lock_guard<std::mutex> lock(print_mtx);
    for (size_t y = 0; y < grid.size(); ++y)
        for (size_t x = 0; x < grid[y].size(); ++x)
            global[offset_y + y][offset_x + x] = grid[y][x];
}

void print_grid(const Grid& grid) {
    for (const auto& row : grid) {
        for (int val : row)
            std::cout << (val ? 'O' : '.');
        std::cout << '\n';
    }
}

void worker(int id, int row_blocks, int col_blocks, int block_y, int block_x,
            int block_size, int iterations,
            std::vector<std::vector<std::shared_ptr<Mailbox>>>& mailboxes,
            Grid& global_grid) {
    Grid local(block_size, std::vector<int>(block_size));
    Grid next(block_size, std::vector<int>(block_size));

    std::mt19937 gen(id);
    std::uniform_int_distribution<> dist(0, 1);
    for (auto& row : local)
        for (auto& cell : row)
            cell = dist(gen);

    print_block(local, block_y * block_size, block_x * block_size, global_grid);

    for (int t = 0; t < iterations; ++t) {
        step(local, next);
        std::swap(local, next);
    }

    print_block(local, block_y * block_size, block_x * block_size, global_grid);
    std::lock_guard<std::mutex> lock(print_mtx);
    std::cout << "Thread [" << block_y << "," << block_x << "] finalizou\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <dimensao> <divisoes> <iteracoes>\n";
        return 1;
    }

    int N = std::stoi(argv[1]);
    int D = std::stoi(argv[2]);
    int T = std::stoi(argv[3]);

    assert(N % D == 0);
    int block_size = N / D;

    std::vector<std::vector<std::shared_ptr<Mailbox>>> mailboxes(D);
    for (int i = 0; i < D; ++i)
        for (int j = 0; j < D; ++j)
            mailboxes[i].push_back(std::make_shared<Mailbox>());

    Grid grid_inicio(N, std::vector<int>(N));
    Grid grid_fim(N, std::vector<int>(N));

    std::vector<std::thread> threads;
    for (int by = 0; by < D; ++by) {
        for (int bx = 0; bx < D; ++bx) {
            int id = by * D + bx;
            threads.emplace_back(worker, id, D, D, by, bx, block_size, 0, std::ref(mailboxes), std::ref(grid_inicio));
        }
    }
    for (auto& t : threads) t.join();

    std::cout << "Estado inicial:\n";
    print_grid(grid_inicio);

    threads.clear();
    for (int by = 0; by < D; ++by) {
        for (int bx = 0; bx < D; ++bx) {
            int id = by * D + bx;
            threads.emplace_back(worker, id, D, D, by, bx, block_size, T, std::ref(mailboxes), std::ref(grid_fim));
        }
    }
    for (auto& t : threads) t.join();

    std::cout << "Estado final apos " << T << " iteracoes:\n";
    print_grid(grid_fim);

    return 0;
}

