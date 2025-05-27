/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em Rust, realiza a contagem de palavras em um arquivo de texto, desconsiderando pontuações e diferenciando palavras apenas por seus caracteres alfabéticos em minúsculas. Apenas palavras com tamanho mínimo definido pelo usuário são consideradas. A contagem é paralelizada utilizando a biblioteca `rayon`.

Execução com Cargo
cargo run --release -- <arquivo.txt> <tamanho_mínimo>

Por exemplo:
cargo run --release -- texto.txt 5

Recursos de Programação Concorrente Utilizados

- **`rayon::par_chunks`**: divisão paralela do vetor de palavras em blocos fixos.
- **`map(...)`**: contagem local de palavras em cada bloco.
- **`reduce(...)`**: fusão dos mapas locais em um único dicionário global de contagens.

Este exemplo ilustra o uso de paralelismo de dados com `rayon`, evitando explicitamente a criação manual de threads. O modelo é declarativo e orientado à composição funcional de operações sobre coleções, promovendo legibilidade e desempenho com baixo esforço de sincronização explícita.
*/

use rayon::prelude::*;
use std::collections::HashMap;
use std::env;
use std::fs;

fn limpar_pontuacao(texto: &str) -> String {
    texto
        .chars()
        .map(|c| {
            if c.is_ascii_punctuation() || c == '"' || c == '\'' {
                ' '
            } else {
                c.to_ascii_lowercase()
            }
        })
        .collect()
}

fn contar_palavras(parte: &[String]) -> HashMap<String, usize> {
    let mut mapa = HashMap::new();
    for palavra in parte {
        *mapa.entry(palavra.clone()).or_insert(0) += 1;
    }
    mapa
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("Uso: {} <arquivo.txt> <tamanho_mínimo>", args[0]);
        std::process::exit(1);
    }

    let caminho = &args[1];
    let tamanho_minimo: usize = args[2].parse().expect("Número inválido");

    let conteudo = fs::read_to_string(caminho).expect("Erro ao ler o arquivo");

    let texto_limpo = limpar_pontuacao(&conteudo);

    let palavras: Vec<String> = texto_limpo
        .split_whitespace()
        .filter(|p| p.len() >= tamanho_minimo)
        .map(|p| p.to_string())
        .collect();

    // Divide o vetor de palavras em blocos e processa com par_iter
    let resultado: HashMap<String, usize> = palavras
        .par_chunks(1000) // Tamanho arbitrário de chunk
        .map(contar_palavras)
        .reduce(HashMap::new, |mut acc, mapa| {
            for (k, v) in mapa {
                *acc.entry(k).or_insert(0) += v;
            }
            acc
        });

    for (palavra, contagem) in resultado {
        println!("{}: {}", palavra, contagem);
    }
}

