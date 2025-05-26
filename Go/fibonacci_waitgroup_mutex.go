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

