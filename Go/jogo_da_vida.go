package main

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"sync"
	"time"
)

type Grid [][]int

type Message struct {
	FromY, FromX int
	Data         Grid
}

func newGrid(n int) Grid {
	g := make(Grid, n)
	for i := range g {
		g[i] = make([]int, n)
	}
	return g
}

func randomFill(g Grid, seed int64) {
	r := rand.New(rand.NewSource(seed))
	for i := range g {
		for j := range g[i] {
			g[i][j] = r.Intn(2)
		}
	}
}

func printGrid(g Grid) {
	for _, row := range g {
		for _, val := range row {
			if val == 1 {
				fmt.Print("O")
			} else {
				fmt.Print(".")
			}
		}
		fmt.Println()
	}
}

func countNeighbors(g Grid, y, x int) int {
	dy := []int{-1, -1, -1, 0, 0, 1, 1, 1}
	dx := []int{-1, 0, 1, -1, 1, -1, 0, 1}
	count := 0
	h, w := len(g), len(g[0])
	for i := 0; i < 8; i++ {
		ny, nx := y+dy[i], x+dx[i]
		if ny >= 0 && ny < h && nx >= 0 && nx < w {
			count += g[ny][nx]
		}
	}
	return count
}

func step(current, next Grid) {
	h, w := len(current), len(current[0])
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			n := countNeighbors(current, y, x)
			if current[y][x] == 1 {
				next[y][x] = 0
				if n == 2 || n == 3 {
					next[y][x] = 1
				}
			} else {
				if n == 3 {
					next[y][x] = 1
				} else {
					next[y][x] = 0
				}
			}
		}
	}
}

func worker(id, by, bx, blockSize, steps int, wg *sync.WaitGroup, output chan<- [3]interface{}) {
	defer wg.Done()
	current := newGrid(blockSize)
	next := newGrid(blockSize)
	randomFill(current, time.Now().UnixNano()+int64(id))

	initial := newGrid(blockSize)
	for i := range current {
		copy(initial[i], current[i])
	}

	for t := 0; t < steps; t++ {
		step(current, next)
		current, next = next, current
	}
	output <- [3]interface{}{by, bx, current}
	fmt.Printf("Thread [%d,%d] finalizou\n", by, bx)
}

func mergeBlocks(blocks map[[2]int]Grid, blockSize, D int) Grid {
	N := blockSize * D
	global := newGrid(N)
	for by := 0; by < D; by++ {
		for bx := 0; bx < D; bx++ {
			b := blocks[[2]int{by, bx}]
			for y := 0; y < blockSize; y++ {
				for x := 0; x < blockSize; x++ {
					global[by*blockSize+y][bx*blockSize+x] = b[y][x]
				}
			}
		}
	}
	return global
}

func main() {
	if len(os.Args) != 4 {
		fmt.Println("Uso: <programa> <dimensao> <divisoes> <iteracoes>")
		return
	}
	N, _ := strconv.Atoi(os.Args[1])
	D, _ := strconv.Atoi(os.Args[2])
	T, _ := strconv.Atoi(os.Args[3])

	if N%D != 0 {
		fmt.Println("A dimensao deve ser divisivel pelo numero de divisoes")
		return
	}
	blockSize := N / D

	output := make(chan [3]interface{}, D*D)
	var wg sync.WaitGroup

	fmt.Println("Estado inicial:")
	gridInicio := newGrid(N)
	for by := 0; by < D; by++ {
		for bx := 0; bx < D; bx++ {
			id := by*D + bx
			wg.Add(1)
			go func(id, by, bx int) {
				defer wg.Done()
				current := newGrid(blockSize)
				randomFill(current, time.Now().UnixNano()+int64(id))
				for y := 0; y < blockSize; y++ {
					for x := 0; x < blockSize; x++ {
						gridInicio[by*blockSize+y][bx*blockSize+x] = current[y][x]
					}
				}
			}(id, by, bx)
		}
	}
	wg.Wait()
	printGrid(gridInicio)

	fmt.Println("Executando...\n")
	wg = sync.WaitGroup{}
	for by := 0; by < D; by++ {
		for bx := 0; bx < D; bx++ {
			id := by*D + bx
			wg.Add(1)
			go worker(id, by, bx, blockSize, T, &wg, output)
		}
	}
	wg.Wait()
	close(output)

	blocks := make(map[[2]int]Grid)
	for data := range output {
		by := data[0].(int)
		bx := data[1].(int)
		g := data[2].(Grid)
		blocks[[2]int{by, bx}] = g
	}

	fmt.Printf("\nEstado final apos %d iteracoes:\n", T)
	gridFim := mergeBlocks(blocks, blockSize, D)
	printGrid(gridFim)
}

