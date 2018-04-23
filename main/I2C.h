#ifndef I2C_H
#define I2C_H


#include <stdint.h>
#include "driver/i2c.h"
#include "PeripheralConfig.h"


void I2C_Initialize();

void I2C_WriteBytes(uint8_t address, const uint8_t* data, size_t length);

void I2C_ReadBytes(uint8_t address, uint8_t* data, size_t length);

void I2C_WriteWithHeader(uint8_t address, const uint8_t* header, size_t headerLength, const uint8_t* data, size_t dataLength);

void I2C_ReadWithHeader(uint8_t address, const uint8_t* header, size_t headerLength, uint8_t* data, size_t dataLength);

#endif