/**
 * @file SW420VibrationSensor.h
 * @brief Classe para leitura do sensor de vibração SW-420 em sistemas embarcados Linux
 * @author Projeto Monitoramento Inteligente de Carga
 * @date 2025-08-31
 * @version 1.0
 * 
 * Esta classe implementa a interface para leitura do sensor de vibração digital SW-420
 * através do sistema de arquivos GPIO do Linux. O sensor detecta movimentações,
 * impactos ou transporte indevido da carga monitorada.
 */

#ifndef SW420_H
#define SW420_H

#include <string>
#include <chrono>
#include <fstream>

/**
 * @class SW420VibrationSensor
 * @brief Classe responsável pela leitura e processamento do sensor de vibração SW-420
 * 
 * Esta classe encapsula todas as operações necessárias para:
 * - Configurar o GPIO para leitura do sensor
 * - Ler o estado digital do sensor
 * - Processar e filtrar os dados de vibração
 * - Detectar eventos de vibração com debounce
 * - Fornecer estatísticas de vibração
 */
class SW420VibrationSensor {
private:
    int m_gpio_pin;                    ///< Número do pino GPIO usado pelo sensor
    std::string m_gpio_path;           ///< Caminho do GPIO no sistema de arquivos
    std::string m_gpio_value_path;     ///< Caminho para leitura do valor do GPIO
    bool m_is_initialized;             ///< Status de inicialização do sensor
    bool m_last_state;                 ///< Último estado lido do sensor
    
    // Controle de debounce e filtragem
    std::chrono::steady_clock::time_point m_last_change_time; ///< Timestamp da última mudança
    unsigned int m_debounce_ms;        ///< Tempo de debounce em milissegundos
    
    // Estatísticas
    unsigned long m_vibration_count;   ///< Contador total de vibrações detectadas
    std::chrono::steady_clock::time_point m_last_vibration_time; ///< Timestamp da última vibração
    
    /**
     * @brief Configura o pino GPIO para entrada
     * @return true se configurado com sucesso, false caso contrário
     */
    bool configureGPIO();
    
    /**
     * @brief Remove a configuração do GPIO (cleanup)
     * @return true se removido com sucesso, false caso contrário
     */
    bool cleanupGPIO();
    
    /**
     * @brief Lê o valor bruto do GPIO
     * @return 0 ou 1 representando o estado digital, -1 em caso de erro
     */
    int readRawValue();

public:
    /**
     * @brief Construtor da classe SW420VibrationSensor
     * @param gpio_pin Número do pino GPIO conectado ao sensor (padrão: 4)
     * @param debounce_ms Tempo de debounce em milissegundos (padrão: 50ms)
     */
    explicit SW420VibrationSensor(int gpio_pin = 4, unsigned int debounce_ms = 50);
    
    /**
     * @brief Destrutor da classe
     * Realiza a limpeza dos recursos GPIO
     */
    ~SW420VibrationSensor();
    
    /**
     * @brief Inicializa o sensor e configura o GPIO
     * @return true se inicializado com sucesso, false caso contrário
     */
    bool initialize();
    
    /**
     * @brief Verifica se o sensor está inicializado e funcionando
     * @return true se inicializado, false caso contrário
     */
    bool isInitialized() const;
    
    /**
     * @brief Lê o estado atual do sensor com filtragem de debounce
     * @return true se vibração detectada, false caso contrário
     */
    bool readVibration();
    
    /**
     * @brief Verifica se houve mudança no estado desde a última leitura
     * @return true se houve mudança, false caso contrário
     */
    bool hasStateChanged();
    
    /**
     * @brief Obtém o número total de vibrações detectadas desde a inicialização
     * @return Contador de vibrações
     */
    unsigned long getVibrationCount() const;
    
    /**
     * @brief Obtém o tempo decorrido desde a última vibração detectada
     * @return Tempo em milissegundos desde a última vibração
     */
    long getTimeSinceLastVibration() const;
    
    /**
     * @brief Reseta o contador de vibrações
     */
    void resetVibrationCount();
    
    /**
     * @brief Define um novo tempo de debounce
     * @param debounce_ms Novo tempo de debounce em milissegundos
     */
    void setDebounceTime(unsigned int debounce_ms);
    
    /**
     * @brief Obtém o tempo de debounce atual
     * @return Tempo de debounce em milissegundos
     */
    unsigned int getDebounceTime() const;
    
    /**
     * @brief Obtém informações de status do sensor em formato string
     * @return String com informações de status
     */
    std::string getStatusInfo() const;
    
    /**
     * @brief Obtém o identificador do sensor para protocolo de comunicação
     * @return String identificadora do sensor
     */
    std::string getSensorId() const;
    
    /**
     * @brief Obtém a unidade de medida do sensor
     * @return String com a unidade (boolean para digital)
     */
    std::string getUnit() const;
};

#endif // SW420_H