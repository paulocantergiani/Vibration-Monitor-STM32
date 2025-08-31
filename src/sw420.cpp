/**
 * @file SW420VibrationSensor.cpp
 * @brief Implementação da classe SW420VibrationSensor
 * @author Projeto Monitoramento Inteligente de Carga
 * @date 2025-08-31
 * @version 1.0
 */

#include "sw420.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <thread>

SW420VibrationSensor::SW420VibrationSensor(int gpio_pin, unsigned int debounce_ms)
    : m_gpio_pin(gpio_pin)
    , m_debounce_ms(debounce_ms)
    , m_is_initialized(false)
    , m_last_state(false)
    , m_vibration_count(0)
{
    // Constrói os caminhos do sistema de arquivos GPIO
    m_gpio_path = "/sys/class/gpio/gpio" + std::to_string(m_gpio_pin);
    m_gpio_value_path = m_gpio_path + "/value";
    
    // Inicializa os timestamps
    m_last_change_time = std::chrono::steady_clock::now();
    m_last_vibration_time = std::chrono::steady_clock::now();
}

SW420VibrationSensor::~SW420VibrationSensor() {
    cleanupGPIO();
}

bool SW420VibrationSensor::initialize() {
    if (m_is_initialized) {
        return true;
    }
    
    // Tenta configurar o GPIO
    if (!configureGPIO()) {
        std::cerr << "Erro: Falha ao configurar GPIO " << m_gpio_pin << std::endl;
        return false;
    }
    
    // Aguarda um breve período para estabilização
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Lê o estado inicial
    int initial_value = readRawValue();
    if (initial_value == -1) {
        std::cerr << "Erro: Falha na leitura inicial do GPIO" << std::endl;
        cleanupGPIO();
        return false;
    }
    
    m_last_state = (initial_value == 1);
    m_is_initialized = true;
    
    std::cout << "SW420 inicializado com sucesso no GPIO " << m_gpio_pin 
              << " (estado inicial: " << (m_last_state ? "HIGH" : "LOW") << ")" << std::endl;
    
    return true;
}

bool SW420VibrationSensor::configureGPIO() {
    // Exporta o GPIO
    std::ofstream export_file("/sys/class/gpio/export");
    if (!export_file.is_open()) {
        std::cerr << "Erro: Não foi possível abrir /sys/class/gpio/export" << std::endl;
        return false;
    }
    
    export_file << m_gpio_pin;
    export_file.close();
    
    // Aguarda a criação do diretório
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Configura como entrada
    std::ofstream direction_file(m_gpio_path + "/direction");
    if (!direction_file.is_open()) {
        std::cerr << "Erro: Não foi possível configurar direção do GPIO " << m_gpio_pin << std::endl;
        return false;
    }
    
    direction_file << "in";
    direction_file.close();
    
    // Configura pull-up/pull-down se disponível
    std::ofstream edge_file(m_gpio_path + "/edge");
    if (edge_file.is_open()) {
        edge_file << "both";  // Detecta bordas de subida e descida
        edge_file.close();
    }
    
    return true;
}

bool SW420VibrationSensor::cleanupGPIO() {
    if (!m_is_initialized) {
        return true;
    }
    
    // Remove a exportação do GPIO
    std::ofstream unexport_file("/sys/class/gpio/unexport");
    if (unexport_file.is_open()) {
        unexport_file << m_gpio_pin;
        unexport_file.close();
    }
    
    m_is_initialized = false;
    return true;
}

int SW420VibrationSensor::readRawValue() {
    std::ifstream value_file(m_gpio_value_path);
    if (!value_file.is_open()) {
        return -1;
    }
    
    std::string value_str;
    std::getline(value_file, value_str);
    value_file.close();
    
    if (value_str.empty()) {
        return -1;
    }
    
    return (value_str[0] == '1') ? 1 : 0;
}

bool SW420VibrationSensor::isInitialized() const {
    return m_is_initialized;
}

bool SW420VibrationSensor::readVibration() {
    if (!m_is_initialized) {
        std::cerr << "Erro: Sensor não inicializado" << std::endl;
        return false;
    }
    
    int raw_value = readRawValue();
    if (raw_value == -1) {
        std::cerr << "Erro na leitura do GPIO" << std::endl;
        return m_last_state;
    }
    
    bool current_state = (raw_value == 1);
    auto current_time = std::chrono::steady_clock::now();
    
    // Aplica debounce - só considera mudança se passou tempo suficiente
    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time - m_last_change_time).count();
    
    if (current_state != m_last_state && time_diff >= m_debounce_ms) {
        m_last_state = current_state;
        m_last_change_time = current_time;
        
        // Se detectou vibração (transição para HIGH), incrementa contador
        if (current_state) {
            m_vibration_count++;
            m_last_vibration_time = current_time;
            
            std::cout << "Vibração detectada! Count: " << m_vibration_count << std::endl;
        }
    }
    
    return current_state;
}

bool SW420VibrationSensor::hasStateChanged() {
    if (!m_is_initialized) {
        return false;
    }
    
    bool previous_state = m_last_state;
    readVibration();  // Atualiza o estado atual
    
    return (previous_state != m_last_state);
}

unsigned long SW420VibrationSensor::getVibrationCount() const {
    return m_vibration_count;
}

long SW420VibrationSensor::getTimeSinceLastVibration() const {
    auto current_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time - m_last_vibration_time).count();
}

void SW420VibrationSensor::resetVibrationCount() {
    m_vibration_count = 0;
    std::cout << "Contador de vibrações resetado" << std::endl;
}

void SW420VibrationSensor::setDebounceTime(unsigned int debounce_ms) {
    m_debounce_ms = debounce_ms;
    std::cout << "Tempo de debounce definido para " << debounce_ms << "ms" << std::endl;
}

unsigned int SW420VibrationSensor::getDebounceTime() const {
    return m_debounce_ms;
}

std::string SW420VibrationSensor::getStatusInfo() const {
    std::ostringstream info;
    info << "SW420 Status:\n";
    info << "  Inicializado: " << (m_is_initialized ? "Sim" : "Não") << "\n";
    info << "  GPIO: " << m_gpio_pin << "\n";
    info << "  Estado atual: " << (m_last_state ? "VIBRANDO" : "PARADO") << "\n";
    info << "  Vibrações detectadas: " << m_vibration_count << "\n";
    info << "  Tempo desde última vibração: " << getTimeSinceLastVibration() << "ms\n";
    info << "  Debounce: " << m_debounce_ms << "ms";
    
    return info.str();
}

std::string SW420VibrationSensor::getSensorId() const {
    return "SW420_VIBRATION";
}

std::string SW420VibrationSensor::getUnit() const {
    return "boolean";
}