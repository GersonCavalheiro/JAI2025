defmodule WordCounter do
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

