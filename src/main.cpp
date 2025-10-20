/**
 * @file main.cpp
 * @brief Programa principal de monitoramento do sensor SW-420
 * @author Turma de Programação Aplicada - IME
 * @date 2025
 *
 * Este programa realiza a leitura contínua do sensor de vibração SW-420
 * conectado ao kit STM32MP1-DK1 através da interface IIO.
 *
 * Funcionalidades:
 * - Inicialização do sensor
 * - Leitura periódica (a cada 500ms)
 * - Exibição do valor bruto do ADC
 * - Detecção de vibração baseada em threshold
 */

#include "sw420.h"
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief Função principal
 *
 * Inicializa o sensor e entra em loop de monitoramento contínuo.
 *
 * @return 0 em caso de sucesso, 1 em caso de erro na inicialização
 */
int main() {
    // Caminho IIO para leitura analógica
    // NOTA: Ajuste o número do canal (voltage13) conforme identificado no kit
    const std::string iio_path = "/sys/bus/iio/devices/iio:device0/in_voltage13_raw";

    // Instancia o objeto do sensor
    SW420 sensor(iio_path);

    // Inicializa o sensor
    try {
        sensor.init();
    } catch (const std::exception &e) {
        std::cerr << "[ERRO] " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[INFO] Monitorando sensor de vibração SW-420..." << std::endl;
    std::cout << "[INFO] Pressione Ctrl+C para encerrar" << std::endl;
    std::cout << std::endl;

    // Loop de monitoramento
    while (true) {
        // Lê o valor bruto do ADC
        int raw_value = sensor.readRaw();

        // Verifica se há vibração detectada
        bool vibration_detected = sensor.read();

        // Exibe os dados
        if (vibration_detected) {
            std::cout << "[ALERTA] Vibração detectada! Valor: " << raw_value << std::endl;
        } else {
            std::cout << "[INFO] Sem vibração. Valor: " << raw_value << std::endl;
        }

        // Aguarda 500ms antes da próxima leitura
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
