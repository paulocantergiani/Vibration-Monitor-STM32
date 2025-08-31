# Makefile para compilação do projeto SW-420
# Projeto Monitoramento Inteligente de Carga

# Configurações do compilador
CXX = arm-buildroot-linux-gnueabihf-g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -pthread

# Diretórios e arquivos
SRC_DIR = src
TARGET = sw420_monitor
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,%.o,$(SOURCES))
# Correção para o alvo de verificação de sintaxe
CHECK_SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/sw420.cpp

# Regra principal
all: $(TARGET)

# Compilação do executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilação concluída: $(TARGET)"

# Compilação dos objetos
# Esta regra agora busca os fontes em $(SRC_DIR)/
%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza dos arquivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Arquivos de compilação removidos"

# Instalação no kit (via SCP)
# Uso: make install IP=192.168.42.2
install: $(TARGET)
	@if [ -z "$(IP)" ]; then \
		echo "Erro: Especifique o IP do kit. Exemplo: make install IP=192.168.42.2"; \
		exit 1; \
	fi
	scp $(TARGET) root@$(IP):/home/root/
	@echo "Executável enviado para root@$(IP):/home/root/"

# Execução remota no kit
# Uso: make run IP=192.168.42.2
run: install
	ssh root@$(IP) "cd /home/root && ./$(TARGET)"

# Geração da documentação Doxygen
docs:
	@if command -v doxygen >/dev/null 2>&1; then \
		doxygen Doxyfile; \
		echo "Documentação gerada em ./docs/html/"; \
	else \
		echo "Erro: Doxygen não encontrado"; \
	fi

# Verificação de sintaxe
check:
	$(CXX) $(CXXFLAGS) -fsyntax-only $(CHECK_SOURCES)
	@echo "Verificação de sintaxe concluída"

# Debug - compilação com símbolos de debug
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)

# Informações sobre o projeto
info:
	@echo "Projeto: Monitoramento SW-420"
	@echo "Compilador: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Arquivos fonte: $(SOURCES)"
	@echo "Executável: $(TARGET)"

.PHONY: all clean install run docs check debug info