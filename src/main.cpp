/**
 * @file main.cpp
 * @brief Programa principal para teste do sensor SW-420
 * @author Projeto Monitoramento Inteligente de Carga
 * @date 2025-08-31
 * 
 * Este programa demonstra o uso da classe SW420VibrationSensor
 * para monitoramento de vibrações em tempo real.
 */

#include "sw420.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>

// Variável global para controle do loop principal
volatile bool running = true;

/**
 * @brief Handler para captura de sinais (Ctrl+C)
 * @param signal Sinal recebido
 */
void signalHandler(int signal) {
    std::cout << "\nSinal recebido, encerrando programa..." << std::endl;
    running = false;
}

/**
 * @brief Função principal
 * @return 0 se executado com sucesso
 */
int main() {
    std::cout << "=== Teste do Sensor SW-420 ===" << std::endl;
    std::cout << "Pressione Ctrl+C para encerrar\n" << std::endl;
    
    // Configura handler para captura de sinais
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Cria instância do sensor no GPIO 4 com debounce de 50ms
    SW420VibrationSensor sensor(4, 50);
    
    // Inicializa o sensor
    if (!sensor.initialize()) {
        std::cerr << "Erro: Falha na inicialização do sensor!" << std::endl;
        return -1;
    }
    
    std::cout << "Sensor inicializado com sucesso!" << std::endl;
    std::cout << sensor.getStatusInfo() << std::endl;
    std::cout << "\nMonitorando vibrações..." << std::endl;
    
    auto last_status_time = std::chrono::steady_clock::now();
    
    // Loop principal de monitoramento
    while (running) {
        // Lê o estado atual do sensor
        bool is_vibrating = sensor.readVibration();
        
        // Verifica se houve mudança de estado
        if (sensor.hasStateChanged()) {
            std::cout << "Mudança detectada: " 
                      << (is_vibrating ? "VIBRANDO" : "PARADO") << std::endl;
        }
        
        // Exibe estatísticas periodicamente (a cada 10 segundos)
        auto current_time = std::chrono::steady_clock::now();
        auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(
            current_time - last_status_time).count();
        
        if (time_diff >= 10) {
            std::cout << "\n--- Estatísticas ---" << std::endl;
            std::cout << "Vibrações detectadas: " << sensor.getVibrationCount() << std::endl;
            std::cout << "Tempo desde última vibração: " 
                      << sensor.getTimeSinceLastVibration() << "ms" << std::endl;
            std::cout << "Estado atual: " 
                      << (is_vibrating ? "VIBRANDO" : "PARADO") << std::endl;
            std::cout << "-------------------\n" << std::endl;
            
            last_status_time = current_time;
        }
        
        // Aguarda um breve período antes da próxima leitura
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Exibe estatísticas finais
    std::cout << "\n=== Estatísticas Finais ===" << std::endl;
    std::cout << sensor.getStatusInfo() << std::endl;
    
    std::cout << "\nPrograma encerrado." << std::endl;
    return 0;
}