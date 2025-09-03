#include "sw420.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <signal.h>

volatile bool running = true;

void signalHandler(int signal) {
    std::cout << "\nSinal recebido, encerrando programa..." << std::endl;
    running = false;
}

int main() {
    std::cout << "=== Teste do Sensor SW-420 ===" << std::endl;
    std::cout << "Pressione Ctrl+C para encerrar\n" << std::endl;
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    SW420VibrationSensor sensor(4, 0, 50);
    
    if (!sensor.initialize()) {
        std::cerr << "Erro: Falha na inicialização do sensor!" << std::endl;
        return -1;
    }
    
    std::cout << "Sensor inicializado com sucesso!" << std::endl;
    std::cout << sensor.getStatusInfo() << std::endl;
    std::cout << "\nMonitorando vibrações..." << std::endl;
    
    auto last_status_time = std::chrono::steady_clock::now();
    
    while (running) {
        bool is_vibrating = sensor.readVibration();
        
        if (sensor.hasStateChanged()) {
            std::cout << "Mudança detectada: " 
                      << (is_vibrating ? "VIBRANDO" : "PARADO") << std::endl;
        }
        
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
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "\n=== Estatísticas Finais ===" << std::endl;
    std::cout << sensor.getStatusInfo() << std::endl;
    
    std::cout << "\nPrograma encerrado." << std::endl;
    return 0;
}