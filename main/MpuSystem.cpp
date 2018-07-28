#include "MpuSystem.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MadgwickAHRS.h"


MpuSystem::MpuSystem() :
	_lastUpdate(xTaskGetTickCount())
{
	
}


void MpuSystem::poll()
{
	_mpuSensor.poll();

	glm::vec3 accelState;
	glm::vec3 gyroState;
	glm::vec3 compassState;

	while (_mpuSensor.popFifoEntry(accelState, gyroState, compassState))
	{
		//printf("A: %f %f %f\n", accelState.x, accelState.y, accelState.z);
		//printf("G: %f %f %f\n", gyroState.x, gyroState.y, gyroState.z);
		//printf("C: %f %f %f\n", compassState.x, compassState.y, compassState.z);

		MadgwickAHRSupdate(
			gyroState.x, gyroState.y, gyroState.z,
			accelState.x, accelState.y, accelState.z,
			compassState.y, compassState.x, compassState.z);
	}
	

	_quaternion = glm::quat(q0, q1, q2, q3);
}