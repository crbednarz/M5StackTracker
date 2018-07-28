#include "MpuSensor.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MPU9250.hpp"
#include "glm/gtc/constants.hpp"


MpuSensor::MpuSensor() :
	_device(MPU9250_ADDRESS),
	_fifoEntries(0),
	_accelBias(0.0f),
	_gyroBias(0.0f),
	_compassBias(0.0f)
{
	calibrate();
	initialize();
}

const size_t FIFO_PACKET_SIZE = 19;


void MpuSensor::poll()
{
	auto rawFifoCount = read<glm::u8vec2>(FIFO_COUNTH);
	auto fifoCount = (static_cast<uint16_t>(rawFifoCount.x) <<  8) | rawFifoCount.y;
	auto packetCount = fifoCount / FIFO_PACKET_SIZE;

	const uint8_t header = FIFO_R_W;
	_device.read(gsl::span<const uint8_t>(&header, 1), gsl::span<uint8_t>(_fifoBuffer.data(), fifoCount));
	
	_fifoEntries = packetCount;
}

bool MpuSensor::popFifoEntry(glm::vec3& accelState, glm::vec3& gyroState, glm::vec3& compassState)
{
	if (_fifoEntries == 0)
		return false;

	gsl::span<uint8_t> rawPacket(_fifoBuffer.data() + (_fifoEntries - 1) * FIFO_PACKET_SIZE, FIFO_PACKET_SIZE);

	glm::i16vec3 accelReading;
	accelReading.x = (static_cast<int16_t>(rawPacket[0]) << 8) | rawPacket[1];
	accelReading.y = (static_cast<int16_t>(rawPacket[2]) << 8) | rawPacket[3];
	accelReading.z = (static_cast<int16_t>(rawPacket[4]) << 8) | rawPacket[5];

	glm::i16vec3 gyroReading;
	gyroReading.x = (static_cast<int16_t>(rawPacket[6]) << 8) | rawPacket[7];
	gyroReading.y = (static_cast<int16_t>(rawPacket[8]) << 8) | rawPacket[9];
	gyroReading.z = (static_cast<int16_t>(rawPacket[10]) << 8) | rawPacket[11];

	glm::i16vec3 compassReading;
	compassReading.x = (static_cast<int16_t>(rawPacket[13]) << 8) | rawPacket[12];
	compassReading.y = (static_cast<int16_t>(rawPacket[15]) << 8) | rawPacket[14];
	compassReading.z = (static_cast<int16_t>(rawPacket[17]) << 8) | rawPacket[16];

	accelState = glm::vec3(accelReading) * (2.0f / 32768.0f);
	gyroState = glm::vec3(gyroReading) * ((250.0f / 32768.0f) * (glm::pi<float>() / 180.0f));
	compassState = glm::vec3(compassReading) * (10.0f * 4912.0f / 32760.0f);

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
	
	write<uint8_t>(INT_ENABLE, 0x00);
	
	write<uint8_t>(USER_CTRL, 0x40 | 0x20);
	write<uint8_t>(I2C_MST_CTRL, 0x0D);
	write<uint8_t>(FIFO_EN, 0x40 | 0x20 | 0x10 | 0x08 | 0x01);

	write<uint8_t>(I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);
	write<uint8_t>(I2C_SLV0_CTRL, 0x81);

	
	vTaskDelay(100 / portTICK_RATE_MS);

	// Shutdown compass
	compassWrite<uint8_t>(AK8963_CNTL1, 0x0);

	vTaskDelay(100 / portTICK_RATE_MS);

	// Reset compass
	compassWrite<uint8_t>(AK8963_CNTL2, 0x1);

	vTaskDelay(100 / portTICK_RATE_MS);


	compassWrite<uint8_t>(AK8963_CNTL1, 0x0F);
	
	vTaskDelay(100 / portTICK_RATE_MS);

	auto bias = compassRead<glm::u8vec3>(AK8963_ASAX);
	_compassBias = (glm::vec3(bias) - 128.0f) / 256.0f + 1.0f;
	compassWrite<uint8_t>(AK8963_CNTL1, 0x00);


	vTaskDelay(100 / portTICK_RATE_MS);

	compassWrite<uint8_t>(AK8963_CNTL1, (static_cast<uint8_t>(CompassScale::MFS_16BITS) << 4) | 0b0110);


	write<uint8_t>(I2C_SLV0_ADDR, AK8963_ADDRESS | 0x80);
	write<uint8_t>(I2C_SLV0_REG, AK8963_XOUT_L);
	write<uint8_t>(I2C_SLV0_CTRL, 0x80 | 7);
	
	vTaskDelay(500 / portTICK_RATE_MS);

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
