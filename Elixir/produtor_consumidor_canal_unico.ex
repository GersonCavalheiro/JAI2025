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

  # Cria um processo de buffer único que todos os consumidores monitoram
  def criar_canal() do
    canal_pid = spawn(fn -> canal_loop([]) end)
    Process.register(canal_pid, :canal)
    canal_pid
  end

  # Loop principal do canal que gerencia a fila de itens
  defp canal_loop(items) do
    receive do
      {:put, item} ->
        # Produtor enviou um item
        canal_loop([item | items])
      
      {:get, consumer_pid} ->
        # Consumidor quer um item
        case items do
          [] ->
            # Nenhum item disponível, avisa o consumidor para tentar novamente
            send(consumer_pid, {:retry})
            canal_loop([])
          
          [item | rest] ->
            # Envia o item mais antigo (primeiro a entrar, primeiro a sair)
            send(consumer_pid, {:item, item})
            canal_loop(rest)
        end
      
      {:shutdown} ->
        # Sinal para encerrar o canal
        :ok
    end
  end

  def produtor(id, primos) do
    Enum.each(primos, fn p ->
      IO.puts("Produtor #{id} produziu item #{p}")
      send(:canal, {:put, p})
      # Pequena pausa para demonstrar concorrência
      :timer.sleep(:rand.uniform(100))
    end)
  end

  def consumidor(id, timeout \\ 1000) do
    # Envia requisição para o canal
    send(:canal, {:get, self()})
    
    receive do
      {:item, valor} when valor < 0 ->
        IO.puts("Consumidor #{id} recebeu sinal de término.")
        :ok

      {:item, valor} ->
        IO.puts("Consumidor #{id} consumiu #{valor}")
        # Pequena pausa para simular o processamento
        :timer.sleep(:rand.uniform(200))
        consumidor(id, timeout)
        
      {:retry} ->
        # Nenhum item disponível, tenta novamente após um breve tempo
        :timer.sleep(50)
        consumidor(id, timeout)
    after
      timeout ->
        # Timeout atingido, tenta novamente
        consumidor(id, timeout)
    end
  end

  def main(args) do
    case args do
      [n_str, np_str, nc_str] ->
        {n, _} = Integer.parse(n_str)
        {np, _} = Integer.parse(np_str)
        {nc, _} = Integer.parse(nc_str)

        primos = gerar_primos(n)

        # Cria o canal compartilhado
        criar_canal()

        # Inicia consumidores
        consumidores =
          for i <- 1..nc do
            spawn(fn -> consumidor(i) end)
          end

        # Inicia produtores
        produtores =
          for i <- 1..np do
            spawn(fn -> produtor(i, primos) end)
          end

        # Monitora todos os produtores para saber quando terminarem
        Enum.each(produtores, fn pid -> Process.monitor(pid) end)

        # Aguarda a finalização de todos os produtores
        wait_for_produtores(length(produtores))

        # Envia sinais de término para os consumidores
        for _ <- 1..nc do
          send(:canal, {:put, -1})
        end
        
        # Aguarda um tempo para os consumidores processarem os sinais de término
        :timer.sleep(1000)
        
        # Encerra o canal
        send(:canal, {:shutdown})

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
