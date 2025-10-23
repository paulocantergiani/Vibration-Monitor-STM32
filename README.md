# Monitor de Vibração STM32MP1 - Sensor SW-420

## Descrição do Projeto

Sistema embarcado de monitoramento de vibrações desenvolvido para o kit **STM32MP1-DK1**, utilizando o sensor de vibração **SW-420**. O projeto faz parte da disciplina de **Programação Aplicada** do **Instituto Militar de Engenharia (IME)**.

### Contexto

Este sistema integra o projeto **Monitoramento Inteligente de Carga**, que visa detectar tentativas de violação, movimentação indevida ou comprometimento de integridade de cargas sensíveis durante transporte.

### Sensor SW-420

O **SW-420** é um sensor de vibração digital/analógico baseado em um interruptor de esfera condutora. Quando ocorre vibração, a esfera movimenta-se e altera o estado elétrico do sensor.

**Características:**
- Tensão de operação: 3.3V - 5V (usamos **3.3V** para compatibilidade com o STM32MP1)
- Saída analógica proporcional à intensidade da vibração
- Interface IIO (Industrial I/O) do Linux embarcado

---

## Estrutura do Código

### Arquivos do Projeto

```
Vibration-Monitor-STM32/
├── src/
│   ├── sw420.h          # Header da classe SW420
│   ├── sw420.cpp        # Implementação da classe SW420
│   ├── udpclient.h      # Header da classe UDPClient
│   ├── udpclient.cpp    # Implementação da classe UDPClient
│   └── main.cpp         # Programa principal
├── Makefile             # Build system
├── Doxyfile             # Configuração do Doxygen
├── class_diagram.gv     # Diagrama UML simplificado (Graphviz)
├── class_diagram.png    # Diagrama UML simplificado (PNG)
├── class_diagram_detailed.gv     # Diagrama UML detalhado (Graphviz)
├── class_diagram_detailed.png    # Diagrama UML detalhado (PNG)
├── README.md            # Este arquivo
└── Documentation.pdf    # Documentação gerada (após make docs)
```

### Diagramas UML

O projeto inclui dois diagramas UML em formato de blobs criados com **Graphviz**:

#### 1. **Diagrama Simplificado** (`class_diagram.gv` / `class_diagram.png`)

Mostra a arquitetura principal com as duas classes principais e a aplicação:

- **SW420**: Classe responsável pela leitura do sensor de vibração
  - Atributos: `iio_path_`, `threshold_`
  - Métodos: `init()`, `readRaw()`, `read()`

- **UDPClient**: Classe responsável pela comunicação em rede
  - Atributos: `server_ip_`, `server_port_`, `sensor_id_`, `sock_fd_`, `server_addr_`, `connected_`
  - Métodos: `init()`, `sendData()`, `isConnected()`

- **main.cpp**: Aplicação principal que orquestra as duas classes

#### 2. **Diagrama Detalhado** (`class_diagram_detailed.gv` / `class_diagram_detailed.png`)

Mostra uma visão mais completa do sistema incluindo:

- **SW420** (Sensor de Vibração) - conexão com o hardware
- **UDPClient** (Cliente de Rede) - implementação da comunicação
- **Aplicação Principal** (main.cpp) - fluxo de execução
- **UDP Protocol** - formato CSV dos dados
- **Hardware** - especificações do sensor SW-420

#### Editando os Diagramas

Os arquivos `.gv` (Graphviz) podem ser editados com qualquer editor de texto. Para regenerar os PNGs após editar:

```bash
dot -Tpng class_diagram.gv -o class_diagram.png
dot -Tpng class_diagram_detailed.gv -o class_diagram_detailed.png
```

### Classe SW420

Implementa o driver do sensor com encapsulamento orientado a objetos.

**Métodos públicos:**
- `SW420(const std::string &iio_path)`: Construtor que recebe o caminho do canal IIO
- `void init()`: Inicializa e valida o sensor
- `int readRaw()`: Retorna o valor bruto do ADC (0-65535)
- `bool read()`: Interpretação digital (true = vibração detectada)

**Atributos privados:**
- `iio_path_`: Caminho do arquivo IIO
- `threshold_`: Limiar de detecção (padrão: 32000)

**Localização:** `src/sw420.h:19-60` e `src/sw420.cpp:1-68`

### Classe UDPClient

Implementa a comunicação UDP para envio de dados ao servidor central.

**Métodos públicos:**
- `UDPClient(server_ip, server_port, sensor_id)`: Construtor com configurações de rede
- `bool init()`: Inicializa o socket UDP
- `bool sendData(value, unit)`: Envia dados do sensor via UDP
- `bool isConnected()`: Verifica status da conexão

**Atributos privados:**
- `server_ip_`: Endereço IP do servidor
- `server_port_`: Porta do servidor
- `sensor_id_`: Identificador único do sensor
- `sock_fd_`: File descriptor do socket
- `server_addr_`: Estrutura de endereço do servidor

