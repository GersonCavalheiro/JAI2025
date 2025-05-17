package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"sync"
)

const BUFFER_SIZE = 10

func isPrime(n int) bool {
	if n <= 1 {
		return false
	}
	if n <= 3 {
		return true
	}
	if n%2 == 0 || n%3 == 0 {
		return false
	}
	for i := 5; i*i <= n; i += 6 {
		if n%i == 0 || n%(i+2) == 0 {
			return false
		}
	}
	return true
}

func producer(id, n int, tx chan int, wg *sync.WaitGroup, primesGenerated *int, mu *sync.Mutex) {
	defer wg.Done()
	num := 2
	for {
		mu.Lock()
		if *primesGenerated >= n {
			mu.Unlock()
			break
		}
		mu.Unlock()

		if isPrime(num) {
			mu.Lock()
			if *primesGenerated < n {
				tx <- num
				*primesGenerated++
				fmt.Printf("Produtor %d produziu: %d\n", id, num)
			}
			mu.Unlock()
		}
		num++
	}
}

func consumer(id, n int, rx chan int, wg *sync.WaitGroup, primesConsumed *int, mu *sync.Mutex) {
	defer wg.Done()
	for {
		mu.Lock()
		if *primesConsumed >= n {
			mu.Unlock()
			break
		}
		mu.Unlock()

		item := <-rx
		mu.Lock()
		*primesConsumed++
		mu.Unlock()

		// Cálculo sintético
		for i := 0; i < item; i++ {
			_ = math.Sqrt(float64(i))
		}

		fmt.Printf("Consumidor %d consumiu: %d\n", id, item)
	}
}

func main() {
	if len(os.Args) != 4 {
		fmt.Printf("Uso: %s <N> <num_produtores> <num_consumidores>\n", os.Args[0])
		os.Exit(1)
	}

	n, _ := strconv.Atoi(os.Args[1])
	numProdutores, _ := strconv.Atoi(os.Args[2])
	numConsumidores, _ := strconv.Atoi(os.Args[3])

	tx := make(chan int, BUFFER_SIZE)

	var primesGenerated, primesConsumed int
	var mu sync.Mutex

	var wg sync.WaitGroup

	for i := 0; i < numProdutores; i++ {
		wg.Add(1)
		go producer(i+1, n, tx, &wg, &primesGenerated, &mu)
	}

	for i := 0; i < numConsumidores; i++ {
		wg.Add(1)
		go consumer(i+1, n, tx, &wg, &primesConsumed, &mu)
	}

	wg.Wait()
	fmt.Println("Todos os produtores e consumidores finalizaram.")
}

