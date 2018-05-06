#include "MpuSensor.hpp"
#include "MadgwickAHRS.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "PeripheralConfig.hpp"
#include "glm/gtc/matrix_transform.hpp"


MpuSensor MpuSensor::create()
{
	MpuSensor sensor;
	sensor.calibrateMpu();
	sensor.initializeMpu();
	sensor.initializeCompass();
	return sensor;
}


MpuSensor::MpuSensor() :
	_lastUpdate(xTaskGetTickCount()),
	_mpuDevice(MPU9250_ADDRESS),
	_compassDevice(AK8963_ADDRESS),
	_gyroScale(GyroScale::GFS_250DPS),
	_accelScale(AccelScale::AFS_2G),
	_compassScale(CompassScale::MFS_16BITS)
{

}


void MpuSensor::poll()
{
	glm::vec3 accelState = readAccelState();
	glm::vec3 gyroState = readGyroState();
	glm::vec3 compassState = readCompassState();

	TickType_t time = xTaskGetTickCount();
	float elapsedSeconds = (float)(time - _lastUpdate) / configTICK_RATE_HZ;

	MadgwickAHRSupdate(
		gyroState.x, gyroState.y, gyroState.z,
		accelState.x, accelState.y, accelState.z,
		compassState.x, compassState.y, compassState.z, 
		elapsedSeconds);

	_lastUpdate = time;

	//float roll = atan2f(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2);
	//float pitch = asinf(-2.0f * (q1 * q3 - q0 * q2));
	//float yaw = atan2f(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3);
	
	_quaternion = glm::quat(q0, q1, q2, q3);
}


glm::quat MpuSensor::quaternion() const
{
	return _quaternion;
}


void MpuSensor::initializeMpu()
{
	// Reference: https://www.invensense.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf

	mpuWrite<uint8_t>(PWR_MGMT_1, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);

	mpuWrite<uint8_t>(PWR_MGMT_1, 0x01);
	vTaskDelay(200 / portTICK_RATE_MS);

	mpuWrite<uint8_t>(CONFIG, 0x03);
	mpuWrite<uint8_t>(SMPLRT_DIV, 0x04);
 
	uint8_t gyroConfig = mpuRead<uint8_t>(GYRO_CONFIG);
	gyroConfig &= ~(0x02 | 0x18);
	gyroConfig |= static_cast<uint8_t>(_gyroScale) << 3;
	mpuWrite<uint8_t>(GYRO_CONFIG, gyroConfig);

	uint8_t accelConfig = mpuRead<uint8_t>(ACCEL_CONFIG);
	accelConfig &= ~0x18;
	accelConfig |= static_cast<uint8_t>(_accelScale) << 3;
	mpuWrite<uint8_t>(ACCEL_CONFIG, accelConfig);

	uint8_t accelConfig2 = mpuRead<uint8_t>(ACCEL_CONFIG2);
	accelConfig2 &= ~0x0F;
	accelConfig2 |= 0x03;
	mpuWrite<uint8_t>(ACCEL_CONFIG2, accelConfig2);
	
	mpuWrite<uint8_t>(INT_PIN_CFG, 0x22);
	mpuWrite<uint8_t>(INT_ENABLE, 0x01);
	
	vTaskDelay(100 / portTICK_RATE_MS);
}


