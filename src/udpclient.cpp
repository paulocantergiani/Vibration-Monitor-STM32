/**
 * @file udpclient.cpp
 * @brief Implementação do cliente UDP para transmissão de dados
 * @author Turma de Programação Aplicada - IME
 * @date 2025
 */

#include "udpclient.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>

/**
 * @brief Construtor
 *
 * Inicializa os parâmetros de conexão UDP.
 */
UDPClient::UDPClient(const std::string &server_ip, int server_port, const std::string &sensor_id)
    : server_ip_(server_ip), server_port_(server_port), sensor_id_(sensor_id),
      sock_fd_(-1), connected_(false) {
    memset(&server_addr_, 0, sizeof(server_addr_));
}

/**
 * @brief Destrutor
 *
 * Fecha o socket se estiver aberto.
 */
UDPClient::~UDPClient() {
    if (sock_fd_ >= 0) {
        close(sock_fd_);
    }
}

/**
 * @brief Inicialização do socket UDP
 *
 * Cria o socket e configura o endereço do servidor.
 */
bool UDPClient::init() {
    // Cria socket UDP
    sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd_ < 0) {
        std::cerr << "[ERRO] Falha ao criar socket UDP" << std::endl;
        return false;
    }

    // Configura endereço do servidor
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(server_port_);

    if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr_.sin_addr) <= 0) {
        std::cerr << "[ERRO] Endereço IP inválido: " << server_ip_ << std::endl;
        close(sock_fd_);
        sock_fd_ = -1;
        return false;
    }

    connected_ = true;
    std::cout << "[INFO] Cliente UDP inicializado para " << server_ip_
              << ":" << server_port_ << std::endl;
    return true;
}

/**
 * @brief Envia dados via UDP
 *
 * Formata os dados em CSV e envia via socket UDP.
 * Formato: "SENSOR_ID,TIMESTAMP,VALUE,UNIT"
 * Exemplo: "SW420_GRUPO_10,2025-10-22T15:30:45,45000,ADC"
 */
bool UDPClient::sendData(int value, const std::string &unit) {
    if (!connected_ || sock_fd_ < 0) {
        std::cerr << "[ERRO] Cliente UDP não inicializado" << std::endl;
        return false;
    }

    // Monta mensagem no formato CSV
    std::stringstream ss;
    ss << sensor_id_ << ","
       << getTimestamp() << ","
       << value << ","
       << unit;

    std::string message = ss.str();

    // Envia via UDP
    ssize_t sent = sendto(sock_fd_, message.c_str(), message.length(), 0,
                          (struct sockaddr*)&server_addr_, sizeof(server_addr_));

    if (sent < 0) {
        std::cerr << "[ERRO] Falha ao enviar dados UDP" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Verifica estado da conexão
 */
bool UDPClient::isConnected() const {
    return connected_ && sock_fd_ >= 0;
}

/**
 * @brief Obtém timestamp atual
 *
 * Retorna timestamp no formato ISO 8601: YYYY-MM-DDTHH:MM:SS
 */
std::string UDPClient::getTimestamp() const {
    time_t now = time(nullptr);
    struct tm *tm_info = localtime(&now);

    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << (tm_info->tm_year + 1900) << "-"
       << std::setw(2) << (tm_info->tm_mon + 1) << "-"
       << std::setw(2) << tm_info->tm_mday << "T"
       << std::setw(2) << tm_info->tm_hour << ":"
       << std::setw(2) << tm_info->tm_min << ":"
       << std::setw(2) << tm_info->tm_sec;

    return ss.str();
}
