defmodule WordCounter do

@doc """
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este programa realiza a contagem de palavras em um arquivo de texto, paralelizando o processamento em blocos utilizando `Task.async_stream`. É um exemplo de uso de multiprogramação leve em Elixir com tarefas assíncronas.

Funcionamento

- O arquivo indicado é lido e convertido em uma lista de palavras, após a remoção de pontuação e a conversão para minúsculas.
- Apenas palavras com comprimento maior ou igual ao valor especificado são consideradas.
- As palavras são divididas em blocos de 1.000 elementos.
- Cada bloco é processado em paralelo com `Task.async_stream`, utilizando tantas tarefas quanto o número de *cores* disponíveis.
- A função `contar_palavras/1` gera um mapa de frequências por bloco.
- Os mapas parciais são então combinados com `Map.merge`, somando as contagens.
- O resultado final é impresso no terminal, com cada palavra seguida de sua frequência.

Exemplo de execução:

$ elixir conta_palavras.ex texto.txt 4
tempo: 12
trabalho: 9
vida: 8
...

Conceitos Aplicados

- Paralelização com `Task.async_stream` para melhor aproveitamento dos *cores* disponíveis.
- Processamento em blocos para limitar a carga de cada tarefa.
- Manipulação de texto e contagem de frequências com mapas (`Map`).
- Eliminação de pontuação usando expressões regulares Unicode.

Este programa mostra como dividir tarefas computacionalmente intensivas em partes menores e distribuí-las eficientemente por múltiplas tarefas concorrentes em Elixir.
"""

  @punctuation ~r/[\p{P}"']+/u

  def run([caminho, tamanho_minimo]) do
    tamanho_minimo = String.to_integer(tamanho_minimo)

    palavras =
      caminho
      |> File.read!()
      |> limpar_texto()
      |> String.downcase()
      |> String.split()
      |> Enum.filter(&(String.length(&1) >= tamanho_minimo))

    blocos = Enum.chunk_every(palavras, 1_000)

    resultados =
      blocos
      |> Task.async_stream(&contar_palavras/1, max_concurrency: System.schedulers_online(), timeout: :infinity)
      |> Enum.map(fn {:ok, mapa} -> mapa end)

    mapa_final = combinar_mapas(resultados)

    Enum.each(mapa_final, fn {palavra, contagem} ->
      IO.puts("#{palavra}: #{contagem}")
    end)
  end

  defp limpar_texto(texto) do
    Regex.replace(@punctuation, texto, " ")
  end

  defp contar_palavras(lista) do
    Enum.reduce(lista, %{}, fn palavra, acc ->
      Map.update(acc, palavra, 1, &(&1 + 1))
    end)
  end

  defp combinar_mapas(lista) do
    Enum.reduce(lista, %{}, fn mapa, acc ->
      Map.merge(acc, mapa, fn _k, v1, v2 -> v1 + v2 end)
    end)
  end
end

# Executar via: elixir word_counter.exs texto.txt 4
WordCounter.run(System.argv())

