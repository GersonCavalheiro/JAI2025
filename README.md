# Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas — JAI 2025

Este repositório contém o material de apoio ao curso **Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas**, ministrado por **Gerson Geraldo H. Cavalheiro**, **André Rauber Du Bois** e **Alexandro Baldassin** durante as **Jornadas de Atualização em Informática 2025 (JAI 2025)** (Congresso da Sociedade Brasileira de Computação 2025).

O conteúdo do repositório está organizado por linguagem de programação e tem como objetivo ilustrar os modelos de concorrência e abstrações de cada linguagem. As implementações buscam destacar os recursos das linguagens C++20, Rust, Go e Elixir, não havendo preocupação específica com desempenho.

## Citação

Se você utilizar este material, por favor cite conforme a entrada BibTeX abaixo:

```bibtex
@chapter{cavalheiro2025multithread,
  author    = {Gerson Geraldo H. Cavalheiro and André Rauber Du Bois and Alexandro Baldassin},
  title     = {Programação Multithread: Modelos e Abstrações em Linguagens Contemporâneas},
  booktitle = {Anais das Jornadas de Atualização em Informática (JAI 2025)},
  editor    = {Soraia Raupp Musse and Alexandre Santos},
  publisher = {Sociedade Brasileira de Computação (SBC)},
  address   = {Porto Alegre},
  year      = {2025}
}
```

## Estrutura do Repositório

- `README.md`: este arquivo.
- `Docker/`: contém um `Dockerfile` com os compiladores e versões utilizadas no curso.
- `files/`: contém arquivos auxiliares de entrada para os exemplos
- `C++/`: exemplos em C++20.
- `Rust/`: exemplos em Rust, utilizando o ecossistema `cargo`.
- `Go/`: exemplos em Go.
- `Elixir/`: exemplos em Elixir.
- `Makefile`: gera os executáveis para C++20, Rust e Go e também possui o alvo `clean`.

Cada diretório de linguagem possui um `Makefile`. O comando `make` constroi os executáveis (exceto em Elixir) e `make run` executa todos os programas com os parâmetros apropriados. Também é possível executar grupos específicos de programas com alvos como `make run-fibo`, `make run-prodcons` etc. É importante observar que os recursos utilizados são os disponibilizados nas últimas versões disponíveis das linguagens C++, Rust, Go e Elixir. Caso não tenha disponível no seu sistema as últimas versões destas linguagens, é sugerido utilizar a imagem docker fornecida.

## Executando em Container com Docker

O repositório fornece um ambiente Docker que já inclui todos as ferramentas necessárias para exploração das linguagens, incluindo também `vim` e `tmux`. As versões disponibilizadas destas ferramentas são as utilizadas para preparar os exemplos. Para utilizar, primeiro certifique-se de possuir o Docker instalado (consulte https://www.docker.com/get-started) (a sequência a seguir foi executada sob GNU-Linux com Ubuntu 20.04):

1. Clone este repositório:
   ```sh
   git clone https://github.com/GersonCavalheiro/JAI2025
   cd JAI2025/Docker
   ```

2. Construa a imagem Docker (uns 6 minutos, em rede cabeada):
   ```sh
   docker build -t multithread-jai2025 .
   ```

3. Execute o container interativamente:
   ```sh
   docker run -it --rm -v "$PWD/..:/app" -w /app multithread-jai2025 bash
   ```

   Isso montará o conteúdo do repositório no container e abrirá um terminal para executar os `Makefile`s.

## Observações

- As implementações focam em clareza e variedade de modelos de concorrência, não sendo otimizadas para benchmarks.
- Para rodar os programas fora do container, é necessário ter as ferramentas equivalentes instaladas: `g++` com suporte a C++20, Rust, Go e Elixir

---
Sociedade Brasileira de Computação (SBC), JAI 2025.
