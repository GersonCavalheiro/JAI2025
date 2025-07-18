EX_FILES := $(wildcard *.ex)

.PHONY: run run-all build clean \
        run-hello run-vida run-prodcons run-fibo run-conta

build:
	@echo "Elixir não requer build prévio para scripts simples."

clean:
	@echo "Nada para limpar para arquivos .ex simples."
	@echo "Limpando arquivos do projeto parallel_sum (mix)..."
	@rm -rf parallel_sum/_build parallel_sum/deps parallel_sum/.elixir_ls

run: run-all

run-all: run-hello run-vida run-prodcons run-fibo run-conta run-parallel_sum

run-hello:
	@echo "===> hello_world"
	@elixir hello_world.ex

run-vida:
	@echo "===> jogo_da_vida 100 10 10"
	@elixir jogo_da_vida.ex 100 10 10

run-prodcons:
	@echo "===> produtor_consumidor_canal_unico 100 10 10"
	@elixir produtor_consumidor_canal_unico.ex 100 10 10
	@echo "===> produtor_consumidor_flow 100 10 10"
	@elixir produtor_consumidor_flow.ex 100 10 10
	@echo "===> produtor_consumidor_processo_distribuidor 100 10 10"
	@elixir produtor_consumidor_processo_distribuidor.ex 100 10 10

run-fibo:
	@echo "===> fibonacci_mensagens 30 10"
	@elixir fibonacci_mensagens.ex 30 10
	@echo "===> fibonacci_task 30 10"
	@elixir fibonacci_task.ex 30 10

run-conta:
	@echo "===> conta_palavras anahy.txt 1000"
	@elixir conta_palavras.ex ../files/anahy.txt 1000

run-parallel_sum:
	@echo "===> parallel_sum (com Flow)"
	@cd parallel_sum && mix deps.get && mix run parallel_sum.exs

