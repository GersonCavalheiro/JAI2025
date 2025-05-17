use std::sync::{Arc, Mutex, mpsc};
use std::thread;

struct Item {
    produtor_id: usize,
    valor: usize,
}

fn eh_primo(n: usize) -> bool {
    if n <= 1 {
        return false;
    }
    if n <= 3 {
        return true;
    }
    if n % 2 == 0 || n % 3 == 0 {
        return false;
    }
    let mut i = 5;
    while i * i <= n {
        if n % i == 0 || n % (i + 2) == 0 {
            return false;
        }
        i += 6;
    }
    true
}

fn gerar_primos(n: usize) -> Vec<usize> {
    let mut primos = Vec::new();
    let mut atual = 2;
    while primos.len() < n {
        if eh_primo(atual) {
            primos.push(atual);
        }
        atual += 1;
    }
    primos
}

fn produtor(id: usize, primos: Vec<usize>, tx: mpsc::Sender<Item>) {
    for p in primos {
        println!("Produtor {} produziu item {}", id, p);
        tx.send(Item { produtor_id: id, valor: p }).unwrap();
    }
    println!("Produtor {} concluiu", id);
}

fn consumidor(id: usize, rx: Arc<Mutex<mpsc::Receiver<Item>>>) {
    loop {
        let item = {
            let rx = rx.lock().unwrap();
            rx.recv()
        };

        match item {
            Ok(Item { produtor_id, valor }) => {
                println!("Consumidor {} consumiu item {}", id, valor);
            }
            Err(_) => {
                println!("Consumidor {} concluiu", id);
                break;
            }
        }
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 4 {
        eprintln!("Uso: {} <N> <produtores> <consumidores>", args[0]);
        return;
    }

    let n: usize = args[1].parse().unwrap();
    let prod_n: usize = args[2].parse().unwrap();
    let cons_n: usize = args[3].parse().unwrap();
    let primos = gerar_primos(n);

    let (tx, rx) = mpsc::channel();
    let rx = Arc::new(Mutex::new(rx));

    let mut handles = vec![];

    for i in 0..prod_n {
        let primos = primos.clone();
        let tx = tx.clone();
        handles.push(thread::spawn(move || produtor(i + 1, primos, tx)));
    }

    drop(tx);

    for i in 0..cons_n {
        let rx = Arc::clone(&rx);
        handles.push(thread::spawn(move || consumidor(i + 1, rx)));
    }

    for h in handles {
        h.join().unwrap();
    }
}

