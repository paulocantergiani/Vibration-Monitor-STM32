# STM32 Vibration Monitor

A vibration monitoring system using STM32MP1 and SW-420 sensor.

## What it does

Monitors vibrations and movements using a digital vibration sensor. Sends data to a server via UDP network connection.

## Files

- `src/sw420.h` - Sensor class header
- `src/sw420.cpp` - Sensor class implementation  
- `src/main.cpp` - Main program
- `Makefile` - Build automation
- `Doxyfile` - Documentation config

## Requirements

- ARM toolchain: `arm-buildroot-linux-gnueabihf-g++`
- Doxygen (for docs)

## Usage

### Compile
```bash
make
```

### Generate docs
```bash
make docs
```

### Deploy to STM32MP1
```bash
make deploy
```