**Localização:** `src/udpclient.h` e `src/udpclient.cpp`

### Programa Principal

O arquivo `main.cpp` implementa:
- Inicialização do sensor
- Inicialização do cliente UDP
- Loop de monitoramento contínuo (leitura a cada 500ms)
- Exibição de valores brutos e estado de vibração
- Envio de dados via UDP para servidor central
- Tratamento de erros

**Localização:** `src/main.cpp:1-93`

---

## Protocolo de Comunicação UDP

### Visão Geral

O sistema utiliza o protocolo **UDP (User Datagram Protocol)** para transmissão em tempo real dos dados do sensor para um servidor central de monitoramento. UDP foi escolhido por:
- **Baixa latência**: Ideal para monitoramento em tempo real
- **Simplicidade**: Sem overhead de estabelecimento de conexão
- **Eficiência**: Menor consumo de recursos no sistema embarcado

### Formato da Mensagem

As mensagens são enviadas no formato **CSV (Comma-Separated Values)** para facilidade de parsing e baixo overhead.

**Estrutura:**
```
SENSOR_ID,TIMESTAMP,VALUE,UNIT
```

**Campos:**
1. **SENSOR_ID** (string): Identificador único do sensor/grupo
   - Exemplo: `SW420_VIBRATION`, `SW420_GRUPO_10`

2. **TIMESTAMP** (string): Data e hora da leitura no formato ISO 8601
   - Formato: `YYYY-MM-DDTHH:MM:SS`
   - Exemplo: `2025-10-22T15:30:45`

3. **VALUE** (int): Valor lido do sensor
   - Faixa: 0-65535 (ADC de 16 bits)
   - Exemplo: `45000`

4. **UNIT** (string): Unidade de medida
   - Padrão: `ADC` (valor bruto do conversor analógico-digital)
   - Outros possíveis: `mV`, `g`, etc.

### Exemplo de Mensagem

```
SW420_VIBRATION,2025-10-22T15:30:45,45000,ADC
```

### Configuração de Rede

**No Kit STM32MP1-DK1:**
- IP: `192.168.42.2` (padrão)
- Porta de origem: Dinâmica (alocada pelo sistema)

**No Servidor (PC):**
- IP: `192.168.42.10` (configurável em `src/main.cpp:38`)
- Porta: `5000` (configurável em `src/main.cpp:39`)
- Protocolo: UDP

### Fluxo de Comunicação

```
┌─────────────────┐                      ┌─────────────────┐
│   STM32MP1-DK1  │                      │  Servidor (PC)  │
│  192.168.42.2   │                      │  192.168.42.10  │
└────────┬────────┘                      └────────┬────────┘
         │                                        │
         │  1. Leitura do sensor (500ms)         │
         ├────────────────────────────────────>  │
         │                                        │
         │  2. Envia pacote UDP:                 │
         │     SW420_VIBRATION,2025-...,45000,ADC│
         ├────────────────────────────────────>  │
         │                                        │
         │  3. Servidor processa e exibe         │
         │     (sem confirmação - UDP)           │
         │                                        │
         │  4. Aguarda 500ms                     │
         │                                        │
         │  5. Repete o ciclo                    │
         └───────────────────────────────────>   │
```

### Implementação

A classe `UDPClient` encapsula toda a lógica de comunicação UDP:

```cpp
// Configuração
UDPClient udp_client("192.168.42.10", 5000, "SW420_VIBRATION");

// Inicialização
udp_client.init();

// Envio de dados
int sensor_value = sensor.readRaw();
udp_client.sendData(sensor_value, "ADC");
```

### Configuração do Servidor

Para receber os dados, execute um servidor UDP no PC. Exemplo em Python no arquivo udp_server.py

### Personalização

Para alterar as configurações de rede, edite o arquivo `src/main.cpp`:

```cpp
// Linha 38: IP do servidor
const std::string server_ip = "192.168.42.10";

// Linha 39: Porta do servidor
const int server_port = 5000;

// Linha 40: Identificador do sensor
const std::string sensor_id = "SW420_VIBRATION";
```

---

## Requisitos do Sistema

### Hardware

- **Kit de desenvolvimento:** STM32MP1-DK1
- **Sensor:** SW-420 Vibration Sensor
- **Conexões:**
  - VCC → 3.3V (pino CN16)
  - GND → GND (pino CN16)
  - AOUT → ADC (ex: in_voltage13_raw via Arduino D4)


### Software

#### Na Máquina de Desenvolvimento

- **Sistema operacional:** Linux (recomendado: Ubuntu 20.04 ou superior)
- **Toolchain ARM:** `arm-buildroot-linux-gnueabihf_sdk-buildroot`
- **Ferramentas:**
  - `make`
  - `ssh` e `scp`
  - `doxygen` (para gerar documentação)
  - `pdflatex` (para compilar PDF da documentação)

