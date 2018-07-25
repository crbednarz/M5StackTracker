#pragma once

#include <array>
#include "I2CDevice.hpp"
#include "glm/glm.hpp"


class MpuSensor
{
public:
	MpuSensor();

	void poll();

	bool popFifoEntry(glm::vec3& accelState, glm::vec3& gyroState);

	size_t fifoEntries() const;

private:
	I2CDevice _device;
	std::array<uint8_t, 512> _fifoBuffer;
	size_t _fifoEntries;
	glm::vec3 _accelBias;
	
	template <typename T>
	void write(uint8_t address, T data) const;

	template <typename T>
	T read(uint8_t address) const;


	void initialize();
	void calibrate();
};


inline size_t MpuSensor::fifoEntries() const
{
	return _fifoEntries;
}


template <typename T>
void MpuSensor::write(uint8_t address, T data) const
{
	_device.write(gsl::span<const uint8_t>(&address, 1), gsl::span<const uint8_t>(reinterpret_cast<uint8_t*>(&data), sizeof(T)));
}


template <typename T>
T MpuSensor::read(uint8_t address) const
{
	T output;
	_device.read(gsl::span<const uint8_t>(&address, 1), gsl::span<uint8_t>(reinterpret_cast<uint8_t*>(&output), sizeof(T)));

	return output;
}
