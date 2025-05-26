defmodule Fib do
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

