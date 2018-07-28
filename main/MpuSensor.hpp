#pragma once

#include <array>
#include "I2CDevice.hpp"
#include "glm/glm.hpp"


class MpuSensor
{
public:
	MpuSensor();

	void poll();

	bool popFifoEntry(glm::vec3& accelState, glm::vec3& gyroState, glm::vec3& compassState);

	size_t fifoEntries() const;

private:
	I2CDevice _device;
	std::array<uint8_t, 512> _fifoBuffer;
	size_t _fifoEntries;
	glm::vec3 _accelBias;
	glm::vec3 _gyroBias;
	glm::vec3 _compassBias;
	
	template <typename T>
	void write(uint8_t address, T data) const;

	template <typename T>
	void compassWrite(uint8_t address, T data) const;

	template <typename T>
	T read(uint8_t address) const;

	template <typename T>
	T compassRead(uint8_t address) const;

	void initialize();

	void calibrate();
};


inline size_t MpuSensor::fifoEntries() const
{
	return _fifoEntries;
}
