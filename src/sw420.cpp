/**
 * @file sw420.cpp
 * @brief Implementação do driver para o sensor SW-420
 * @author Turma de Programação Aplicada - IME
 * @date 2025
 */

#include "sw420.h"
#include <iostream>
#include <stdexcept>

/**
 * @brief Construtor
 *
 * Inicializa o sensor com o caminho IIO especificado.
 * Define o threshold padrão em 32000 (aproximadamente metade da escala do ADC).
 */
SW420::SW420(const std::string &iio_path)
    : iio_path_(iio_path), threshold_(32000) {}

/**
 * @brief Inicialização do sensor
 *
 * Testa a abertura do arquivo IIO para validar o caminho fornecido.
 */
void SW420::init() {
    std::ifstream file(iio_path_);
    if (!file.is_open()) {
        throw std::runtime_error("Falha ao abrir arquivo IIO: " + iio_path_);
    }
    file.close();
    std::cout << "[INFO] SW420 inicializado em " << iio_path_ << std::endl;
}

/**
 * @brief Leitura do valor bruto do ADC
 *
 * Abre o arquivo IIO, lê o valor inteiro e fecha o arquivo.
 *
 * @return Valor bruto (0-65535) ou -1 em caso de erro
 */
int SW420::readRaw() {
    std::ifstream file(iio_path_);
    if (!file.is_open()) {
        std::cerr << "[ERRO] Falha ao abrir arquivo IIO: " << iio_path_ << std::endl;
        return -1;
    }

    int value;
    file >> value;
    file.close();
    return value;
}

/**
 * @brief Interpretação digital do sensor
 *
 * Compara o valor lido com o threshold definido.
 *
 * @return true se detectou vibração, false caso contrário
 */
bool SW420::read() {
    int raw_value = readRaw();
    if (raw_value == -1) {
        return false;
    }
    return (raw_value >= threshold_);
}
