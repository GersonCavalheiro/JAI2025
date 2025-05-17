defmodule ProdutorConsumidor do
  def eh_primo(n) when n < 2, do: false
  def eh_primo(2), do: true
  def eh_primo(n) do
    Enum.all?(2..(trunc(:math.sqrt(n))), fn i -> rem(n, i) != 0 end)
  end

  def gerar_primos(n), do: gerar_primos(n, 2, [])

  defp gerar_primos(0, _atual, primos), do: Enum.reverse(primos)
  defp gerar_primos(n, atual, primos) do
    if eh_primo(atual) do
      gerar_primos(n - 1, atual + 1, [atual | primos])
    else
      gerar_primos(n, atual + 1, primos)
    end
  end

  def produtor(id, primos, buffer_pid) do
    Enum.each(primos, fn p ->
      IO.puts("Produtor #{id} produziu item #{p}")
      send(buffer_pid, {:item, p})
    end)
  end

  def consumidor(id) do
    receive do
      {:item, valor} when valor < 0 ->
        IO.puts("Consumidor #{id} recebeu sinal de término.")
        :ok

      {:item, valor} ->
        IO.puts("Consumidor #{id} consumiu #{valor}")
        consumidor(id)
    end
  end

  def distribuidor(nc) do
    # Cria consumidores e coleta seus PIDs
    consumidores =
      for i <- 1..nc do
        spawn(fn -> consumidor(i) end)
      end

    loop(consumidores)
  end

  defp loop(consumidores) do
    receive do
      {:item, valor} ->
        # Distribui round-robin
        [dest | resto] = consumidores
        send(dest, {:item, valor})
        loop(resto ++ [dest])
    end
  end

  def main(args) do
    case args do
      [n_str, np_str, nc_str] ->
        {n, _} = Integer.parse(n_str)
        {np, _} = Integer.parse(np_str)
        {nc, _} = Integer.parse(nc_str)

        primos = gerar_primos(n)

        # Inicia o distribuidor (buffer)
        buffer_pid = spawn(fn -> distribuidor(nc) end)

        # Inicia produtores
        produtores =
          for i <- 1..np do
            spawn(fn -> produtor(i, primos, buffer_pid) end)
          end

        # Aguarda todos os produtores terminarem
        Enum.each(produtores, fn pid -> Process.monitor(pid) end)

        # Aguarda a finalização
        wait_for_produtores(length(produtores))

        # Envia sinais de término para os consumidores
        for _ <- 1..nc do
          send(buffer_pid, {:item, -1})
        end

      _ ->
        IO.puts("Uso: elixir produtor_consumidor.exs <N> <num_produtores> <num_consumidores>")
    end
  end

  defp wait_for_produtores(0), do: :ok
  defp wait_for_produtores(n) do
    receive do
      {:DOWN, _ref, :process, _pid, _reason} ->
        wait_for_produtores(n - 1)
    end
  end
end

ProdutorConsumidor.main(System.argv())

