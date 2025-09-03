# STM32 Vibration Monitor

## Descrição

Sistema de monitoramento de vibrações para STM32MP1 usando sensor SW-420. Monitora vibrações através da interface IIO do Linux embarcado.

## Compilação e Execução

```bash
# Compilar
make clean
make

# Deploy na placa
make deploy

# Executar na placa
ssh root@192.168.42.2
./VibrationMonitor

# Gerar documentação
make docs
```

## Estrutura do Código

- **Classe SW420** (`src/sw420.h`, `src/sw420.cpp`):
  - `SW420(path)`: Construtor
  - `init()`: Inicializa sensor
  - `readRaw()`: Lê valor analógico bruto
  - `read()`: Detecta vibração (threshold >= 65535)

- **Main** (`src/main.cpp`): Loop de monitoramento a cada 500ms

## Dependências

- ARM toolchain: `arm-buildroot-linux-gnueabihf-g++`
- Doxygen (documentação)
- SSH/SCP para deploy (IP: 192.168.42.2)