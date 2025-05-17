package main

import (
	"fmt"
	"sync"
)

func f(nome string, wg *sync.WaitGroup) {
	defer wg.Done() // Decrementa o contador quando a função termina
	fmt.Println("Executando:", nome)
}

func main() {
	var wg sync.WaitGroup

	wg.Add(2) // Serão sincronizadas 2 coroutines

	go f("goroutine 1", &wg)
	go func() {
		defer wg.Done()
		fmt.Println("Executando: goroutine 2")
	}()

	// ...

	wg.Wait() // Espera até que o contador seja zero
	fmt.Println("Função main retornou.")
}
