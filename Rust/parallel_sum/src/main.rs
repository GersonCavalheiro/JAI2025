/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread
: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Gerald
o H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atual
ização de Informática (JAI 2024) e se encontra disponível em https://github.com/Gers
onCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa escrito em Rust utiliza a biblioteca Rayon para realizar uma operação
 de paralelismo de dados: somar todos os inteiros de 1 a 1 milhão que sejam múltipl
os de 3 e maiores que 100.

A biblioteca Rayon permite transformar iteradores em iteradores paralelos com pouc
a modificação no código, mantendo uma interface funcional e segura.

Parâmetros de Lançamento

O programa não exige argumentos. Para compilar e executar:

cargo run --release

Recursos de Programação Concorrente Utilizados

- rayon::prelude::*: provê a extensão par_iter() para vetores e coleções.
- Iteradores paralelos: abstração segura e eficiente de paralelismo de dados.
- Soma com tipo i64 para evitar overflow de inteiros.

Este exemplo demonstra como expressar paralelismo de dados de forma idiomática e s
egura em Rust, com desempenho competitivo e código conciso.
*/

use rayon::prelude::*;

fn main() {
    let data: Vec<i64> = (1..=1_000_000).collect();
    let sum: i64 = data.par_iter()
        .filter(|&&x| x > 100 && x % 3 == 0)
        .sum();

    println!("Sum = {}", sum);
}

