#include "DistanceSensor.hpp"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"


DistanceSensor::DistanceSensor()
{
	VL53L0X_Dev_t device;
	VL53L0X_DeviceInfo_t deviceInfo;

	device.I2cDevAddr = 0x52;
	device.comms_type = 1;
	device.comms_speed_khz = 400;

	VL53L0X_DataInit(&device);
	auto status = VL53L0X_GetDeviceInfo(&device, &deviceInfo);

}