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

