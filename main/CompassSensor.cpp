#include "CompassSensor.hpp"
#include "MPU9250.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


CompassSensor::CompassSensor() :
	_device(AK8963_ADDRESS),
	_bias(1.0f),
	_state(0.0f)
{
	initialize();
}


void CompassSensor::poll()
{
	uint8_t header = AK8963_XOUT_L;
	std::array<uint8_t, 7> output;
	_device.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(output.data(), output.size()));

	glm::vec3 state;
	state.x = reinterpret_cast<int16_t&>(output[0]);
	state.y = reinterpret_cast<int16_t&>(output[2]);
	state.z = reinterpret_cast<int16_t&>(output[4]);

	state *= 10.0f * 4912.0f / 32760.0f;
	
	_state = state * _bias;
}


void CompassSensor::initialize()
{
	write<uint8_t>(AK8963_CNTL, 0x00);
    vTaskDelay(10 / portTICK_RATE_MS);
    write<uint8_t>(AK8963_CNTL, 0x0F);
    vTaskDelay(10 / portTICK_RATE_MS);

	auto bias = read<glm::u8vec3>(AK8963_ASAX);
	_bias = (glm::vec3(bias) - 128.0f) / 256.0f + 1.0f;

	write<uint8_t>(AK8963_CNTL, 0x00);
	vTaskDelay(10 / portTICK_RATE_MS);

	write<uint8_t>(AK8963_CNTL, (static_cast<uint8_t>(CompassScale::MFS_16BITS) << 4) | 0x02);

	vTaskDelay(10 / portTICK_RATE_MS);
	poll();
}
