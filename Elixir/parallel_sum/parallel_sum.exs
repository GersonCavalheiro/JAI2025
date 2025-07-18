# --------------------------------------
# Este programa faz parte do material que acompanha o curso "Programação Multithread
# : Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Gerald
# o H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atual
# ização de Informática (JAI 2024) e se encontra disponível em https://github.com/Gers
# onCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
# --------------------------------------
#
# Descrição do Programa
#
# Este script escrito em Elixir utiliza a biblioteca Flow para realizar uma soma par
# alela sobre uma coleção de números inteiros. O objetivo é somar todos os valores
# entre 1 e 1 milhão que sejam múltiplos de 3 e maiores que 100.
#
# A biblioteca Flow permite dividir e processar fluxos de dados em paralelo, combin
# ando expressividade funcional com desempenho escalável.
#
# Parâmetros de Lançamento
#
# Para executar o script, certifique-se de estar dentro de um projeto Mix com a dep
# endência {:flow, "~> 1.2"} instalada e execute:
#
# mix run parallel_sum.exs
#
# Recursos de Programação Concorrente Utilizados
#
# - Flow.from_enumerable/1: transforma uma range em fluxo paralelo.
# - Flow.filter/2 e Enum.sum/1: expressam a lógica de filtro e redução de forma funcional.
# - Paralelismo de dados: abstraído pela biblioteca Flow sobre `GenStage` e processos.
#
# Este exemplo demonstra uma forma idiomática de expressar paralelismo de dados em Elixir,
# com um modelo declarativo e escalável.

data = 1..1_000_000

result =
  data
  |> Flow.from_enumerable()
  |> Flow.filter(&(&1 > 100 and rem(&1, 3) == 0))
  |> Enum.sum()

IO.puts("Sum = #{result}")

