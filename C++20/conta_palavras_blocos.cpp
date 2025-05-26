/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em C++20, realiza a contagem de palavras em um arquivo de texto utilizando paralelismo baseado em blocos. Ele divide o conjunto de palavras extraídas em partes proporcionais ao número de núcleos disponíveis na máquina, processando essas partes em paralelo para acelerar a contagem. Ao final, os resultados parciais são agregados em um único mapa contendo as palavras e suas respectivas frequências.

Parâmetros de Lançamento

O programa exige dois argumentos na linha de comando:

1. Caminho para um arquivo de texto.
2. Tamanho mínimo das palavras a serem consideradas.

Exemplo de execução:

./conta_palavras_bloco ../files/anahy.txt 4

Esse comando contará todas as palavras com pelo menos 4 caracteres no arquivo `arquivo.txt`.

Recursos de Programação Concorrente Utilizados

O programa faz uso do paralelismo de dados via algoritmo `std::for_each` com a política de execução `std::execution::par`, introduzida no C++17 e padronizada no C++20 para ambientes com suporte a execução paralela. Outros recursos relevantes:

- `std::thread::hardware_concurrency()`: determina o número de *cores* disponíveis, utilizado para definir o número de blocos.
- Divisão manual dos dados: o vetor de palavras é segmentado em intervalos para processamento independente.
- `std::unordered_map`: cada bloco utiliza um mapa separado para registrar suas contagens, evitando a necessidade de sincronização.
- Agregação sequencial: os mapas parciais são combinados em um único resultado final após o processamento paralelo.

Esse modelo de paralelismo é ideal para tarefas que envolvem grande volume de dados e podem ser divididas em partes independentes, com custo reduzido de sincronização entre threads. Ele demonstra como combinar programação funcional e concorrente com as ferramentas da biblioteca padrão do C++.
*/ 

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

std::string limpar_pontuacao(std::string&& texto) {
    for (char& c : texto) {
        if (std::ispunct(static_cast<unsigned char>(c)) || c == '"' || c == '\'')
            c = ' ';
        else
            c = std::tolower(static_cast<unsigned char>(c));
    }
    return std::move(texto);
}

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

    std::ostringstream oss;
    char c;
    while (arquivo.get(c)) {
        oss << c;
    }

    std::string texto = limpar_pontuacao(std::move(oss.str()));
    std::vector<std::string> palavras = separar_palavras(texto, tamanho_minimo);

    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t bloco = palavras.size() / num_threads;

    std::vector<std::pair<size_t, size_t>> blocos;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t inicio = i * bloco;
        size_t fim = (i == num_threads - 1) ? palavras.size() : inicio + bloco;
        blocos.emplace_back(inicio, fim);
    }

    std::vector<std::unordered_map<std::string, size_t>> mapas_parciais(blocos.size());

    std::for_each(std::execution::par, blocos.begin(), blocos.end(),
        [&](const std::pair<size_t, size_t>& intervalo) {
            size_t idx = &intervalo - &blocos[0]; // índice do bloco
            auto& mapa = mapas_parciais[idx];
            for (size_t i = intervalo.first; i < intervalo.second; ++i) {
                ++mapa[palavras[i]];
            }
        });

    std::unordered_map<std::string, size_t> resultado_final;
    for (const auto& mapa : mapas_parciais) {
        agregar_mapas(resultado_final, mapa);
    }

    for (const auto& [palavra, contagem] : resultado_final) {
        std::cout << palavra << ": " << contagem << '\n';
    }

    return 0;
}

