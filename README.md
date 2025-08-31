````markdown
# Monitoramento de Vibração com STM32 e Sensor SW-420

## 1. Descrição Geral do Projeto

[cite_start]Este projeto faz parte da disciplina de Programação Aplicada e tem como objetivo o desenvolvimento de um sistema de monitoramento para cargas sensíveis, utilizando o kit de desenvolvimento STM32MP1 DK1[cite: 19, 20].

[cite_start]O sistema utiliza um **Módulo Sensor de Vibração Digital (SW-420)** para detectar movimentações, impactos ou transporte indevido da carga[cite: 44]. [cite_start]Os dados coletados pelo sensor são processados e enviados em tempo real para um servidor central via rede local (UDP), caracterizando uma aplicação de Internet das Coisas (IoT)[cite: 23, 25].

Este repositório contém o código-fonte da classe C++ responsável pela interface com o sensor, um programa de teste e toda a documentação associada.

## 2. Estrutura do Código

O código-fonte está organizado da seguinte forma:

* `src/`: Contém os arquivos-fonte do projeto.
    * `sw420.h`: Arquivo de cabeçalho da classe `SW420VibrationSensor`. Define a interface da classe, incluindo métodos para inicialização, leitura de dados и configuração.
    * `sw420.cpp`: Implementação dos métodos da classe `SW420VibrationSensor`. Contém a lógica para interagir com o GPIO do Linux, aplicar o filtro de *debounce* e gerenciar os dados de vibração.
    * `main.cpp`: Programa principal de exemplo que instancia e utiliza a classe `SW420VibrationSensor` para monitorar e exibir o estado do sensor em tempo real.
* `docs/`: Contém a documentação do código gerada com Doxygen.
* `Makefile`: Arquivo para automação da compilação, geração de documentação e envio do executável para o kit STM32MP1.
* `Doxyfile`: Arquivo de configuração do Doxygen para geração da documentação do projeto.

## 3. Dependências

Para a compilação cruzada do projeto, é necessária a seguinte ferramenta:

* **Toolchain ARM:** `arm-buildroot-linux-gnueabihf-g++`.

Para a geração da documentação:

* **Doxygen**

## 4. Instruções de Compilação e Execução

O `Makefile` fornecido automatiza as principais tarefas do projeto.

### Compilação

Para compilar o projeto e gerar o executável `sw420_monitor`, execute o seguinte comando na raiz do repositório:

```bash
make all
````

### Gerando a Documentação

Para gerar a documentação do código-fonte com o Doxygen, utilize o comando:

```bash
make docs
```

A documentação será gerada no diretório `docs/html/`.

### Transferência e Execução no Kit STM32MP1

**1. Transferir o executável para o kit:**

Substitua `<IP_DO_KIT>` pelo endereço IP do seu STM32MP1 DK1 (ex: 192.168.42.2).

```bash
make install IP=<IP_DO_KIT>
```

Este comando utiliza `scp` para copiar o executável para o diretório `/home/root` do kit.

**2. Executar remotamente:**

Para executar o programa no kit via SSH, utilize o comando:

```bash
make run IP=<IP_DO_KIT>
```

O programa iniciará o monitoramento do sensor. Para encerrar a execução, pressione `Ctrl+C`.

```
```