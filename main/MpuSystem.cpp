#include "MpuSystem.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MadgwickAHRS.h"
#include "BNO055.hpp"


MpuSystem::MpuSystem() :
	_device(BNO055_ADDRESS)
{
	// Enter config mode
	_device.write<uint8_t>(BNO055_OPR_MODE, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);

	// Reset device
	_device.write<uint8_t>(BNO055_SYS_TRIGGER, 0x20);
	vTaskDelay(100 / portTICK_RATE_MS);

	while (_device.read<uint8_t>(BNO055_CHIP_ID) != 160) {
		vTaskDelay(10 / portTICK_RATE_MS);
	}

	// Set power mode to normal
	_device.write<uint8_t>(BNO055_PWR_MODE, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);

	_device.write<uint8_t>(BNO055_PAGE_ID, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);

	_device.write<uint8_t>(BNO055_SYS_TRIGGER, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);

	// Enter 9-DoF mode
	_device.write<uint8_t>(BNO055_OPR_MODE, 0x0C);
	vTaskDelay(100 / portTICK_RATE_MS);
}


void MpuSystem::poll()
{
	//_mpuSensor.poll();

	const double QUAT_SCALE = (1.0 / (1 << 14));

	auto rawQuat = _device.read<glm::i16vec4>(BNO055_QUATERNION_DATA);
	auto status = _device.read<uint8_t>(BNO055_CALIB_STAT);

	printf("BNO055 Status: %i  ", status);
	for (int i = 0; i < 8; i++) 
	{
		printf("%i", (status & 0b10000000) >> 7);
		status <<= 1;
	}
	printf("\n");
	printf("Err: %i\n", _device.read<uint8_t>(BNO055_SYS_ERR));
	//printf("%i\n", id);
	printf("%f %f %f %f\n", rawQuat.x * QUAT_SCALE, rawQuat.y * QUAT_SCALE, rawQuat.z * QUAT_SCALE, rawQuat.w * QUAT_SCALE);
	/*glm::vec3 accelState;
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
	*/

	_quaternion = glm::quat(
		rawQuat.x * QUAT_SCALE, 
		rawQuat.y * QUAT_SCALE, 
		rawQuat.z * QUAT_SCALE, 
		rawQuat.w * QUAT_SCALE);
	//_quaternion = glm::quat(q0, q1, q2, q3);
}