# Use uma imagem base do Ubuntu
FROM ubuntu:latest

# Instale as dependências básicas
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    curl \
    git \
    wget \
    unzip \
    vim \
    tmux

# Atualize os certificados CA
RUN apt-get update && apt-get install -y ca-certificates && update-ca-certificates

# C++20
RUN apt-get install -y g++
RUN g++ --version

# Rust
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y && \
    export PATH="/root/.cargo/bin:${PATH}" && \
    rustc --version && \
    cargo --version

# Go
ENV GOLANG_VERSION=1.24.2
RUN wget https://go.dev/dl/go${GOLANG_VERSION}.linux-amd64.tar.gz
RUN rm -rf /usr/local/go && tar -C /usr/local -xzf go${GOLANG_VERSION}.linux-amd64.tar.gz
ENV PATH="/usr/local/go/bin:${PATH}"
RUN go version

# Elixir
RUN apt-get update && apt-get install -y --no-install-recommends \
    erlang \
    elixir
RUN elixir --version
