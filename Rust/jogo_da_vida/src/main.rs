/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em Rust, implementa o Jogo da Vida de Conway com paralelismo explícito via `std::thread`. A simulação é dividida em quadrantes, e cada thread é responsável por evoluir seu próprio bloco de células durante um número fixo de iterações. Ao início e ao fim, o conteúdo local de cada thread é copiado para uma grade global protegida por `Mutex`.

Execução com Cargo
cargo run --release -- <dimensao> <divisoes> <iteracoes>

Por exemplo:
cargo run --release -- 20 2 10

Esse comando executa o jogo com um grid de 20×20, dividido em 4 quadrantes (2×2), por 10 iterações.

Recursos de Programação Concorrente Utilizados

- **`std::thread::spawn`**: criação de threads para processar cada quadrante de forma independente.
- **`Arc<Mutex<Grid>>`**: grades globais compartilhadas (estado inicial e final), com acesso sincronizado por `Mutex`.
- Cada thread gera seu estado local, executa a evolução independentemente e copia os dados de volta para a grade compartilhada.
- A comunicação entre threads é indireta, apenas por meio de regiões de memória compartilhadas com sincronização explícita.

Este exemplo demonstra um modelo de concorrência por divisão de dados (data parallelism) com barreiras implícitas (sincronização via `join`) e controle manual de acesso a recursos compartilhados.
*/

use rand::Rng;
use std::env;
use std::sync::{Arc, Mutex};
use std::thread;

type Grid = Vec<Vec<u8>>;

fn count_neighbors(grid: &Grid, y: usize, x: usize) -> u8 {
    let height = grid.len();
    let width = grid[0].len();
    let mut count = 0;
    for dy in [-1isize, 0, 1].iter() {
        for dx in [-1isize, 0, 1].iter() {
            if *dy == 0 && *dx == 0 {
                continue;
            }
            let ny = y as isize + dy;
            let nx = x as isize + dx;
            if ny >= 0 && ny < height as isize && nx >= 0 && nx < width as isize {
                count += grid[ny as usize][nx as usize];
            }
        }
    }
    count
}

fn step(current: &Grid) -> Grid {
    let height = current.len();
    let width = current[0].len();
    let mut next = vec![vec![0; width]; height];
    for y in 0..height {
        for x in 0..width {
            let neighbors = count_neighbors(current, y, x);
            next[y][x] = match (current[y][x], neighbors) {
                (1, 2) | (1, 3) => 1,
                (0, 3) => 1,
                _ => 0,
            };
        }
    }
    next
}

fn print_grid(grid: &Grid) {
    for row in grid {
        for &cell in row {
            print!("{}", if cell == 1 { 'O' } else { '.' });
        }
        println!();
    }
}

fn worker(
    _id: usize,
    row: usize,
    col: usize,
    block_size: usize,
    iterations: usize,
    result: Arc<Mutex<Grid>>,
) {
    let mut rng = rand::thread_rng();
    let mut local = vec![vec![0u8; block_size]; block_size];
    for row in &mut local {
        for cell in row.iter_mut() {
            *cell = rng.gen_range(0..=1);
        }
    }

    // Primeira cópia para o estado inicial
    {
        let mut res = result.lock().unwrap();
        for y in 0..block_size {
            for x in 0..block_size {
                res[row * block_size + y][col * block_size + x] = local[y][x];
            }
        }
    }

    // Evolução
    for _ in 0..iterations {
        local = step(&local);
    }

    // Segunda cópia para o estado final
    {
        let mut res = result.lock().unwrap();
        for y in 0..block_size {
            for x in 0..block_size {
                res[row * block_size + y][col * block_size + x] = local[y][x];
            }
        }
    }

    println!("Thread [{}][{}] finalizou", row, col);
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        eprintln!("Uso: {} <dimensao> <divisoes> <iteracoes>", args[0]);
        std::process::exit(1);
    }

    let n: usize = args[1].parse().expect("dimensao invalida");
    let d: usize = args[2].parse().expect("divisoes invalidas");
    let t: usize = args[3].parse().expect("iteracoes invalidas");

    assert!(n % d == 0);
    let block_size = n / d;

    let grid_inicio = Arc::new(Mutex::new(vec![vec![0u8; n]; n]));
    let grid_fim = Arc::new(Mutex::new(vec![vec![0u8; n]; n]));

    // Estado inicial
    let mut handles = vec![];
    for row in 0..d {
        for col in 0..d {
            let result = Arc::clone(&grid_inicio);
            handles.push(thread::spawn(move || {
                worker(row * d + col, row, col, block_size, 0, result);
            }));
        }
    }
    for h in handles {
        h.join().unwrap();
    }

    println!("Estado inicial:");
    print_grid(&grid_inicio.lock().unwrap());

    // Estado final
    let mut handles = vec![];
    for row in 0..d {
        for col in 0..d {
            let result = Arc::clone(&grid_fim);
            handles.push(thread::spawn(move || {
                worker(row * d + col, row, col, block_size, t, result);
            }));
        }
    }
    for h in handles {
        h.join().unwrap();
    }

    println!("Estado final apos {} iteracoes:", t);
    print_grid(&grid_fim.lock().unwrap());
}

