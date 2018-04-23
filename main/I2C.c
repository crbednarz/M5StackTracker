#include "I2C.h"


void I2C_Initialize()
{
	i2c_config_t config;
	config.mode = I2C_MODE_MASTER;
	config.sda_io_num = I2C_SDA;
	config.sda_pullup_en = GPIO_PULLUP_ENABLE;
	config.scl_io_num = I2C_SCL;
	config.scl_pullup_en = GPIO_PULLUP_ENABLE;
	config.master.clk_speed = 100000; // 100000 is the "Standard" frequency
	
	i2c_param_config(I2C_PORT, &config);
	i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0);		
}


void I2C_WriteBytes(uint8_t address, const uint8_t* data, size_t length)
{
	// 'const' has been casted away on data for i2c_master_write which I can't imagine ever modifying the data.
	// It may be worth investigating the source further.
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write(cmd, (uint8_t*)data, length, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);	
}


void I2C_ReadBytes(uint8_t address, uint8_t* data, size_t length)
{
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


void I2C_WriteWithHeader(uint8_t address, const uint8_t* header, size_t headerLength, const uint8_t* data, size_t dataLength)
{	
	// 'const' has been casted away on data for i2c_master_write which I can't imagine ever modifying the data.
	// It may be worth investigating the source further.
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write(cmd, (uint8_t*)header, headerLength, true);
	i2c_master_write(cmd, (uint8_t*)data, dataLength, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);	
}


void I2C_ReadWithHeader(uint8_t address, const uint8_t* header, size_t headerLength, uint8_t* data, size_t dataLength)
{
	I2C_WriteBytes(address, header, headerLength);
	I2C_ReadBytes(address, data, dataLength);
}


