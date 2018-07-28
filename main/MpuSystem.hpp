#pragma once

#include "MpuSensor.hpp"
#include "freertos/FreeRTOS.h"
#include "glm/glm.hpp"
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/gtx/quaternion.hpp"


class MpuSystem
{
public:
	MpuSystem();
	
	void poll();

	inline glm::quat quaternion() const;

	inline glm::vec3 rotation() const;

private:	
	TickType_t _lastUpdate;
	MpuSensor _mpuSensor;
	glm::quat _quaternion;

	glm::vec3 _rotation;
};


inline glm::quat MpuSystem::quaternion() const
{
	return _quaternion;
}


inline glm::vec3 MpuSystem::rotation() const
{
	return _rotation;
}