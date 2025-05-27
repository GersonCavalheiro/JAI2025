@doc """
--------------------------------------
Este programa faz parte do material que acompanha o curso "Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas", ministrado por "Gerson Geraldo H. Cavalheiro, Alexandro Baldassin, André Rauber Du Bois" nas Jornadas de Atualização de Informática (JAI 2024) e se encontra disponível em https://github.com/GersonCavalheiro/JAI2025. Ao utilizar, referenciar a fonte.
--------------------------------------

Descrição do Programa

Este exemplo demonstra a criação básica de um processo leve (lightweight process) em Elixir utilizando a função `spawn`.

Trata-se de uma introdução à concorrência baseada em processos, característica da máquina virtual Erlang (BEAM), sobre a qual Elixir é construído. Cada processo é isolado e se comunica com outros processos exclusivamente por troca de mensagens.

Funcionamento

- Define-se um módulo `Demo` com uma função `saudacao/0` que imprime uma mensagem no terminal.
- O processo é criado com `spawn(Demo, :saudacao, [])`, que executa a função `saudacao/0` de forma assíncrona em um novo processo.
- A função `spawn` retorna imediatamente um identificador de processo (PID), permitindo que o programa principal continue sua execução sem esperar o término do processo criado.

Exemplo de saída:
Olá do processo!

Conceitos Aplicados

- Concorrência baseada em processos isolados.
- Criação de processos com `spawn/3`.
- Modelo de execução assíncrona, sem bloqueio.
- Sem compartilhamento de memória entre processos (modelo "share nothing").

Este é o ponto de partida para os demais exemplos com troca de mensagens e coordenação entre processos em Elixir.

     """

defmodule Demo do
  def saudacao do
    IO.puts("Olá do processo!")
    end
end
pid = spawn(Demo, :saudacao, [])
