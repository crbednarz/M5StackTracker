#pragma once

#include "MpuSensor.hpp"
#include "CompassSensor.hpp"
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

private:	
	TickType_t _lastUpdate;
	MpuSensor _mpuSensor;
	CompassSensor _compassSensor;
	glm::quat _quaternion;
};


inline glm::quat MpuSystem::quaternion() const
{
	return _quaternion;
}