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

