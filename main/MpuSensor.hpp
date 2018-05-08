#pragma once


#include "I2CDevice.hpp"
#include "glm/glm.hpp"


class MpuSensor
{
public:
	MpuSensor();

	void poll();

	inline glm::vec3 accelState() const;
	inline glm::vec3 gyroState() const;

private:
	I2CDevice _device;
	glm::vec3 _accelState;
	glm::vec3 _gyroState;
	glm::vec3 _accelBias;
	
	template <typename T>
	void write(uint8_t address, T data) const;

	template <typename T>
	T read(uint8_t address) const;

	void initialize();
	void calibrate();

	glm::vec3 readAccelState() const;
	glm::vec3 readGyroState() const;

};


inline glm::vec3 MpuSensor::accelState() const
{
	return _accelState;
}


inline glm::vec3 MpuSensor::gyroState() const
{
	return _gyroState;
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