void MpuSensor::calibrateMpu()
{
	mpuWrite<uint8_t>(PWR_MGMT_1, 0x80);
	vTaskDelay(100 / portTICK_RATE_MS);

	mpuWrite<uint8_t>(PWR_MGMT_1, 0x01);
	mpuWrite<uint8_t>(PWR_MGMT_2, 0x00);
	vTaskDelay(200 / portTICK_RATE_MS);

	mpuWrite<uint8_t>(INT_ENABLE, 0x00); 
	mpuWrite<uint8_t>(FIFO_EN, 0x00);
	mpuWrite<uint8_t>(PWR_MGMT_1, 0x00);
	mpuWrite<uint8_t>(I2C_MST_CTRL, 0x00);
	mpuWrite<uint8_t>(USER_CTRL, 0x00);
	mpuWrite<uint8_t>(USER_CTRL, 0x0C);
	vTaskDelay(15 / portTICK_RATE_MS);

	mpuWrite<uint8_t>(CONFIG, 0x01);
	mpuWrite<uint8_t>(SMPLRT_DIV, 0x00);
	mpuWrite<uint8_t>(GYRO_CONFIG, 0x00);
	mpuWrite<uint8_t>(ACCEL_CONFIG, 0x00);

	mpuWrite<uint8_t>(USER_CTRL, 0x40);
	mpuWrite<uint8_t>(FIFO_EN, 0x78);
	vTaskDelay(40 / portTICK_RATE_MS);

	mpuWrite<uint8_t>(FIFO_EN, 0x00);
	auto rawFifoCount = mpuRead<glm::u8vec2>(FIFO_COUNTH);
	auto fifoCount = (static_cast<uint16_t>(rawFifoCount.x) << 8) | rawFifoCount.y;
	auto packetCount = fifoCount / 12;

	glm::i16vec3 accelBiasSum(0);
	glm::i16vec3 gyroBiasSum(0);
	for (int i = 0; i < packetCount; i++)
	{
		std::array<uint8_t, 12> rawPacket;
		uint8_t header = FIFO_R_W;

		_mpuDevice.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(rawPacket.data(), rawPacket.size()));
		
		glm::i16vec3 accelReading;
		accelReading.x = (static_cast<int16_t>(rawPacket[0]) << 8) | rawPacket[1];
		accelReading.y = (static_cast<int16_t>(rawPacket[2]) << 8) | rawPacket[3];
		accelReading.z = (static_cast<int16_t>(rawPacket[4]) << 8) | rawPacket[5];

		glm::i16vec3 gyroReading;
		gyroReading.x = (static_cast<int16_t>(rawPacket[6]) << 8) | rawPacket[7];
		gyroReading.y = (static_cast<int16_t>(rawPacket[8]) << 8) | rawPacket[9];
		gyroReading.z = (static_cast<int16_t>(rawPacket[10]) << 8) | rawPacket[11];
		
		accelBiasSum += accelReading;
		gyroBiasSum += gyroReading;
	}

	accelBiasSum /= packetCount;
	gyroBiasSum /= packetCount;

	if(accelBiasSum.z > 0) 
		accelBiasSum.z -= 16384;
	else
		accelBiasSum.z += 16384;

	mpuWrite<uint8_t>(XG_OFFSET_H, (-gyroBiasSum.x / 4 >> 8) & 0xFF);
	mpuWrite<uint8_t>(XG_OFFSET_L, (-gyroBiasSum.x / 4) & 0xFF);
	mpuWrite<uint8_t>(YG_OFFSET_H, (-gyroBiasSum.y / 4 >> 8) & 0xFF);
	mpuWrite<uint8_t>(YG_OFFSET_L, (-gyroBiasSum.y / 4) & 0xFF);
	mpuWrite<uint8_t>(ZG_OFFSET_H, (-gyroBiasSum.z / 4 >> 8) & 0xFF);
	mpuWrite<uint8_t>(ZG_OFFSET_L, (-gyroBiasSum.z / 4) & 0xFF);

	glm::ivec3 rawAccelBias;
	rawAccelBias.x = (mpuRead<int8_t>(XA_OFFSET_H) << 8) | mpuRead<int8_t>(XA_OFFSET_L);
	rawAccelBias.y = (mpuRead<int8_t>(YA_OFFSET_H) << 8) | mpuRead<int8_t>(YA_OFFSET_L);
	rawAccelBias.z = (mpuRead<int8_t>(ZA_OFFSET_H) << 8) | mpuRead<int8_t>(ZA_OFFSET_L);

	uint8_t xMask = rawAccelBias.x & 1;
	uint8_t yMask = rawAccelBias.y & 1;
	uint8_t zMask = rawAccelBias.z & 1;
	
	rawAccelBias -= accelBiasSum / glm::i16vec3(8);

	mpuWrite<uint8_t>(XA_OFFSET_H, (rawAccelBias.x >> 8) & 0xFF);
	mpuWrite<uint8_t>(XA_OFFSET_L, (rawAccelBias.x & 0xFF) | xMask);
	mpuWrite<uint8_t>(YA_OFFSET_H, (rawAccelBias.y >> 8) & 0xFF);
	mpuWrite<uint8_t>(YA_OFFSET_L, (rawAccelBias.y & 0xFF) | yMask);
	mpuWrite<uint8_t>(ZA_OFFSET_H, (rawAccelBias.z >> 8) & 0xFF);
	mpuWrite<uint8_t>(ZA_OFFSET_L, (rawAccelBias.z & 0xFF) | zMask);
}


void MpuSensor::initializeCompass()
{
	compassWrite<uint8_t>(AK8963_CNTL, 0x00);
    vTaskDelay(10 / portTICK_RATE_MS);
    compassWrite<uint8_t>(AK8963_CNTL, 0x0F);
    vTaskDelay(10 / portTICK_RATE_MS);

	auto bias = compassRead<glm::u8vec3>(AK8963_ASAX);
	_compassBias = (glm::vec3(bias) - 128.0f) / 256.0f + 1.0f;

	compassWrite<uint8_t>(AK8963_CNTL, 0x00);
	vTaskDelay(10 / portTICK_RATE_MS);

	compassWrite<uint8_t>(AK8963_CNTL, (static_cast<uint8_t>(_compassScale) << 4) | 0x02);

	vTaskDelay(10 / portTICK_RATE_MS);
}


glm::vec3 MpuSensor::readAccelState() const
{
	uint8_t header = ACCEL_XOUT_H;
	std::array<uint8_t, 6> output;
	_mpuDevice.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(output.data(), output.size()));
	std::swap(output[0], output[1]);
	std::swap(output[2], output[3]);
	std::swap(output[4], output[5]);

	glm::vec3 state;
	state.x = reinterpret_cast<int16_t&>(output[0]);
	state.y = reinterpret_cast<int16_t&>(output[2]);
	state.z = reinterpret_cast<int16_t&>(output[4]);

	state *= 2.0f / 32768.0f;
	
	return state;
}


glm::vec3 MpuSensor::readGyroState() const
{
	uint8_t header = GYRO_XOUT_H;
	std::array<uint8_t, 6> output;
	_mpuDevice.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(output.data(), output.size()));
	std::swap(output[0], output[1]);
	std::swap(output[2], output[3]);
	std::swap(output[4], output[5]);
	
	glm::vec3 state;
	state.x = reinterpret_cast<int16_t&>(output[0]);
	state.y = reinterpret_cast<int16_t&>(output[2]);
	state.z = reinterpret_cast<int16_t&>(output[4]);

	state *= (250.0f / 32768.0f) * (glm::pi<float>() / 180.0f);
	return state;
}


glm::vec3 MpuSensor::readCompassState() const
{
	uint8_t header = AK8963_XOUT_L;
	std::array<uint8_t, 7> output;
	_compassDevice.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(output.data(), output.size()));

	glm::vec3 state;
	state.x = reinterpret_cast<int16_t&>(output[0]);
	state.y = reinterpret_cast<int16_t&>(output[2]);
	state.z = reinterpret_cast<int16_t&>(output[4]);

	state *= 10.0f * 4912.0f / 32760.0f;
	
	return state * _compassBias;
}
