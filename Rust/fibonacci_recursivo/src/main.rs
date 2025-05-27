/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em Rust, calcula o n-ésimo número de Fibonacci utilizando threads explícitas com `std::thread::spawn`. A cada chamada recursiva em que `n` é maior ou igual ao valor de `limite_sequencial`, duas novas threads são criadas para computar os dois ramos da recursão em paralelo. Abaixo desse limite, o cálculo é feito de forma sequencial.

Execução com Cargo
cargo run --release -- <n> <limite_sequencial>

Por exemplo:
cargo run --release -- 30 20

Recursos de Programação Concorrente Utilizados

- **`std::thread::spawn`**: criação manual de threads para computar em paralelo os ramos da recursão.
- O parâmetro `limite_sequencial` serve para limitar o paralelismo em profundidade, evitando a explosão no número de threads.
- As chamadas `join()` são usadas para sincronizar os resultados das threads filhas antes de retornar o valor.

Este exemplo destaca o controle explícito sobre a criação e junção de threads, permitindo ilustrar os custos de granularidade do paralelismo e a importância de limitar a profundidade do uso de threads para manter o desempenho e evitar sobrecarga no sistema.
*/

use std::env;
use std::thread;

fn fib(n: u32, threshold: u32) -> u64 {
    if n <= 1 {
        return n as u64;
    }
    if n < threshold {
        return fib(n - 1, threshold) + fib(n - 2, threshold);
    }

    let t1 = {
        let threshold = threshold;
        thread::spawn(move || fib(n - 1, threshold))
    };

    let t2 = {
        let threshold = threshold;
        thread::spawn(move || fib(n - 2, threshold))
    };

    let r1 = t1.join().unwrap();
    let r2 = t2.join().unwrap();
    r1 + r2
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

