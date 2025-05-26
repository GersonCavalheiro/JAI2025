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

