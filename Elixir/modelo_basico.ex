defmodule Demo do
  def saudacao do
    IO.puts("Olá do processo!")
    end
end
pid = spawn(Demo, :saudacao, [])
