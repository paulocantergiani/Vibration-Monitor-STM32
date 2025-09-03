#include "sw420.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <thread>

SW420VibrationSensor::SW420VibrationSensor(int gpio_pin, int gpio_chip, unsigned int debounce_ms)
    : m_gpio_pin(gpio_pin)
    , m_gpio_chip(gpio_chip)
    , m_value_path("")
    , m_is_initialized(false)
    , m_last_state(false)
    , m_debounce_ms(debounce_ms)
    , m_vibration_count(0)
{
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
    
    if (!configureGPIO()) {
        std::cerr << "Erro: Falha ao configurar GPIO " << m_gpio_pin << std::endl;
        return false;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
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
    std::string gpio_str = std::to_string(m_gpio_pin);
    
    std::ofstream export_file("/sys/class/gpio/export");
    export_file << gpio_str;
    export_file.close();
    
    usleep(100000);
    
    std::string direction_path = "/sys/class/gpio/gpio" + gpio_str + "/direction";
    std::ofstream direction_file(direction_path);
    direction_file << "in";
    direction_file.close();
    
    m_value_path = "/sys/class/gpio/gpio" + gpio_str + "/value";
    
    return true;
}

bool SW420VibrationSensor::cleanupGPIO() {
    if (!m_is_initialized) {
        return true;
    }
    
    std::ofstream unexport_file("/sys/class/gpio/unexport");
    unexport_file << m_gpio_pin;
    unexport_file.close();
    
    m_is_initialized = false;
    return true;
}

int SW420VibrationSensor::readRawValue() {
    if (!m_is_initialized) {
        return -1;
    }
    
    std::ifstream value_file(m_value_path);
    char value_char;
    value_file >> value_char;
    value_file.close();
    
    return (value_char == '1') ? 1 : 0;
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
    
    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time - m_last_change_time).count();
    
    if (current_state != m_last_state && time_diff >= m_debounce_ms) {
        m_last_state = current_state;
        m_last_change_time = current_time;
        
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
    readVibration();
    
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