#ifndef SW420_H
#define SW420_H

#include <string>
#include <chrono>
#include <fstream>
#include <cstdint>


class SW420VibrationSensor {
private:
    int m_gpio_pin;
    int m_gpio_chip;
    std::string m_value_path;
    bool m_is_initialized;
    bool m_last_state;
    
    std::chrono::steady_clock::time_point m_last_change_time;
    unsigned int m_debounce_ms;
    
    unsigned long m_vibration_count;
    std::chrono::steady_clock::time_point m_last_vibration_time;
    
    bool configureGPIO();

    bool cleanupGPIO();
    
    int readRawValue();

public:
    explicit SW420VibrationSensor(int gpio_pin = 4, int gpio_chip = 0, unsigned int debounce_ms = 50);
    
    ~SW420VibrationSensor();
    
    bool initialize();
    
    bool isInitialized() const;
    
    bool readVibration();
    
    bool hasStateChanged();
    
    unsigned long getVibrationCount() const;
    
    long getTimeSinceLastVibration() const;
    
    void resetVibrationCount();
    
    void setDebounceTime(unsigned int debounce_ms);
    
    unsigned int getDebounceTime() const;
    
    std::string getStatusInfo() const;
    
    std::string getSensorId() const;
    
    std::string getUnit() const;
};

#endif // SW420_H