/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Implementação concorrente do cálculo do número de Fibonacci em Go utilizando WaitGroup e mutex para sincronização de acesso a dados compartilhados.

O programa divide o cálculo recursivo de Fibonacci em chamadas concorrentes que são sincronizadas com WaitGroup. Um mutex protege o acesso à variável de resultado acumulado, garantindo que múltiplas goroutines não corrompam a soma parcial.

Parâmetros

- `n`: índice do número Fibonacci a calcular (0 ou maior).
- `threshold`: limite para optar entre cálculo sequencial e recursivo concorrente, evitando excesso de criação de goroutines.

Uso
go run fibonacci_waitgroup_mutex.go <n> <threshold>

Exemplo:
go run fibonacci_waitgroup_mutex.go 10 5

Funcionamento

- Para `n` menor ou igual a 1, retorna `n`.
- Para `n` menor que `threshold`, calcula sequencialmente para evitar overhead de concorrência.
- Para `n` maior ou igual a `threshold`, executa chamadas concorrentes para `fib(n-1)` e `fib(n-2)`.
- Usa mutex para garantir exclusão mútua na atualização da variável compartilhada `res`.
- Espera as goroutines internas com um WaitGroup local antes de atualizar o resultado.

Conceitos Aplicados

- Concurrency com goroutines.
- Sincronização com sync.WaitGroup.
- Proteção de seção crítica com sync.Mutex.
- Controle de paralelismo via threshold para otimizar desempenho.
*/

package main

import (
	"fmt"
	"os"
	"strconv"
	"sync"
)

// Versão sequencial
func fibSeq(n int) uint64 {
	if n <= 1 {
		return uint64(n)
	}
	return fibSeq(n-1) + fibSeq(n-2)
}

// Versão concorrente usando WaitGroup
func fib(n, threshold int, res *uint64, wg *sync.WaitGroup, mu *sync.Mutex) {
	defer wg.Done()

	if n <= 1 {
		mu.Lock()
		*res += uint64(n)
		mu.Unlock()
		return
	}
	if n < threshold {
		mu.Lock()
		*res += fibSeq(n)
		mu.Unlock()
		return
	}

	var r1, r2 uint64
	var wgInner sync.WaitGroup
	wgInner.Add(2)

	// Lança chamadas concorrentes recursivas
	go fib(n-1, threshold, &r1, &wgInner, mu)
	go fib(n-2, threshold, &r2, &wgInner, mu)

	// Espera os dois cálculos terminarem
	wgInner.Wait()

	mu.Lock()
	*res += r1 + r2
	mu.Unlock()
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
	var mu sync.Mutex

	wg.Add(1)
	go fib(n, threshold, &resultado, &wg, &mu)

	wg.Wait()

	fmt.Printf("Fibonacci(%d) = %d\n", n, resultado)
}

