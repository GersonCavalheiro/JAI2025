/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa, escrito em Rust, apresenta um exemplo introdutório de criação e execução de uma thread. Um valor inteiro é transferido para a nova thread, que realiza o cálculo do quadrado desse valor. O resultado é retornado para a thread principal por meio do método `join()`.

Execução com Cargo

Este programa deve ser executado por meio do gerenciador de pacotes e build system padrão do Rust:
cargo run --release

Recursos de Programação Concorrente Utilizados

- **`std::thread::spawn`**: criação de uma nova thread.
- **Clausura com `move`**: captura de variável por valor, necessária para transferir propriedade de `x` para a thread.
- **`join()`**: espera pela conclusão da thread e coleta do resultado.

O programa ilustra o modelo básico de concorrência em Rust, no qual a comunicação entre threads ocorre via retorno de valores e o controle de propriedade das variáveis é gerenciado pelo sistema de tipos da linguagem.
*/
use std::thread;

fn main() {
    let x: i32 = 8;
    let r: i32;
    let t = thread::spawn(move || { x * x });
    r = t.join().unwrap();
    print!("{}\n", r);
}
