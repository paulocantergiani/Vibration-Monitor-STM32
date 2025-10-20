#ifndef SW420_H
#define SW420_H

#include <string>
#include <fstream>

/**
 * @class SW420
 * @brief Driver para sensor de vibração SW-420
 *
 * Esta classe implementa a leitura do sensor de vibração SW-420 através
 * da interface IIO (Industrial I/O) do Linux embarcado.
 *
 * O sensor detecta vibrações e movimentações, sendo útil para:
 * - Monitoramento de cargas em transporte
 * - Detecção de tentativas de violação
 * - Sistemas de alarme por movimento
 */
class SW420 {
public:
    /**
     * @brief Construtor da classe SW420
     * @param iio_path Caminho para o arquivo de canal IIO
     *                 (ex: "/sys/bus/iio/devices/iio:device0/in_voltage13_raw")
     */
    SW420(const std::string &iio_path);

    /**
     * @brief Inicializa o sensor
     *
     * Verifica se o arquivo IIO existe e está acessível.
     * Deve ser chamado antes de realizar leituras.
     *
     * @throw std::runtime_error se não conseguir acessar o arquivo IIO
     */
    void init();

    /**
     * @brief Realiza leitura do valor bruto analógico
     *
     * Lê o valor direto do ADC sem processamento.
     *
     * @return Valor bruto do ADC (0-65535) ou -1 em caso de erro
     */
    int readRaw();

    /**
     * @brief Lê o estado atual do sensor (interpretação digital)
     *
     * Compara o valor lido com o threshold para determinar
     * se há vibração detectada.
     *
     * @return true se vibração detectada (valor >= threshold), false caso contrário
     */
    bool read();

private:
    std::string iio_path_;  ///< Caminho do arquivo IIO
    int threshold_;         ///< Limiar de detecção de vibração (padrão: 32000)
};

#endif // SW420_H
