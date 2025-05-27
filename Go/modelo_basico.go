/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa apresenta um modelo básico de criação e sincronização de goroutines em Go. Duas goroutines são iniciadas a partir da função `main` e sua finalização é aguardada por meio de um `sync.WaitGroup`.

A função `f` representa uma tarefa simples executada em paralelo. O `WaitGroup` é utilizado para coordenar a finalização das goroutines antes que a função `main` termine sua execução.

Execução com Go
go run modelo_basico.go

Saída esperada (ordem pode variar):
Executando: goroutine 1
Executando: goroutine 2
Função main retornou.

Recursos de Programação Concorrente Utilizados

- **`go`**: criação de goroutines, unidades leves de execução concorrente.
- **`sync.WaitGroup`**: mecanismo de sincronização para aguardar a conclusão de múltiplas goroutines.

O programa ilustra o uso elementar de goroutines e o controle de sua finalização, servindo como introdução ao modelo de concorrência leve oferecido por Go.
*/

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
