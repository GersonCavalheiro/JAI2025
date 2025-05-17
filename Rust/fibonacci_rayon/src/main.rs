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