#### No Kit STM32MP1-DK1

- Linux embarcado (pré-configurado)
- Interface IIO habilitada
- Acesso via rede (IP padrão: `192.168.42.2`)

---

## Instruções de Compilação

### 1. Preparação do Ambiente

Certifique-se de que a toolchain ARM está instalada e acessível no diretório do projeto:

```bash
# Verifique se o compilador existe
ls arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-g++
```

### 2. Limpeza (Opcional)

Remove arquivos de compilação anteriores:

```bash
make clean
```

### 3. Compilação

Compila o código-fonte para a arquitetura ARM:

```bash
make
```

**Saída esperada:**
```
[INFO] Compilando src/sw420.cpp...
[INFO] Compilando src/main.cpp...
[INFO] Linkando binário para placa...
[OK] Binário 'VibrationMonitor' gerado com sucesso!
```

O executável `VibrationMonitor` será criado no diretório raiz do projeto.

---

## Instruções de Execução

### 1. Identificação do Canal ADC

Antes de executar pela primeira vez, identifique qual canal IIO corresponde ao pino conectado ao sensor:

```bash
# Conecte-se à placa
ssh root@192.168.42.2

# Liste os canais disponíveis
ls /sys/bus/iio/devices/iio:device0/in_voltage*_raw

# Conecte o pino do sensor ao GND e teste cada canal
cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw
cat /sys/bus/iio/devices/iio:device0/in_voltage1_raw
# ... continue até encontrar o canal que retorna 0
```

O canal que retornar valor zero é o correto. Atualize o caminho em `src/main.cpp:32`.

### 2. Deploy para a Placa

Envia o executável para o kit via SCP e ajusta permissões:

```bash
make deploy
```

**Requisitos:**
- Placa conectada na mesma rede
- IP configurado: `192.168.42.2`
- SSH habilitado (usuário `root`, senha `root`)

### 3. Execução no Kit

Conecte-se à placa via SSH e execute o programa:

```bash
# Acesso via SSH
ssh root@192.168.42.2

# Navegue até o diretório (se necessário)
cd /root

# Execute o programa
./VibrationMonitor
```

**Saída esperada:**
```
[INFO] SW420 inicializado em /sys/bus/iio/devices/iio:device0/in_voltage13_raw
[INFO] Cliente UDP inicializado para 192.168.42.10:5000
[INFO] Monitorando sensor de vibração SW-420...
[INFO] Pressione Ctrl+C para encerrar

[INFO] Sem vibração. Valor: 125
[UDP] Dados enviados ao servidor
[INFO] Sem vibração. Valor: 150
[UDP] Dados enviados ao servidor
[ALERTA] Vibração detectada! Valor: 45000
[UDP] Dados enviados ao servidor
[ALERTA] Vibração detectada! Valor: 52000
[UDP] Dados enviados ao servidor
[INFO] Sem vibração. Valor: 200
[UDP] Dados enviados ao servidor
```

### 4. Configuração do Servidor UDP (PC)

Antes de executar o programa na placa, configure um servidor UDP no PC para receber os dados:

**Opção 1: Servidor Python simples**

Execute:
```bash
python3 udp_server.py
```

**Opção 2: Usar netcat (nc)**

```bash
nc -ul 5000
```

### 5. Encerramento

Pressione `Ctrl+C` para interromper o programa.

---

## Geração de Documentação

O projeto utiliza **Doxygen** para gerar documentação automática do código.

### Gerar Documentação Completa

```bash
make docs
```

Este comando:
1. Gera documentação HTML em `docs/html/`
2. Gera documentação LaTeX em `docs/latex/`
3. Compila o LaTeX em PDF
4. Move o PDF para `Documentation.pdf`

### Visualizar Documentação

- **HTML:** Abra `docs/html/index.html` no navegador
- **PDF:** Abra `Documentation.pdf`

---

## Troubleshooting

### Erro: "Failed to open IIO file"

**Causa:** Caminho do canal IIO incorreto.

**Solução:**
1. Identifique o canal correto (veja seção 1 de Execução)
2. Atualize `src/main.cpp:32` com o caminho correto
3. Recompile e faça deploy novamente

### Erro: "ssh: connect to host 192.168.42.2 port 22: No route to host"

**Causa:** Placa não acessível na rede.

**Solução:**
1. Verifique se o cabo Ethernet está conectado
2. Configure IP estático no PC: `192.168.42.10/24`
3. Teste conectividade: `ping 192.168.42.2`

### Sensor sempre retorna valor baixo

**Causa:** Sensor pode estar com defeito ou mal conectado.

**Solução:**
1. Verifique as conexões (VCC, GND, AOUT)
2. Teste o sensor com multímetro
3. Ajuste o threshold em `src/sw420.cpp:19` se necessário