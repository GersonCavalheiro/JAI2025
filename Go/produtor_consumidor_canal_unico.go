/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Implementação do problema produtor/consumidor em Go usando um único canal para comunicação.

Neste programa, múltiplos produtores geram números primos e enviam para um canal comum, enquanto múltiplos consumidores recebem e processam esses números. Cada produtor produz uma quantidade fixa de números primos. A sincronização e comunicação entre produtores e consumidores é feita exclusivamente por meio do canal compartilhado.

Execução com Go
go run produtor_consumidor_canal_unico.go <num_produtores> <num_consumidores> <primos_por_produtor>

Onde:
- `<num_produtores>` é a quantidade de goroutines produtoras;
- `<num_consumidores>` é a quantidade de goroutines consumidoras;
- `<primos_por_produtor>` é a quantidade de números primos que cada produtor deve gerar.

Exemplo:
go run produtor_consumidor_canal_unico.go 3 2 5

Saída típica:
Consumidor 0 recebeu 2 do produtor 1
Consumidor 1 recebeu 3 do produtor 0
...

Recursos de Programação Concorrente Utilizados

- **Goroutines** para produtores e consumidores concorrentes.
- **Canal único (channel)** para comunicação segura e sincronizada entre produtores e consumidores.
- **WaitGroup** para sincronização e garantia de término ordenado do programa.
- **Fechamento do canal** após todos os produtores finalizarem para sinalizar término aos consumidores.

Este programa demonstra a simplicidade e eficácia do uso de canais para troca de dados em Go, evitando necessidade de mutex ou variáveis de condição.
*/

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

