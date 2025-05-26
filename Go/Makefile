# Detecta automaticamente os arquivos .go
go_files := $(wildcard *.go)
executables := $(go_files:.go=)

.PHONY: all build clean run run-all \
        run-hello run-vida run-prodcons run-fibo run-conta

all: build

build:
	@for file in $(go_files); do \
		echo "Compilando $$file..."; \
		go build $$file; \
	done

clean:
	@for exe in $(executables); do \
		rm -f $$exe; \
	done

run: run-all

run-all: run-hello run-vida run-prodcons run-fibo run-conta

run-hello:
	@echo "===> hello_word"
	@./hello_world

run-vida:
	@echo "===> jogo_da_vida 100 10 10"
	@./jogo_da_vida 100 10 10

run-prodcons:
	@echo "===> produtor_consumidor_canal_unico 10 2 2"
	@./produtor_consumidor_canal_unico 10 2 2
	@echo "===> produtor_consumidor_secao_critica 10 2 2"
	@./produtor_consumidor_secao_critica 10 2 2

run-fibo:
	@echo "===> fibonacci_canais 30 10"
	@./fibonacci_canais 30 10
	@echo "===> fibonacci_waitgroup_atomic 30 10"
	@./fibonacci_waitgroup_atomic 30 10
	@echo "===> fibonacci_waitgroup_mutex 30 10"
	@./fibonacci_waitgroup_mutex 30 10

run-conta:
	@echo "===> conta_palavras anahy.txt 1000"
	@./conta_palavras ../files/anahy.txt 1000

