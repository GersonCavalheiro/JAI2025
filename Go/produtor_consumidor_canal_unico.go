package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"sync"
	"time"
)

type Item struct {
	produtor int
	valor    int
}

func ehPrimo(n int) bool {
	if n < 2 {
		return false
	}
	raiz := int(math.Sqrt(float64(n)))
	for i := 2; i <= raiz; i++ {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func produtor(id int, out chan<- Item, qtd int, wg *sync.WaitGroup) {
	defer wg.Done()
	num := 2
	enviados := 0
	for enviados < qtd {
		if ehPrimo(num) {
			out <- Item{produtor: id, valor: num}
			enviados++
		}
		num++
	}
}

func consumidor(id int, in <-chan Item, wg *sync.WaitGroup) {
	defer wg.Done()
	for item := range in {
		fmt.Printf("Consumidor %d recebeu %d do produtor %d\n", id, item.valor, item.produtor)
		time.Sleep(100 * time.Millisecond)
	}
}

func main() {
	if len(os.Args) != 4 {
		fmt.Printf("Uso: %s <num_produtores> <num_consumidores> <primos_por_produtor>\n", os.Args[0])
		return
	}

	numProdutores, err1 := strconv.Atoi(os.Args[1])
	numConsumidores, err2 := strconv.Atoi(os.Args[2])
	primosPorProdutor, err3 := strconv.Atoi(os.Args[3])

	if err1 != nil || err2 != nil || err3 != nil || numProdutores <= 0 || numConsumidores <= 0 || primosPorProdutor <= 0 {
		fmt.Println("Parâmetros inválidos. Todos devem ser inteiros positivos.")
		return
	}

	canal := make(chan Item)
	var wgProdutores sync.WaitGroup
	var wgConsumidores sync.WaitGroup

	for i := 0; i < numConsumidores; i++ {
		wgConsumidores.Add(1)
		go consumidor(i, canal, &wgConsumidores)
	}

	for i := 0; i < numProdutores; i++ {
		wgProdutores.Add(1)
		go produtor(i, canal, primosPorProdutor, &wgProdutores)
	}

	go func() {
		wgProdutores.Wait()
		close(canal)
	}()

	wgConsumidores.Wait()
}

