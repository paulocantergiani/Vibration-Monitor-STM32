#ifndef SW420_H
#define SW420_H

#include <string>
#include <fstream>

/**
 * @brief SW420 vibration sensor driver
 */
class SW420 {
public:
    /**
     * @brief Constructor
     * @param iio_path Path to IIO channel file (e.g., "/sys/bus/iio/devices/iio:device0/in_voltage13_raw")
     */
    SW420(const std::string &iio_path);

    /**
     * @brief Initialize the sensor
     */
    void init();

    /**
     * @brief Read the raw analog value
     * @return Raw ADC value
     */
    int readRaw();

    /**
     * @brief Read the current sensor state (digital interpretation)
     * @return true if vibration detected (value > threshold), false otherwise
     */
    bool read();

private:
    std::string iio_path_;
    int threshold_;
};

#endif // SW420_H
