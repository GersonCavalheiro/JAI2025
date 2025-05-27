/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em Rust, calcula o n-ésimo número de Fibonacci utilizando avaliação paralela com a biblioteca `rayon`. O programa recebe dois parâmetros: o valor de `n` e um limite `limite_sequencial` a partir do qual a computação recursiva passa a ser feita de forma sequencial, reduzindo a sobrecarga causada pela criação excessiva de tarefas paralelas.

Execução com Cargo
cargo run --release -- <n> <limite_sequencial>

Por exemplo:
cargo run --release -- 40 20

Recursos de Programação Concorrente Utilizados

- **`rayon::join`**: divide recursivamente o problema em duas tarefas paralelas para os ramos da chamada de Fibonacci.
- A paralelização é controlada por um parâmetro que define até que profundidade da recursão o paralelismo será utilizado. Abaixo desse limite, a computação é feita sequencialmente para evitar overhead.

Este exemplo ilustra o uso do paralelismo funcional em Rust com `rayon`, permitindo fácil paralelização de algoritmos recursivos puros. É particularmente útil para algoritmos de "dividir-para-conquistar".
*/

use std::env;
use rayon::join;

fn fib(n: u32, threshold: u32) -> u64 {
    if n <= 1 {
        return n as u64;
    }

    if n < threshold {
        return fib(n - 1, threshold) + fib(n - 2, threshold);
    }

    let (a, b) = join(|| fib(n - 1, threshold), || fib(n - 2, threshold));
    a + b
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("Uso: {} <n> <limite_sequencial>", args[0]);
        std::process::exit(1);
    }

    let n: u32 = args[1].parse().expect("n inválido");
    let threshold: u32 = args[2].parse().expect("limite inválido");

    let resultado = fib(n, threshold);
    println!("Fibonacci({}) com limite {} = {}", n, threshold, resultado);
}

