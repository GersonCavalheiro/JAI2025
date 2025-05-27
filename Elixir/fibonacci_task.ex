defmodule Fib do
@doc """
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa calcula o n-ésimo número de Fibonacci em Elixir utilizando o módulo `Task` para paralelizar chamadas recursivas. Um limiar (`threshold`) é usado para determinar até que ponto a computação será concorrente. Abaixo desse valor, a recursão ocorre de forma sequencial.

Funcionamento

- A função `Fib.compute/2` compara o valor de `n` com o limiar:
  - Se `n < threshold`, chama a função sequencial `fib_seq/1`.
  - Caso contrário, cria duas tarefas assíncronas com `Task.async/1`, que computam `Fib(n-1)` e `Fib(n-2)` em paralelo.
- Os resultados são recuperados com `Task.await/1` e somados.
- A função `fib_seq/1` é a versão clássica e recursiva do cálculo de Fibonacci.

Exemplo de execução:
$ elixir fibonacci_task.ex 10 5
Fibonacci(10) = 55

- Paralelismo com tarefas (`Task.async` e `Task.await`).
- Divisão do trabalho recursivo em subtarefas concorrentes.
- Uso de limiar para controle da profundidade de criação de tarefas.
- Comparação com abordagem baseada em `spawn` e `send/receive` (vide outro exemplo no material).

Este programa explora o uso de tarefas como abstração mais direta e conveniente para paralelismo estruturado em Elixir, mantendo controle explícito sobre o ponto de corte entre execução concorrente e sequencial.
"""

  def compute(n, threshold) when n < threshold do
    fib_seq(n)
  end

  def compute(n, threshold) do
    task1 = Task.async(fn -> compute(n - 1, threshold) end)
    task2 = Task.async(fn -> compute(n - 2, threshold) end)

    res1 = Task.await(task1)
    res2 = Task.await(task2)

    res1 + res2
  end

  defp fib_seq(n) when n <= 1, do: n
  defp fib_seq(n), do: fib_seq(n - 1) + fib_seq(n - 2)
end

defmodule Main do
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

