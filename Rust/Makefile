dirs := $(wildcard */)

.PHONY: all build run clean \
	run-hello run-vida run-prodcons run-fibo run-conta run-parallel_sum\
	run-all

build:
	@for d in $(dirs); do \
		echo "Building $$d..."; \
		cargo build --release --manifest-path $$d/Cargo.toml; \
	done

run-hello:
	@echo "Running hello_word with no parameters"
	@cargo run --release --manifest-path hello_word/Cargo.toml

run-vida:
	@echo "Running jogo_da_vida with parameters: 100 10 10"
	@cargo run --release --manifest-path jogo_da_vida/Cargo.toml -- 100 10 10

run-prodcons:
	@echo "Running produtor_consumidor_secao_critica with parameters: 100 2 2"
	@cargo run --release --manifest-path produtor_consumidor_secao_critica/Cargo.toml -- 100 2 2
	@echo "Running produtor_consumidor_canal_unico with parameters: 100 2 2"
	@cargo run --release --manifest-path produtor_consumidor_canal_unico/Cargo.toml -- 100 2 2

run-fibo:
	@echo "Running fibonacci_recursivo with parameters: 20 10"
	@cargo run --release --manifest-path fibonacci_recursivo/Cargo.toml -- 20 10
	@echo "Running fibonacci_rayon with parameters: 20 10"
	@cargo run --release --manifest-path fibonacci_rayon/Cargo.toml -- 20 10

run-conta:
	@echo "Running conta_palavras with parameters: anahy.txt 5"
	@cargo run --release --manifest-path conta_palavras/Cargo.toml -- ../files/anahy.txt 5

run-parallel_sum:
	@echo "Running parallel_sum"
	@cargo run --release --manifest-path parallel_sum/Cargo.toml

run: run-hello run-vida run-prodcons run-fibo run-conta run-parallel_sum

clean:
	@for d in $(dirs); do \
		echo "Cleaning $$d..."; \
		cargo clean --manifest-path $$d/Cargo.toml; \
	done

