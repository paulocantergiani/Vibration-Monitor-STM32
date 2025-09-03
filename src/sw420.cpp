#include "sw420.h"
#include <iostream>
#include <stdexcept>

SW420::SW420(const std::string &iio_path)
    : iio_path_(iio_path), threshold_(65535) {}

void SW420::init() {
    std::ifstream file(iio_path_);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open IIO file: " + iio_path_);
    }
    file.close();
    std::cout << "[INFO] SW420 initialized on " << iio_path_ << std::endl;
}

int SW420::readRaw() {
    std::ifstream file(iio_path_);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open IIO file: " << iio_path_ << std::endl;
        return -1;
    }
    
    int value;
    file >> value;
    file.close();
    return value;
}

bool SW420::read() {
    int raw_value = readRaw();
    if (raw_value == -1) {
        return false;
    }
    return (raw_value >= threshold_);
}
