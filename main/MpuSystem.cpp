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
	_compassSensor.poll();

	glm::vec3 accelState;
	glm::vec3 gyroState;
	glm::vec3 compassState = _compassSensor.state();

	while (_mpuSensor.popFifoEntry(accelState, gyroState))
	{
		printf("%f %f %f\n", compassState.x, compassState.y, compassState.z);

		MadgwickAHRSupdate(
			gyroState.x, gyroState.y, gyroState.z,
			accelState.x, accelState.y, accelState.z,
			compassState.y, compassState.x, compassState.z);
	}
	

/*	glm::vec3 accelState = _mpuSensor.accelState();
	glm::vec3 gyroState = _mpuSensor.gyroState();
	glm::vec3 compassState = _compassSensor.state();

	TickType_t time = xTaskGetTickCount();
	float elapsedSeconds = (float)(time - _lastUpdate) / configTICK_RATE_HZ;

	MadgwickAHRSupdate(
		gyroState.x, gyroState.y, gyroState.z,
		accelState.x, accelState.y, accelState.z,
		compassState.x, compassState.y, compassState.z, 
		elapsedSeconds);

	_lastUpdate = time;

	_rotation = compassState;

	//float roll = atan2f(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2);
	//float pitch = asinf(-2.0f * (q1 * q3 - q0 * q2));
	//float yaw = atan2f(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3);
	*/
	_quaternion = glm::quat(q0, q1, q2, q3);
}