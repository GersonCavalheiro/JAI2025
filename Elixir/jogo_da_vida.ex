# jogo_da_vida.exs

defmodule Worker do
  def start(id, block, iterations, parent) do
    spawn(fn -> loop(id, block, iterations, parent) end)
  end

  defp loop(_id, block, 0, parent) do
    send(parent, {:done, block})
  end

  defp loop(id, block, iterations, parent) do
    next = step(block)
    loop(id, next, iterations - 1, parent)
  end

  defp step(grid) do
    for y <- 0..(length(grid) - 1) do
      for x <- 0..(length(List.first(grid)) - 1) do
        n = count_neighbors(grid, y, x)
        cell = Enum.at(Enum.at(grid, y), x)
        cond do
          cell == 1 and (n == 2 or n == 3) -> 1
          cell == 0 and n == 3 -> 1
          true -> 0
        end
      end
    end
  end

  defp count_neighbors(grid, y, x) do
    for dy <- -1..1, dx <- -1..1, dy != 0 or dx != 0, reduce: 0 do
      acc ->
        ny = y + dy
        nx = x + dx
        if ny in 0..(length(grid) - 1) and nx in 0..(length(List.first(grid)) - 1) do
          acc + Enum.at(Enum.at(grid, ny), nx)
        else
          acc
        end
    end
  end
end

defmodule GameOfLife do
  def run(n, d, t) do
    block_size = div(n, d)
    blocks = for _y <- 0..(d - 1), _x <- 0..(d - 1), do: random_block(block_size)
    
    IO.puts("Estado inicial:")
    print_blocks(blocks, d)

    pids = Enum.with_index(blocks)
           |> Enum.map(fn {block, id} ->
             Worker.start(id, block, t, self())
           end)

    final_blocks = collect_results([], length(pids))

    IO.puts("\nEstado final após #{t} iterações:")
    print_blocks(final_blocks, d)
  end

  defp random_block(size) do
    for _ <- 1..size do
      for _ <- 1..size, do: Enum.random([0, 1])
    end
  end

  defp collect_results(acc, 0), do: Enum.reverse(acc)
  defp collect_results(acc, n) do
    receive do
      {:done, block} -> collect_results([block | acc], n - 1)
    end
  end

  defp print_blocks(blocks, d) do
    rows = Enum.chunk_every(blocks, d)
    Enum.each(rows, fn row_blocks ->
      for i <- 0..(length(List.first(row_blocks)) - 1) do
        Enum.each(row_blocks, fn block ->
          IO.write(Enum.map_join(Enum.at(block, i), "", fn x -> if x == 1, do: "O", else: "." end))
          IO.write(" ")
        end)
        IO.puts("")
      end
      IO.puts("")
    end)
  end
end

# Execução do programa
# Parâmetros: dimensão do grid, divisões, iterações
[n_str, d_str, t_str] = System.argv()
{n, d, t} = {String.to_integer(n_str), String.to_integer(d_str), String.to_integer(t_str)}
GameOfLife.run(n, d, t)
