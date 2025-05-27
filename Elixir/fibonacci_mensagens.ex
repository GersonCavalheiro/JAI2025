defmodule Main do

@doc """
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa calcula o n-ésimo número de Fibonacci em Elixir usando concorrência baseada em processos leves (`spawn`) e troca explícita de mensagens. A criação de processos é limitada por um limiar (`threshold`), abaixo do qual a computação ocorre sequencialmente.

Funcionamento

- A função `Fib.compute/2` avalia `n`:
  - Se `n < threshold`, calcula `fib(n)` de forma sequencial com recursão tradicional.
  - Caso contrário, cria dois processos concorrentes para calcular `Fib(n-1)` e `Fib(n-2)`.
- Cada processo filho envia o resultado de sua computação de volta ao processo pai por meio de `send`.
- O processo pai aguarda os dois resultados usando `receive` e retorna a soma.
- A computação sequencial é feita com `fib_seq/1`, sem paralelismo.

Exemplo de execução:
$ elixir fibonacci_mensagens.ex 10 5
Fibonacci(10) = 55


- Concorrência com `spawn` para criação de processos leves.
- Comunicação entre processos com `send` e `receive`.
- Uso de um limiar (`threshold`) para evitar explosão no número de processos.
- Computação assíncrona sem uso de tarefas (`Task`) ou fluxos (`Flow`), explorando o modelo de processos do Erlang/Elixir.

Este programa demonstra como construir algoritmos recursivos concorrentes com controle explícito da criação de processos e sincronização por mensagens, evidenciando o modelo de ator adotado por Elixir.
"""

defmodule Fib do
  def compute(n, threshold) when n < threshold do
    fib_seq(n)
  end

  def compute(n, threshold) do
    parent = self()

    spawn(fn ->
      send(parent, {:fib1, compute(n - 1, threshold)})
    end)

    spawn(fn ->
      send(parent, {:fib2, compute(n - 2, threshold)})
    end)

    receive do
      {:fib1, res1} ->
        receive do
          {:fib2, res2} ->
            res1 + res2
        end
    end
  end

  defp fib_seq(n) when n <= 1, do: n
  defp fib_seq(n), do: fib_seq(n - 1) + fib_seq(n - 2)
end

  def main(args) do
    case args do
      [n_str, threshold_str] ->
        {n, _} = Integer.parse(n_str)
        {threshold, _} = Integer.parse(threshold_str)

        result = Fib.compute(n, threshold)
        IO.puts("Fibonacci(#{n}) = #{result}")

      _ ->
        IO.puts("Uso: elixir fibonacci.ex <n> <threshold>")
    end
  end
end

Main.main(System.argv())

