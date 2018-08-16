#include "I2CDevice.hpp"
#include "driver/i2c.h"
#include "PeripheralConfig.hpp"


void I2CDevice::initialize()
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
	i2c_set_timeout(I2C_PORT, 1000000);
}


I2CDevice::I2CDevice(uint8_t slaveAddress) : 
	_slaveAddress(slaveAddress)
{

}


void I2CDevice::write(gsl::span<const uint8_t> data) const
{
	// 'const' has been casted away on data for i2c_master_write which I can't imagine ever modifying the data.
	// It may be worth investigating the source further.
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_slaveAddress << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write(cmd, const_cast<uint8_t*>(data.data()), data.length(), true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100000));
	i2c_cmd_link_delete(cmd);
}

void I2CDevice::read(gsl::span<uint8_t> output) const
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_slaveAddress << 1) | I2C_MASTER_READ, true);
	if (output.length() > 1)
		i2c_master_read(cmd, output.data(), output.length() - 1, I2C_MASTER_ACK);
	i2c_master_read_byte(cmd, output.data() + output.length() - 1, I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100000));
	i2c_cmd_link_delete(cmd);
}


void I2CDevice::write(gsl::span<const uint8_t> header, gsl::span<const uint8_t> data) const
{
	// 'const' has been casted away on data for i2c_master_write which I can't imagine ever modifying the data.
	// It may be worth investigating the source further.
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_slaveAddress << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write(cmd, const_cast<uint8_t*>(header.data()), header.length(), true);
	i2c_master_write(cmd, const_cast<uint8_t*>(data.data()), data.length(), true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100000));
	i2c_cmd_link_delete(cmd);
}


void I2CDevice::read(gsl::span<const uint8_t> header, gsl::span<uint8_t> output) const
{
	write(header);
	read(output);
}
