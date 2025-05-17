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

