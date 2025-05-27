/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Implementação concorrente do cálculo do número de Fibonacci em Go utilizando WaitGroup para sincronização e operações atômicas para atualização segura da variável compartilhada.

Este programa explora a concorrência com goroutines e proteção da integridade dos dados através da biblioteca sync/atomic, que oferece primitivas de operação atômica eficientes.

Parâmetros

- `n`: índice do número Fibonacci a calcular (inteiro não negativo).
- `threshold`: limite para alternar entre execução sequencial e concorrente, para evitar overhead excessivo com pequenas chamadas.

Uso
go run fibonacci_waitgroup_atomic.go <n> <threshold>

Exemplo:
go run fibonacci_waitgroup_atomic.go 10 5

Funcionamento

- Para `n` igual ou menor que 1, envia o valor diretamente usando operação atômica.
- Para `n` menor que `threshold`, calcula sequencialmente a série de Fibonacci.
- Para `n` maior ou igual a `threshold`, cria duas goroutines concorrentes para calcular `fib(n-1)` e `fib(n-2)`.
- Usa `atomic.AddUint64` para somar resultados de forma segura, evitando condições de corrida.
- Espera a conclusão das goroutines internas com WaitGroup antes de continuar.

Conceitos Aplicados

- Concurrency com goroutines em Go.
- Sincronização usando sync.WaitGroup para esperar o término de goroutines.
- Controle de concorrência granular pelo parâmetro `threshold`.
- Segurança na atualização da variável compartilhada usando operações atômicas (`sync/atomic`).
*/

package main

import (
	"fmt"
	"os"
	"strconv"
	"sync"
	"sync/atomic"
)

// Versão sequencial
func fibSeq(n int) uint64 {
	if n <= 1 {
		return uint64(n)
	}
	return fibSeq(n-1) + fibSeq(n-2)
}

// Versão concorrente usando WaitGroup e atomic
func fib(n, threshold int, res *uint64, wg *sync.WaitGroup) {
	defer wg.Done()

	if n <= 1 {
		atomic.AddUint64(res, uint64(n))
		return
	}
	if n < threshold {
		atomic.AddUint64(res, fibSeq(n))
		return
	}

	var r1, r2 uint64
	var wgInner sync.WaitGroup
	wgInner.Add(2)

	// Lança chamadas concorrentes recursivas
	go fib(n-1, threshold, &r1, &wgInner)
	go fib(n-2, threshold, &r2, &wgInner)

	wgInner.Wait()

	atomic.AddUint64(res, r1+r2)
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

	var resultado uint64
	var wg sync.WaitGroup

	wg.Add(1)
	go fib(n, threshold, &resultado, &wg)

	wg.Wait()

	fmt.Printf("Fibonacci(%d) = %d\n", n, resultado)
}

