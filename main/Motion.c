#include "Motion.h"
#include "I2C.h"
#include "MPU9250.h"
#include "Vector.h"
#include "MadgwickAHRS.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <math.h>


#define GYRO_SCALE GFS_250DPS
#define ACCEL_SCALE AFS_2G
#define COMPASS_SCALE MFS_16BITS



static struct
{
    Vector3 CompassBias;
	Vector3 AccelBias;
	Vector3 GyroBias;
	TickType_t LastUpdate;
} MotionContext;


static void MPU_WriteByte(uint8_t deviceRegister, uint8_t data)
{
	I2C_WriteWithHeader(MPU9250_ADDRESS, &deviceRegister, 1, &data, 1);
}


static void MPU_WriteBytes(uint8_t deviceRegister, const uint8_t* data, size_t length)
{
	I2C_WriteWithHeader(MPU9250_ADDRESS, &deviceRegister, 1, data, length);
}


static uint8_t MPU_ReadByte(uint8_t deviceRegister)
{
	uint8_t data;
	I2C_ReadWithHeader(MPU9250_ADDRESS, &deviceRegister, 1, &data, 1);
	return data;
}


static void MPU_ReadBytes(uint8_t deviceRegister, uint8_t* data, size_t length)
{
	I2C_ReadWithHeader(MPU9250_ADDRESS, &deviceRegister, 1, data, length);
}


static void Compass_WriteByte(uint8_t deviceRegister, uint8_t data)
{
	I2C_WriteWithHeader(AK8963_ADDRESS, &deviceRegister, 1, &data, 1);
}


static void Compass_WriteBytes(uint8_t deviceRegister, const uint8_t* data, size_t length)
{
	I2C_WriteWithHeader(AK8963_ADDRESS, &deviceRegister, 1, data, length);
}


static uint8_t Compass_ReadByte(uint8_t deviceRegister)
{
	uint8_t data;
	I2C_ReadWithHeader(AK8963_ADDRESS, &deviceRegister, 1, &data, 1);
	return data;
}


static void Compass_ReadBytes(uint8_t deviceRegister, uint8_t* data, size_t length)
{
	I2C_ReadWithHeader(AK8963_ADDRESS, &deviceRegister, 1, data, length);
}


static void Motion_CalibrateMPU9250()
{
	
}


static void Motion_InitializeMPU9250()
{
	// Reference: https://www.invensense.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf

	MPU_WriteByte(PWR_MGMT_1, 0x00);
	vTaskDelay(100 / portTICK_RATE_MS);

	MPU_WriteByte(PWR_MGMT_1, 0x01);
	vTaskDelay(200 / portTICK_RATE_MS);

	MPU_WriteByte(CONFIG, 0x03);
	MPU_WriteByte(SMPLRT_DIV, 0x04);

	uint8_t gyroConfig = MPU_ReadByte(GYRO_CONFIG);
	gyroConfig &= ~(0x02 | 0x18);
	gyroConfig |= GYRO_SCALE << 3;
	MPU_WriteByte(GYRO_CONFIG, gyroConfig);

	uint8_t accelConfig = MPU_ReadByte(ACCEL_CONFIG);
	accelConfig &= ~0x18;
	accelConfig |= ACCEL_SCALE << 3;
	MPU_WriteByte(ACCEL_CONFIG, accelConfig);

	uint8_t accelConfig2 = MPU_ReadByte(ACCEL_CONFIG2);
	accelConfig2 &= ~0x0F;
	accelConfig2 |= 0x03;
	MPU_WriteByte(ACCEL_CONFIG2, accelConfig2);
	
	MPU_WriteByte(INT_PIN_CFG, 0x22);
	MPU_WriteByte(INT_ENABLE, 0x01);
	
	vTaskDelay(100 / portTICK_RATE_MS);
}


