#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * @class UDPClient
 * @brief Cliente UDP para envio de dados do sensor
 *
 * Esta classe implementa a comunicação UDP para enviar dados dos sensores
 * para um servidor central de monitoramento.
 *
 * Características:
 * - Protocolo UDP (connectionless)
 * - Formato de dados: CSV (leve e simples)
 * - Timestamp automático
 * - Identificação do sensor/grupo
 */
class UDPClient {
public:
    /**
     * @brief Construtor da classe UDPClient
     * @param server_ip Endereço IP do servidor (ex: "192.168.42.10")
     * @param server_port Porta do servidor (ex: 5000)
     * @param sensor_id Identificador do sensor/grupo (ex: "SW420_GRUPO_10")
     */
    UDPClient(const std::string &server_ip, int server_port, const std::string &sensor_id);

    /**
     * @brief Destrutor - fecha o socket
     */
    ~UDPClient();

    /**
     * @brief Inicializa o socket UDP
     * @return true se sucesso, false caso contrário
     */
    bool init();

    /**
     * @brief Envia dados do sensor via UDP
     * @param value Valor lido do sensor
     * @param unit Unidade de medida (ex: "ADC", "mV", "g")
     * @return true se enviado com sucesso, false caso contrário
     */
    bool sendData(int value, const std::string &unit = "ADC");

    /**
     * @brief Verifica se o cliente está conectado
     * @return true se socket está ativo
     */
    bool isConnected() const;

private:
    std::string server_ip_;     ///< IP do servidor
    int server_port_;           ///< Porta do servidor
    std::string sensor_id_;     ///< Identificador do sensor
    int sock_fd_;               ///< File descriptor do socket
    struct sockaddr_in server_addr_; ///< Estrutura de endereço do servidor
    bool connected_;            ///< Estado da conexão

    /**
     * @brief Obtém timestamp atual em formato ISO 8601
     * @return String com timestamp (ex: "2025-10-22T15:30:45")
     */
    std::string getTimestamp() const;
};

#endif // UDPCLIENT_H
