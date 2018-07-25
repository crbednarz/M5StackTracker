#include "MpuSensor.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MPU9250.hpp"
#include "glm/gtc/constants.hpp"


MpuSensor::MpuSensor() :
	_device(MPU9250_ADDRESS),
	_fifoEntries(0),
	_accelBias(0.0f)
{
	calibrate();
	initialize();
}


void MpuSensor::poll()
{
	auto rawFifoCount = read<glm::u8vec2>(FIFO_COUNTH);
    auto fifoCount = (static_cast<uint16_t>(rawFifoCount.x) <<  8) | rawFifoCount.y;
    auto packetCount = fifoCount / 13;

	printf("%i", fifoCount);
	const uint8_t header = FIFO_R_W;
	_device.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(_fifoBuffer.data(), fifoCount));
	
	/*for (int i = 0; i < packetCount; i++)
    {
    	const uint8_t header = FIFO_R_W;
        _device.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(_fifoBuffer.data() + i * 19, 19));
	}*/
	_fifoEntries = packetCount;
}

bool MpuSensor::popFifoEntry(glm::vec3& accelState, glm::vec3& gyroState)
{
	if (_fifoEntries == 0)
		return false;

	gsl::span<uint8_t> rawPacket(_fifoBuffer.data() + (_fifoEntries - 1) * 13, 13);

	glm::i16vec3 accelReading;
	accelReading.x = (static_cast<int16_t>(rawPacket[0]) << 8) | rawPacket[1];
	accelReading.y = (static_cast<int16_t>(rawPacket[2]) << 8) | rawPacket[3];
	accelReading.z = (static_cast<int16_t>(rawPacket[4]) << 8) | rawPacket[5];

	glm::i16vec3 gyroReading;
	gyroReading.x = (static_cast<int16_t>(rawPacket[6]) << 8) | rawPacket[7];
	gyroReading.y = (static_cast<int16_t>(rawPacket[8]) << 8) | rawPacket[9];
	gyroReading.z = (static_cast<int16_t>(rawPacket[10]) << 8) | rawPacket[11];

	accelState = glm::vec3(accelReading) * (2.0f / 32768.0f) - _accelBias;
	gyroState = glm::vec3(gyroReading) * ((250.0f / 32768.0f) * (glm::pi<float>() / 180.0f));
	
	_fifoEntries--;

	return true;
}

void MpuSensor::initialize()
{
	// Reference: https://www.invensense.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf

	write<uint8_t>(PWR_MGMT_1, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);


	write<uint8_t>(PWR_MGMT_1, 0x01);
	vTaskDelay(200 / portTICK_RATE_MS);

	write<uint8_t>(CONFIG, 0x03);
	write<uint8_t>(SMPLRT_DIV, 0x04);
 
	uint8_t gyroConfig = read<uint8_t>(GYRO_CONFIG);
	gyroConfig &= ~(0x02 | 0x18);
	gyroConfig |= static_cast<uint8_t>(GyroScale::GFS_250DPS) << 3;
	write<uint8_t>(GYRO_CONFIG, gyroConfig);

	uint8_t accelConfig = read<uint8_t>(ACCEL_CONFIG);
	accelConfig &= ~0x18;
	accelConfig |= static_cast<uint8_t>(AccelScale::AFS_2G) << 3;
	write<uint8_t>(ACCEL_CONFIG, accelConfig);

	uint8_t accelConfig2 = read<uint8_t>(ACCEL_CONFIG2);
	accelConfig2 &= ~0x0F;
	accelConfig2 |= 0x03;
	write<uint8_t>(ACCEL_CONFIG2, accelConfig2);
	
	write<uint8_t>(INT_PIN_CFG, 0x22);
	write<uint8_t>(INT_ENABLE, 0x01);
	
	write<uint8_t>(USER_CTRL, 0x40 | 0x20);
	write<uint8_t>(I2C_MST_CTRL, 0x0D);
	write<uint8_t>(FIFO_EN, 0x79);

	write<uint8_t>(I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);
	write<uint8_t>(I2C_SLV0_CTRL, 0x81);
	
	vTaskDelay(100 / portTICK_RATE_MS);

	compassWrite<uint8_t>(AK8963_CNTL, 0);
	compassWrite<uint8_t>(AK8963_CNTL2, 1);

	vTaskDelay(100 / portTICK_RATE_MS);

	
	compassWrite<uint8_t>(AK8963_CNTL, 0x0F);

	vTaskDelay(100 / portTICK_RATE_MS);


	printf("Test: %i\n", compassRead<uint8_t>(0x00));
}


