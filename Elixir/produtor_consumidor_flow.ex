Mix.install([:flow])

defmodule ProdutorConsumidorFlow do
@moduledoc """
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa implementa o problema do produtor/consumidor em Elixir utilizando a biblioteca Flow, que oferece uma abstração de processamento concorrente baseada em fluxos de dados. A comunicação entre produtores e consumidores é coordenada por meio da composição funcional de operações sobre o fluxo.

Funcionamento

- A função `gerar_primos/1` produz os *n* primeiros números primos usando streams.
- A etapa `Flow.from_enumerable/1` simula múltiplos produtores. Cada produtor gera os mesmos *n* primos e os empacota como tuplas com seu identificador.
- A função `flat_map` expande a produção de cada produtor e adiciona um marcador especial `{:fim_produtor, id}` para indicar seu término.
- O fluxo é particionado com `Flow.partition/1` para distribuir os dados entre *n_cons* consumidores.
- A função `map` realiza o consumo dos itens e contabiliza os itens processados por consumidor. Um hash do valor é usado para decidir o consumidor.
- No final, `Enum.reduce/3` acumula a quantidade total de itens consumidos por consumidor e `Enum.each/2` exibe os totais.

Exemplo de execução:
$ elixir produtor_consumidor_flow.ex 5 2 3
Produtor 1 produziu item 2
Produtor 1 produziu item 3
...
Consumidor 1 consumiu 2
Consumidor 2 consumiu 3
...
Consumidor 1 encerrado. Total de itens consumidos: 3
Consumidor 2 encerrado. Total de itens consumidos: 4
Consumidor 3 encerrado. Total de itens consumidos: 3

Conceitos Aplicados

- Uso da biblioteca Flow para processamento concorrente em Elixir.
- Paralelismo por particionamento de fluxo (stages) com balanceamento automático.
- Comunicação implícita e ordenação parcial dos dados no fluxo.
- Contabilização agregada de resultados com `Enum.reduce`.

Esta versão demonstra como fluxos paralelos podem ser utilizados para modelar concorrência entre produtores e consumidores, de forma declarativa e escalável, aproveitando a infraestrutura de processamento concorrente oferecida pela plataforma Beam.
"""

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
