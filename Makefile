#################################################################
## Makefile - Sensor de Vibração SW-420
## Projeto: Monitoramento Inteligente de Carga
## Instituto Militar de Engenharia
#################################################################

#################################################################
## Variáveis de Execução
#################################################################

# Nome do executável
TARGET      := VibrationMonitor

# Diretórios e arquivos fonte
SRCDIR      := src
SRC         := $(wildcard $(SRCDIR)/*.cpp)
OBJ         := $(SRC:.cpp=.o)

# Configurações da placa STM32MP1-DK1
PLACA_USER  := root
PLACA_IP    := 192.168.42.2
PLACA_PATH  := /root

# Diretório da documentação LaTeX (gerada pelo Doxygen)
LATEX_PATH  := docs/latex

# Toolchain para compilação cruzada ARM
CXX         := arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-g++
SYSROOT     := arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot
CXXFLAGS    := --sysroot=$(SYSROOT) -Wall -O2 -I$(SRCDIR)
LIBS        :=



#################################################################
## Regras de Compilação
#################################################################

# Alvo padrão: compila o executável
all: $(TARGET)

# Linkagem do executável final
$(TARGET): $(OBJ)
	@echo "\e[1;36;40m[INFO] Linkando binário para placa...\e[0m"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS)
	@echo "\e[1;32;40m[OK] Binário '$(TARGET)' gerado com sucesso!\e[0m"

# Compilação de arquivos objeto
%.o: %.cpp
	@echo "\e[1;36;40m[INFO] Compilando $<...\e[0m"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

#################################################################
## Comandos Auxiliares
#################################################################

# Deploy: envia o executável para a placa STM32MP1-DK1
deploy: $(TARGET)
	@echo "\e[1;36;40m[INFO] Enviando binário para placa...\e[0m"
	@scp -O $(TARGET) $(PLACA_USER)@$(PLACA_IP):$(PLACA_PATH)/
	@echo "\e[1;36;40m[INFO] Ajustando permissões de execução...\e[0m"
	@ssh $(PLACA_USER)@$(PLACA_IP) "chmod +x $(PLACA_PATH)/$(TARGET)"
	@echo "\e[1;32;40m[OK] Deploy concluído! Execute com: ssh $(PLACA_USER)@$(PLACA_IP) './$(TARGET)'\e[0m"

# Documentação: gera a documentação com Doxygen e compila o PDF
docs:
	@echo "\e[1;36;40m[INFO] Gerando documentação com Doxygen...\e[0m"
	@doxygen Doxyfile
	@echo "\e[1;36;40m[INFO] Compilando PDF da documentação...\e[0m"
	@$(MAKE) -C $(LATEX_PATH)
	@echo "\e[1;36;40m[INFO] Movendo PDF para o diretório raiz...\e[0m"
	@mv $(LATEX_PATH)/refman.pdf Documentation.pdf
	@echo "\e[1;32;40m[OK] Documentação gerada: Documentation.pdf\e[0m"

# Limpeza: remove arquivos compilados e documentação
clean:
	@rm -rf docs/html docs/latex $(OBJ) $(TARGET) debug_$(TARGET)
	@echo "\e[1;33;40m[INFO] Arquivos de compilação removidos\e[0m"

.PHONY: all deploy docs clean