/*--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa implementa o problema clássico do Produtor-Consumidor em Go utilizando um canal com buffer e exclusão mútua explícita para controlar contadores de progresso.

Produtores geram números primos e os enviam para um canal com capacidade limitada. Consumidores retiram esses números e realizam um processamento sintético sobre cada item. A produção e o consumo terminam assim que um número total de primos (`N`) é alcançado. A coordenação entre threads é feita com goroutines, canais e mutexes.

Execução com Go
go run produtor_consumidor_secao_critica.go <N> <produtores> <consumidores>

Exemplo:
go run produtor_consumidor_secao_critica.go 10 2 2

Saída esperada (ordem pode variar):
Produtor 1 produziu: 2
Consumidor 1 consumiu: 2
Produtor 2 produziu: 3
Consumidor 2 consumiu: 3
...
Todos os produtores e consumidores finalizaram.

Recursos de Programação Concorrente Utilizados

- **Goroutines**: criação de produtores e consumidores concorrentes.
- **Canal com buffer (`chan int`)**: transporte intermediário dos dados entre produtores e consumidores.
- **`sync.Mutex`**: proteção de variáveis compartilhadas (`primesGenerated` e `primesConsumed`) contra condições de corrida.
- **`sync.WaitGroup`**: espera pela finalização de todas as goroutines.

Este programa demonstra como implementar exclusão mútua e comunicação entre threads em Go utilizando os mecanismos nativos da linguagem.
*/

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

