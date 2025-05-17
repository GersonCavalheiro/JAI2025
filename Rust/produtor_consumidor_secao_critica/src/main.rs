use std::env;
use std::sync::{Arc, Condvar, Mutex};
use std::thread::{self};

fn eh_primo(n: u32) -> bool {
    if n <= 1 {
        return false;
    }
    for i in 2..=((n as f64).sqrt() as u32) {
        if n % i == 0 {
            return false;
        }
    }
    true
}

fn gerar_primos(n: usize) -> Vec<u32> {
    let mut primos = Vec::new();
    let mut candidato = 2;
    while primos.len() < n {
        if eh_primo(candidato) {
            primos.push(candidato);
        }
        candidato += 1;
    }
    primos
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        println!("Uso: {} <n_primos> <n_produtores> <n_consumidores>", args[0]);
        return;
    }

    let n_primos: usize = args[1].parse().unwrap();
    let n_produtores: usize = args[2].parse().unwrap();
    let n_consumidores: usize = args[3].parse().unwrap();

    let primos = Arc::new(gerar_primos(n_primos));
    let buffer = Arc::new((Mutex::new(Vec::<u32>::new()), Condvar::new()));

    let mut prod_handles = Vec::new();
    for i in 0..n_produtores {
        let primos = Arc::clone(&primos);
        let buffer = Arc::clone(&buffer);
        let h = thread::spawn(move || {
            for &p in primos.iter() {
                let (lock, cvar) = &*buffer;
                let mut buf = lock.lock().unwrap();
                buf.push(p);
                println!("Produtor {} produziu item {}", i + 1, p);
                cvar.notify_one();
            }
            println!("Produtor {} concluiu.", i + 1);
        });
        prod_handles.push(h);
    }

    let mut cons_handles = Vec::new();
    for i in 0..n_consumidores {
        let buffer = Arc::clone(&buffer);
        let h = thread::spawn(move || {
            loop {
                let (lock, cvar) = &*buffer;
                let mut buf = lock.lock().unwrap();
                while buf.is_empty() {
                    buf = cvar.wait(buf).unwrap();
                }
                let v = buf.remove(0);
                if v == u32::MAX {
                    break;
                }
                println!("Consumidor {} consumiu {}", i + 1, v);
            }
            println!("Consumidor {} concluiu.", i + 1);
        });
        cons_handles.push(h);
    }

    for h in prod_handles {
        h.join().unwrap();
    }

    let (lock, cvar) = &*buffer;
    let mut buf = lock.lock().unwrap();
    for _ in 0..n_consumidores {
        buf.push(u32::MAX);
    }
    cvar.notify_all();
    drop(buf);

    for h in cons_handles {
        h.join().unwrap();
    }
}

