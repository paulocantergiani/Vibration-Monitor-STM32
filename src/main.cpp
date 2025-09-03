#include "sw420.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // IIO path for analog reading of D4 pin
    const std::string iio_path = "/sys/bus/iio/devices/iio:device0/in_voltage13_raw";

    SW420 sensor(iio_path);

    try {
        sensor.init();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::cout << "[INFO] Monitoring SW420 vibration sensor..." << std::endl;

    while (true) {
        if (sensor.read()) {
            std::cout << "[ALERT] Vibration detected!" << std::endl;
        } else {
            std::cout << "[INFO] No vibration." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
