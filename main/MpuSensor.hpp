#pragma once


#include "I2CDevice.hpp"
#include "MPU9250.hpp"
#include "freertos/FreeRTOS.h"
#include "glm/glm.hpp"
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include "glm/gtx/quaternion.hpp"


class MpuSensor
{
public:
	static MpuSensor create();

	void poll();

	glm::quat quaternion() const;	

private:
	TickType_t _lastUpdate;

	I2CDevice _mpuDevice;
	I2CDevice _compassDevice;

	GyroScale _gyroScale;
	AccelScale _accelScale;
	CompassScale _compassScale;

	glm::vec3 _compassBias;

	glm::quat _quaternion;

	MpuSensor();
	
	template <typename T>
	void mpuWrite(uint8_t address, T data) const;

	template <typename T>
	T mpuRead(uint8_t address) const;

	template <typename T>
	void compassWrite(uint8_t address, T data) const;

	template <typename T>
	T compassRead(uint8_t address) const;

	void initializeMpu();
	void calibrateMpu();
	void initializeCompass();

	glm::vec3 readAccelState() const;
	glm::vec3 readGyroState() const;
	glm::vec3 readCompassState() const;

};


template <typename T>
void MpuSensor::mpuWrite(uint8_t address, T data) const
{
	_mpuDevice.write(gsl::span<const uint8_t>(&address, 1), gsl::span<const uint8_t>(reinterpret_cast<uint8_t*>(&data), sizeof(T)));
}


template <typename T>
T MpuSensor::mpuRead(uint8_t address) const
{
	T output;
	_mpuDevice.read(gsl::span<const uint8_t>(&address, 1), gsl::span<uint8_t>(reinterpret_cast<uint8_t*>(&output), sizeof(T)));

	return output;
}


template <typename T>
void MpuSensor::compassWrite(uint8_t address, T data) const
{
	_compassDevice.write(gsl::span<const uint8_t>(&address, 1), gsl::span<const uint8_t>(reinterpret_cast<uint8_t*>(&data), sizeof(T)));
}


template <typename T>
T MpuSensor::compassRead(uint8_t address) const
{
	T output;
	_compassDevice.read(gsl::span<const uint8_t>(&address, 1), gsl::span<uint8_t>(reinterpret_cast<uint8_t*>(&output), sizeof(T)));

	return output;
}



