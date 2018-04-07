#include "Gyro.h"
#include "driver/i2c.h"
#include "MPU9250.h"
#include <stdint.h>


#define PIN_SDA 21
#define PIN_SCL 22
#define I2C_PORT I2C_NUM_1
#define MAGNETOMETER_SCALE MFS_16BITS
#define ACCELEROMETER_SCALE AFS_2G
#define GYRO_SCALE GFS_250DPS


static void Gyro_WriteReadRequest(uint8_t address, uint8_t subAddress)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, subAddress, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);	
}

static void Gyro_WriteByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, subAddress, true);
	i2c_master_write_byte(cmd, data, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);	
}


static uint8_t Gyro_ReadByte(uint8_t address, uint8_t subAddress)
{
	Gyro_WriteReadRequest(address, subAddress);
	uint8_t result;
	
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(cmd, &result, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return result;
}

static void Gyro_ReadBytes(uint8_t address, uint8_t subAddress, uint8_t* data, size_t length)
{	
	Gyro_WriteReadRequest(address, subAddress);
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);
	if (length > 1)
		i2c_master_read(cmd, data, length - 1, false);
	i2c_master_read(cmd, data + length - 1, 1, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

static void Gyro_InitializeI2C()
{
	i2c_config_t config;
	config.mode = I2C_MODE_MASTER;
	config.sda_io_num = PIN_SDA;
	config.sda_pullup_en = GPIO_PULLUP_ENABLE;
	config.scl_io_num = PIN_SCL;
	config.scl_pullup_en = GPIO_PULLUP_ENABLE;
	config.master.clk_speed = 100000;  // 100000 is the "Standard" frequency
	
	i2c_param_config(I2C_PORT, &config);
	i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0);		
}


static void Gyro_InitializeDevice()
{
	
}


void Gyro_Initialize()
{
	Gyro_InitializeI2C();
	Gyro_InitializeDevice();
}

void Gyro_UpdateState()
{
	
}