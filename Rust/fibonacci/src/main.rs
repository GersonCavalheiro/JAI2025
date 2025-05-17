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

