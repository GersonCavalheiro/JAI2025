/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread
: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Gerald
o H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atual
ização de Informática (JAI 2024) e se encontra disponível em https://github.com/Gers
onCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa escrito em Go implementa uma versão manual de paralelismo de dados,
 utilizando goroutines e WaitGroup para somar todos os inteiros entre 1 e 1 milhão
 que sejam múltiplos de 3 e maiores que 100.

Os dados são particionados em fatias e processados em paralelo por múltiplas goroutines.
 Os resultados parciais são enviados a um canal e somados ao final.

Parâmetros de Lançamento

O programa não requer parâmetros. Para compilar e executar:

go run parallel_sum.go

Recursos de Programação Concorrente Utilizados

- Goroutines: unidades leves de execução concorrente.
- sync.WaitGroup: sincronização de múltiplas goroutines.
- Canais: comunicação segura entre goroutines.
- Particionamento manual de dados para simular paralelismo de dados.

Este exemplo demonstra como expressar paralelismo de dados em Go de forma explícita,
com controle sobre a divisão de tarefas e sincronização.
*/

package main

import (
    "fmt"
    "sync"
)

func main() {
    const N = 1_000_000
    const numWorkers = 8

    data := make([]int, N)
    for i := range data {
        data[i] = i + 1
    }

    // Canal para coleta dos resultados parciais
    results := make(chan int, numWorkers)

    var wg sync.WaitGroup
    chunkSize := N / numWorkers

    for i := 0; i < numWorkers; i++ {
        start := i * chunkSize
        end := start + chunkSize
        if i == numWorkers-1 {
            end = N // último worker pega o restante
        }

        wg.Add(1)
        go func(slice []int) {
            defer wg.Done()
            localSum := 0
            for _, x := range slice {
                if x > 100 && x%3 == 0 {
                    localSum += x
                }
            }
            results <- localSum
        }(data[start:end])
    }

    // Encerramento do canal após todas as goroutines terminarem
    go func() {
        wg.Wait()
        close(results)
    }()

    total := 0
    for partial := range results {
        total += partial
    }

    fmt.Println("Soma total:", total)
}

