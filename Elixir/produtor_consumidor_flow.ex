Mix.install([:flow])

defmodule ProdutorConsumidorFlow do
  defp eh_primo(n) when n >= 2 do
    2..trunc(:math.sqrt(n))
    |> Enum.all?(fn d -> rem(n, d) != 0 end)
  end

  defp eh_primo(_), do: false

  defp gerar_primos(n) do
    Stream.iterate(2, &(&1 + 1))
    |> Stream.filter(&eh_primo/1)
    |> Enum.take(n)
  end

  def main(args) do
    case Enum.map(args, &String.to_integer/1) do
      [n, n_prod, n_cons] ->
        primos = gerar_primos(n)

        fluxo =
          Flow.from_enumerable(1..n_prod)
          |> Flow.flat_map(fn produtor_id ->
            Enum.map(primos, fn primo ->
              IO.puts("Produtor #{produtor_id} produziu item #{primo}")
              {produtor_id, primo}
            end) ++ [{:fim_produtor, produtor_id}]
          end)
          |> Flow.partition(stages: n_cons)
          |> Flow.map(fn
            {:fim_produtor, produtor_id} ->
              IO.puts("Produtor #{produtor_id} encerrado.")
              :ignore

            {_, item} ->
              consumidor = :erlang.phash2(item, n_cons) + 1
              IO.puts("Consumidor #{consumidor} consumiu #{item}")
              consumidor
          end)

        consumidores =
          fluxo
          |> Enum.reduce(%{}, fn
            :ignore, acc -> acc
            consumidor, acc -> Map.update(acc, consumidor, 1, &(&1 + 1))
          end)

        Enum.each(1..n_cons, fn id ->
          IO.puts("Consumidor #{id} encerrado. Total de itens consumidos: #{Map.get(consumidores, id, 0)}")
        end)

      _ ->
        IO.puts("Uso: elixir produtor_consumidor_flow.ex N N_PROD N_CONS")
    end
  end
end

ProdutorConsumidorFlow.main(System.argv())