void MpuSensor::calibrate()
{
	write<uint8_t>(PWR_MGMT_1, 0x80);
	vTaskDelay(100 / portTICK_RATE_MS);

	write<uint8_t>(PWR_MGMT_1, 0x01);
	write<uint8_t>(PWR_MGMT_2, 0x00);
	vTaskDelay(200 / portTICK_RATE_MS);

	write<uint8_t>(INT_ENABLE, 0x00); 
	write<uint8_t>(FIFO_EN, 0x00);
	write<uint8_t>(PWR_MGMT_1, 0x00);
	write<uint8_t>(I2C_MST_CTRL, 0x00);
	write<uint8_t>(USER_CTRL, 0x00);
	write<uint8_t>(USER_CTRL, 0x0C);
	vTaskDelay(15 / portTICK_RATE_MS);

	write<uint8_t>(CONFIG, 0x01);
	write<uint8_t>(SMPLRT_DIV, 0x00);
	write<uint8_t>(GYRO_CONFIG, 0x00);
	write<uint8_t>(ACCEL_CONFIG, 0x00);

	write<uint8_t>(USER_CTRL, 0x40);
	write<uint8_t>(FIFO_EN, 0x78);
	vTaskDelay(40 / portTICK_RATE_MS);

	write<uint8_t>(FIFO_EN, 0x00);
	auto rawFifoCount = read<glm::u8vec2>(FIFO_COUNTH);
	auto fifoCount = (static_cast<uint16_t>(rawFifoCount.x) << 8) | rawFifoCount.y;
	auto packetCount = fifoCount / 12;

	glm::i16vec3 accelBiasSum(0);
	glm::i16vec3 gyroBiasSum(0);
	for (int i = 0; i < packetCount; i++)
	{
		std::array<uint8_t, 12> rawPacket;
		uint8_t header = FIFO_R_W;

		_device.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(rawPacket.data(), rawPacket.size()));
		
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

	write<uint8_t>(XG_OFFSET_H, (-gyroBiasSum.x / 4 >> 8) & 0xFF);
	write<uint8_t>(XG_OFFSET_L, (-gyroBiasSum.x / 4) & 0xFF);
	write<uint8_t>(YG_OFFSET_H, (-gyroBiasSum.y / 4 >> 8) & 0xFF);
	write<uint8_t>(YG_OFFSET_L, (-gyroBiasSum.y / 4) & 0xFF);
	write<uint8_t>(ZG_OFFSET_H, (-gyroBiasSum.z / 4 >> 8) & 0xFF);
	write<uint8_t>(ZG_OFFSET_L, (-gyroBiasSum.z / 4) & 0xFF);

	glm::ivec3 rawAccelBias;
	rawAccelBias.x = (read<int8_t>(XA_OFFSET_H) << 8) | read<int8_t>(XA_OFFSET_L);
	rawAccelBias.y = (read<int8_t>(YA_OFFSET_H) << 8) | read<int8_t>(YA_OFFSET_L);
	rawAccelBias.z = (read<int8_t>(ZA_OFFSET_H) << 8) | read<int8_t>(ZA_OFFSET_L);

	uint8_t xMask = rawAccelBias.x & 1;
	uint8_t yMask = rawAccelBias.y & 1;
	uint8_t zMask = rawAccelBias.z & 1;
	
	rawAccelBias -= accelBiasSum / glm::i16vec3(8);

	write<uint8_t>(XA_OFFSET_H, (rawAccelBias.x >> 8) & 0xFF);
	write<uint8_t>(XA_OFFSET_L, (rawAccelBias.x & 0xFF) | xMask);
	write<uint8_t>(YA_OFFSET_H, (rawAccelBias.y >> 8) & 0xFF);
	write<uint8_t>(YA_OFFSET_L, (rawAccelBias.y & 0xFF) | yMask);
	write<uint8_t>(ZA_OFFSET_H, (rawAccelBias.z >> 8) & 0xFF);
	write<uint8_t>(ZA_OFFSET_L, (rawAccelBias.z & 0xFF) | zMask);

	_accelBias = glm::vec3(accelBiasSum) / 16384.0f;
}



template <typename T>
void MpuSensor::write(uint8_t address, T data) const
{
	_device.write(gsl::span<const uint8_t>(&address, 1), gsl::span<const uint8_t>(reinterpret_cast<uint8_t*>(&data), sizeof(T)));
}

template <typename T>
void MpuSensor::compassWrite(uint8_t address, T data) const
{
	write<uint8_t>(I2C_SLV0_ADDR, AK8963_ADDRESS);
	write<uint8_t>(I2C_SLV0_REG, address);
	write<uint8_t>(I2C_SLV0_CTRL, 0x80 | sizeof(T));
	write<T>(I2C_SLV0_DO, data);
	
	vTaskDelay(10 / portTICK_RATE_MS);
}



template <typename T>
T MpuSensor::read(uint8_t address) const
{
	T output;
	_device.read(gsl::span<const uint8_t>(&address, 1), gsl::span<uint8_t>(reinterpret_cast<uint8_t*>(&output), sizeof(T)));

	return output;
}


template <typename T>
T MpuSensor::compassRead(uint8_t address) const
{
	write<uint8_t>(I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);
	write<uint8_t>(I2C_SLV0_REG, address);
	write<uint8_t>(I2C_SLV0_CTRL, 0x80 | sizeof(T));
	
	vTaskDelay(10 / portTICK_RATE_MS);
	
	return read<T>(EXT_SENS_DATA_00);
}
