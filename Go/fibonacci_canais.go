/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Implementação concorrente do cálculo do número Fibonacci usando goroutines e canais em Go.

O cálculo é feito recursivamente, onde para valores de n maiores que um limite (threshold), duas goroutines são criadas para calcular fib(n-1) e fib(n-2) concorrentemente. Os resultados parciais são enviados via canais e somados para obter o resultado final. Para valores menores que threshold, é usado cálculo sequencial para evitar overhead excessivo.

Execução com Go
go run fibonacci_canais.go <n> <threshold>

Onde:
- `<n>` é o índice do número de Fibonacci desejado;
- `<threshold>` é o limite abaixo do qual a recursão passa a ser sequencial.

Exemplo:
go run fibonacci_canais.go 40 10

Saída esperada:
Fibonacci(40) = 102334155

Recursos de Programação Concorrente Utilizados

- **Goroutines**: execução paralela das chamadas recursivas para fib(n-1) e fib(n-2).
- **Canais (channels)**: comunicação e sincronização entre goroutines para agregação dos resultados.
- **Controle de recursão com threshold**: balanceamento entre paralelismo e custo de criação de goroutines.

Este programa exemplifica o uso de canais para sincronizar resultados concorrentes e demonstrar como dividir um problema recursivo em subtarefas paralelas em Go.
*/

package main

import (
	"fmt"
	"os"
	"strconv"
)

func fib(n, threshold int, ch chan<- uint64) {
	if n <= 1 {
		ch <- uint64(n)
		return
	}
	if n < threshold {
		ch <- fibSeq(n)
		return
	}

	ch1 := make(chan uint64)
	ch2 := make(chan uint64)

	go fib(n-1, threshold, ch1)
	go fib(n-2, threshold, ch2)

	res1 := <-ch1
	res2 := <-ch2
	ch <- res1 + res2
}

func fibSeq(n int) uint64 {
	if n <= 1 {
		return uint64(n)
	}
	return fibSeq(n-1) + fibSeq(n-2)
}

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Uso: go run fibonacci.go <n> <threshold>")
		return
	}

	n, err1 := strconv.Atoi(os.Args[1])
	threshold, err2 := strconv.Atoi(os.Args[2])

	if err1 != nil || err2 != nil || n < 0 || threshold < 0 {
		fmt.Println("Parâmetros inválidos. Ambos devem ser inteiros não negativos.")
		return
	}

	resultCh := make(chan uint64)
	go fib(n, threshold, resultCh)
	result := <-resultCh

	fmt.Printf("Fibonacci(%d) = %d\n", n, result)
}