static void Motion_InitializeAK8963()
{
    Compass_WriteByte(AK8963_CNTL, 0x00);
    vTaskDelay(10 / portTICK_RATE_MS);
    Compass_WriteByte(AK8963_CNTL, 0x0F);
    vTaskDelay(10 / portTICK_RATE_MS);

	Compass_WriteByte(AK8963_CNTL, 0x00);
	vTaskDelay(10 / portTICK_RATE_MS);
	Compass_WriteByte(AK8963_CNTL, 0x0F);
	vTaskDelay(10 / portTICK_RATE_MS);

    uint8_t gyroCalibration[3];
	Compass_ReadBytes(AK8963_ASAX, gyroCalibration, 3);
	MotionContext.CompassBias.X = (gyroCalibration[0] - 128) / 256.0f + 1.0f;
	MotionContext.CompassBias.Y = (gyroCalibration[1] - 128) / 256.0f + 1.0f;  
	MotionContext.CompassBias.Z = (gyroCalibration[2] - 128) / 256.0f + 1.0f; 
	Compass_WriteByte(AK8963_CNTL, 0x00);
	vTaskDelay(10 / portTICK_RATE_MS);

	Compass_WriteByte(AK8963_CNTL, (COMPASS_SCALE << 4) | 0x02);
	vTaskDelay(10 / portTICK_RATE_MS);

}


void Motion_Initialize()
{
	memset(&MotionContext, 0, sizeof(MotionContext));

	Motion_InitializeMPU9250();
	Motion_InitializeAK8963();
	MotionContext.LastUpdate = xTaskGetTickCount();

}


static Vector3 Motion_ReadAccelState()
{
	uint8_t rawState[6];
	MPU_ReadBytes(ACCEL_XOUT_H, rawState, sizeof(rawState));

	Vector3 state =
	{
		.X = (int16_t)((rawState[0] << 8) | rawState[1]),
		.Y = (int16_t)((rawState[2] << 8) | rawState[3]),
		.Z = (int16_t)((rawState[4] << 8) | rawState[5])
	};

	state.X *= 2.0f / 32768.0f;
	state.Y *= 2.0f / 32768.0f;
	state.Z *= 2.0f / 32768.0f;

	return state;
}


static Vector3 Motion_ReadGyroState()
{
	uint8_t rawState[6];
	MPU_ReadBytes(GYRO_XOUT_H, rawState, sizeof(rawState));

	Vector3 state =
	{
		.X = (int16_t)((rawState[0] << 8) | rawState[1]),
		.Y = (int16_t)((rawState[2] << 8) | rawState[3]),
		.Z = (int16_t)((rawState[4] << 8) | rawState[5])
	};

	state.X *= 2.0f / 32768.0f;
	state.Y *= 2.0f / 32768.0f;
	state.Z *= 2.0f / 32768.0f;
	
	return state;
}

static Vector3 Motion_ReadCompassState()
{
	uint8_t rawState[6];
	Compass_ReadBytes(AK8963_XOUT_H, rawState, sizeof(rawState));

	Vector3 state =
	{
		.X = (int16_t)((rawState[0] << 8) | rawState[1]),
		.Y = (int16_t)((rawState[2] << 8) | rawState[3]),
		.Z = (int16_t)((rawState[4] << 8) | rawState[5])
	};

	state.X *= 10.0f * 4912.0f / 32760.0f;
	state.Y *= 10.0f * 4912.0f / 32760.0f;
	state.Z *= 10.0f * 4912.0f / 32760.0f;
	
	return state;
}


void Motion_UpdateState()
{
	Vector3 accelState = Motion_ReadAccelState();
	Vector3 gyroState = Motion_ReadGyroState();
	Vector3 compassState = Motion_ReadCompassState();

	TickType_t time = xTaskGetTickCount();
	float elapsedSeconds = (float)(time - MotionContext.LastUpdate) / configTICK_RATE_HZ;

	MadgwickAHRSupdate(
		gyroState.X, gyroState.Y, gyroState.Z,
		accelState.X, accelState.Y, accelState.Z,
		compassState.X, compassState.Y, compassState.Z, 
		elapsedSeconds);

	MotionContext.LastUpdate = time;

	float roll = atan2f(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2);
	float pitch = asinf(-2.0f * (q1 * q3 - q0 * q2));
	float yaw = atan2f(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3);

	printf("%d %d %d\n", (int)(roll * 360.0f / M_2_PI), (int)(pitch * 360.0f / M_2_PI), (int)(yaw * 360.0f / M_2_PI));
}