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
│   └── main.cpp         # Programa principal
├── Makefile             # Build system
├── Doxyfile             # Configuração do Doxygen
├── README.md            # Este arquivo
└── Documentation.pdf    # Documentação gerada (após make docs)
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

### Programa Principal

O arquivo `main.cpp` implementa:
- Inicialização do sensor
- Loop de monitoramento contínuo (leitura a cada 500ms)
- Exibição de valores brutos e estado de vibração
- Tratamento de erros

**Localização:** `src/main.cpp:1-69`

---

## Requisitos do Sistema

### Hardware

- **Kit de desenvolvimento:** STM32MP1-DK1
- **Sensor:** SW-420 Vibration Sensor
- **Conexões:**
  - VCC → 3.3V (pino CN16)
  - GND → GND (pino CN16)
  - AOUT → ADC (ex: in_voltage13_raw via Arduino D4)

⚠️ **IMPORTANTE:** Sempre alimente o sensor com **3.3V**. Nunca use 5V, pois pode danificar o kit!

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
[INFO] Monitorando sensor de vibração SW-420...
[INFO] Pressione Ctrl+C para encerrar

[INFO] Sem vibração. Valor: 125
[INFO] Sem vibração. Valor: 150
[ALERTA] Vibração detectada! Valor: 45000
[ALERTA] Vibração detectada! Valor: 52000
[INFO] Sem vibração. Valor: 200
```

### 4. Encerramento

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

---

## Informações de Entrega

**Disciplina:** Programação Aplicada
**Instituição:** Instituto Militar de Engenharia (IME)
**Projeto:** Monitoramento Inteligente de Carga
**Entrega:** 2 (prazo: 10/09/2025)

### Checklist da Entrega 2

- [x] Código-fonte em C++ documentado com Doxygen
- [x] Programa funcional no kit de desenvolvimento
- [x] Leitura correta dos dados do sensor
- [x] Instruções de compilação documentadas
- [x] Instruções de execução documentadas
- [x] Classe encapsulada e sem variáveis globais

---

## Autores

Grupo responsável pelo sensor SW-420 (Vibração)

---

## Licença

Projeto acadêmico - Instituto Militar de Engenharia