#pragma once


#include <stdint.h>
#include "gsl.hpp"


class I2CDevice
{
public:
	//! Static initialization of I2C
	static void initialize();

	I2CDevice(uint8_t slaveAddress);

	void write(gsl::span<const uint8_t> data) const;

	void read(gsl::span<uint8_t> output) const;

	void write(gsl::span<const uint8_t> header, gsl::span<const uint8_t> data) const;

	void read(gsl::span<const uint8_t> header, gsl::span<uint8_t> output) const;

private:
	uint8_t _slaveAddress;
	
};

