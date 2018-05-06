#pragma once


#include "I2CDevice.hpp"
#include "glm/glm.hpp"


class CompassSensor
{
public:
	CompassSensor();

	void poll();

	inline glm::vec3 state() const;

private:
	I2CDevice _device;
	glm::vec3 _bias;
	glm::vec3 _state;

	void initialize();
	
	template <typename T>
	void write(uint8_t address, T data) const;

	template <typename T>
	T read(uint8_t address) const;
};


inline glm::vec3 CompassSensor::state() const
{
	return _state;
}


template <typename T>
void CompassSensor::write(uint8_t address, T data) const
{
	_device.write(gsl::span<const uint8_t>(&address, 1), gsl::span<const uint8_t>(reinterpret_cast<uint8_t*>(&data), sizeof(T)));
}


template <typename T>
T CompassSensor::read(uint8_t address) const
{
	T output;
	_device.read(gsl::span<const uint8_t>(&address, 1), gsl::span<uint8_t>(reinterpret_cast<uint8_t*>(&output), sizeof(T)));

	return output;
}
