#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <execution>
#include <iterator>
#include <thread>

// Remove pontuação e converte para minúsculas
std::string limpar_pontuacao(std::string&& texto) {
    for (char& c : texto) {
        if (std::ispunct(static_cast<unsigned char>(c)) || c == '"' || c == '\'')
            c = ' ';
        else
            c = std::tolower(static_cast<unsigned char>(c));
    }
    return std::move(texto);
}
// Separa palavras e filtra pelo tamanho mínimo
std::vector<std::string> separar_palavras(const std::string& texto, size_t tamanho_minimo) {
    std::istringstream iss(texto);
    std::vector<std::string> palavras;
    std::string palavra;
    while (iss >> palavra) {
        if (palavra.size() >= tamanho_minimo)
            palavras.push_back(palavra);
    }
    return palavras;
}

// Agrega contagem de origem no destino
void agregar_mapas(std::unordered_map<std::string, size_t>& destino,
                   const std::unordered_map<std::string, size_t>& origem) {
    for (const auto& [palavra, contagem] : origem) {
        destino[palavra] += contagem;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.txt> <tamanho_minimo_palavra>\n";
        return 1;
    }

    std::ifstream arquivo(argv[1]);
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo.\n";
        return 1;
    }

    size_t tamanho_minimo = std::stoul(argv[2]);

    // Leitura caractere a caractere
    std::ostringstream oss;
    char c;
    while (arquivo.get(c)) {
        oss << c;
    }

    std::string texto = limpar_pontuacao(std::move(oss.str()));
    std::vector<std::string> palavras = separar_palavras(texto, tamanho_minimo);

    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t bloco = palavras.size() / num_threads;

    // Estrutura para os blocos
    std::vector<std::pair<size_t, size_t>> blocos;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t inicio = i * bloco;
        size_t fim = (i == num_threads - 1) ? palavras.size() : inicio + bloco;
        blocos.emplace_back(inicio, fim);
    }

    // Vetor de mapas parciais (um por bloco)
    std::vector<std::unordered_map<std::string, size_t>> mapas_parciais(blocos.size());

    // Processa os blocos em paralelo
    std::for_each(std::execution::par, blocos.begin(), blocos.end(),
        [&](const std::pair<size_t, size_t>& intervalo) {
            size_t idx = &intervalo - &blocos[0]; // índice do bloco
            auto& mapa = mapas_parciais[idx];
            for (size_t i = intervalo.first; i < intervalo.second; ++i) {
                ++mapa[palavras[i]];
            }
        });

    // Agrega resultados
    std::unordered_map<std::string, size_t> resultado_final;
    for (const auto& mapa : mapas_parciais) {
        agregar_mapas(resultado_final, mapa);
    }

    // Exibe o resultado
    for (const auto& [palavra, contagem] : resultado_final) {
        std::cout << palavra << ": " << contagem << '\n';
    }

    return 0;
}

