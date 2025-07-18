CXX = g++
CXXFLAGS = -std=c++20 -Wall -pthread

# Lista automática de fontes e executáveis
SOURCES := $(wildcard *.cpp)
EXES := $(SOURCES:.cpp=)

# Grupos de programas por categoria
prodcons_progs := $(filter produtor_consumidor_%, $(EXES))
fibo_progs := $(filter fibonacci_%, $(EXES))
cancel_coop_progs := cancelamento_cooperativo
cancel_colab_progs := fibonacci_cancelamento_colaborativo
vida_progs := jogo_da_vida
conta_progs := conta_palavras_blocos
hello_progs := hello_world
parallel_sum_progs := parallel_sum

.PHONY: all clean run run_prodcons run_fibo run_cancel_coop run_cancel_colab run_vida run_hello run_conta run_parallel_sum

all: $(EXES)

clean:
	rm -f $(EXES)

run: run_hello run_vida run_prodcons run_fibo run_conta run_cancel_coop run_cancel_colab run_parallel_sum

run_hello: $(hello_progs)
	./$(hello_progs)

run_vida: $(vida_progs)
	./$(vida_progs) 100 10 10

run_prodcons: $(prodcons_progs)
	@for prog in $(prodcons_progs); do ./$$prog 20 2 2; done

run_fibo: $(fibo_progs)
	@for prog in $(fibo_progs); do ./$$prog 30 10; done

run_conta: $(conta_progs)
	./$(conta_progs) ../files/anahy.txt 100

run_cancel_coop: $(cancel_coop_progs)
	./$(cancel_coop_progs)

run_cancel_colab: $(cancel_colab_progs)
	./$(cancel_colab_progs) 30 10

run_parallel_sum: $(parallel_sum_progs)
	./$(parallel_sum_progs)

