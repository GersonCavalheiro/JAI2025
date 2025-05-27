/*
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa realiza a contagem de palavras em um arquivo texto, utilizando processamento concorrente com goroutines. O texto é dividido em blocos e processado paralelamente por múltiplas goroutines, cada uma responsável por contabilizar as ocorrências de palavras com um tamanho mínimo especificado.

A contagem final é obtida pela agregação dos mapas parciais produzidos por cada thread.

Execução com Go
go run conta_palavras.go <arquivo.txt> <tamanho_minimo>

Exemplo:
go run conta_palavras.go texto.txt 4

Saída esperada (exemplo com base no conteúdo de `texto.txt`):
tempo: 3
dados: 2
informacao: 1
...

Recursos de Programação Concorrente Utilizados

- **Goroutines**: execução paralela da função de contagem de palavras.
- **`sync.WaitGroup`**: sincronização do término das goroutines.
- **Canal (`chan map[string]int`)**: comunicação entre as goroutines e a função principal.
- **Divisão de trabalho estática**: o vetor de palavras é dividido uniformemente entre as goroutines.

Este programa ilustra o uso eficiente de paralelismo para análise textual em Go, com canais e goroutines sendo usados para distribuir e agregar o trabalho de forma controlada.
*/

package main

import (
    "bufio"
    "fmt"
    "os"
    "regexp"
    "strings"
    "sync"
)

func limparTexto(texto string) string {
    re := regexp.MustCompile(`[[:punct:]\"']`) // Modificação aqui para incluir as aspas
    return re.ReplaceAllStringFunc(texto, func(s string) string {
        return " "
    })
}

func contarPalavras(palavras []string, resultado chan map[string]int, tamanhoMin int, wg *sync.WaitGroup) {
    defer wg.Done()
    contagem := make(map[string]int)
    for _, palavra := range palavras {
        if len(palavra) >= tamanhoMin {
            palavra = strings.ToLower(palavra)
            contagem[palavra]++
        }
    }
    resultado <- contagem
}

func agregarMapas(mapas []map[string]int) map[string]int {
    resultado := make(map[string]int)
    for _, m := range mapas {
        for k, v := range m {
            resultado[k] += v
        }
    }
    return resultado
}

func main() {
    if len(os.Args) != 3 {
        fmt.Println("Uso: go run programa.go <arquivo.txt> <tamanho_minimo>")
        return
    }

    arquivo, err := os.Open(os.Args[1])
    if err != nil {
        fmt.Println("Erro ao abrir o arquivo:", err)
        return
    }
    defer arquivo.Close()

    tamanhoMin := 0
    fmt.Sscanf(os.Args[2], "%d", &tamanhoMin)

    scanner := bufio.NewScanner(arquivo)
    scanner.Split(bufio.ScanRunes)

    var texto strings.Builder
    for scanner.Scan() {
        texto.WriteString(scanner.Text())
    }

    textoLimpo := limparTexto(texto.String())
    palavras := strings.Fields(textoLimpo)

    numThreads := 4
    tamanhoBloco := len(palavras) / numThreads
    resultadoChan := make(chan map[string]int, numThreads)
    var wg sync.WaitGroup

    for i := 0; i < numThreads; i++ {
        inicio := i * tamanhoBloco
        fim := inicio + tamanhoBloco
        if i == numThreads-1 {
            fim = len(palavras)
        }
        wg.Add(1)
        go contarPalavras(palavras[inicio:fim], resultadoChan, tamanhoMin, &wg)
    }

    wg.Wait()
    close(resultadoChan)

    var mapasParciais []map[string]int
    for m := range resultadoChan {
        mapasParciais = append(mapasParciais, m)
    }

    resultadoFinal := agregarMapas(mapasParciais)

    for palavra, contagem := range resultadoFinal {
        fmt.Printf("%s: %d\n", palavra, contagem)
    }
}

