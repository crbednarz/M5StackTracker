#include "Display.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <string.h>
#include <driver/ledc.h>
#include "ILI9341.hpp"
#include "PeripheralConfig.hpp"
#include "soc/spi_reg.h"


lldesc_t* Display::DmaDescription = nullptr;


void Display::initialize()
{
	setupPins();
	setupSpi();
	sendInitialize();
}


void Display::writeCommand(uint8_t command)
{
	gsl::span<uint8_t> message(&command, 1);
	write(message, MessageMode::Command);
}


void Display::writeData(uint8_t data)
{
	gsl::span<uint8_t> message(&data, 1);
	write(message, MessageMode::Data);
}


void Display::writeData(gsl::span<const uint8_t> data)
{
	write(data, MessageMode::Data);
}


void Display::setupSpi()
{
	DmaDescription = reinterpret_cast<lldesc_t*>(heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA));
	spi_bus_config_t spiBugConfig = {};
	spiBugConfig.mosi_io_num = DISPLAY_MOSI;
	spiBugConfig.miso_io_num = -1;
	spiBugConfig.sclk_io_num = DISPLAY_CLK;
	spiBugConfig.quadwp_io_num = -1;
	spiBugConfig.quadhd_io_num = -1;
	
	spi_device_interface_config_t deviceInterfaceConfig = {};
	deviceInterfaceConfig.clock_speed_hz = 60000000;
	deviceInterfaceConfig.mode = 0;
	deviceInterfaceConfig.spics_io_num = DISPLAY_CS;
	deviceInterfaceConfig.queue_size = 7;
	deviceInterfaceConfig.duty_cycle_pos = 1;
	deviceInterfaceConfig.flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_HALFDUPLEX;	
	spi_bus_initialize(VSPI_HOST, &spiBugConfig, DISPLAY_DMA_CHANNEL);
	
	spi_device_handle_t deviceHandle = {};
	spi_bus_add_device(VSPI_HOST, &deviceInterfaceConfig, &deviceHandle);
	
	// Sending a blank dummy command allows us to piggy-back off of esp-idf's SPI initialization code.
	uint8_t command = 0x55;
	spi_transaction_t transaction = {};
	memset(&transaction, 0, sizeof(transaction));
	transaction.length = 8;
	transaction.tx_buffer = &command;
	spi_device_transmit(deviceHandle, &transaction);
}


void Display::setupPins()
{
	//Initialize non-SPI GPIOs
	gpio_set_direction(DISPLAY_DC, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_RST, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_BCKL, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_CS, GPIO_MODE_OUTPUT);

	gpio_set_level(DISPLAY_CS, 1);
	
	//Reset the display
	gpio_set_level(DISPLAY_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(DISPLAY_RST, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(DISPLAY_BCKL, 0);
	
	
	gpio_set_level(DISPLAY_CS, 0);
	
	ledc_timer_config_t ledcTimer = {};
	ledcTimer.freq_hz = 5000;
	ledcTimer.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledcTimer.timer_num = LEDC_TIMER_0;
	ledcTimer.duty_resolution = LEDC_TIMER_13_BIT;
	
	ledc_timer_config(&ledcTimer);
	
	ledc_channel_config_t ledcChannelConfig = {};
	ledcChannelConfig.channel = LEDC_CHANNEL_0;
	ledcChannelConfig.duty = 0;
	ledcChannelConfig.gpio_num = DISPLAY_BCKL;
	ledcChannelConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledcChannelConfig.timer_sel = LEDC_TIMER_0;
	
	ledc_channel_config(&ledcChannelConfig);
	
	ledc_fade_func_install(0);
	
	ledc_set_duty(ledcChannelConfig.speed_mode, ledcChannelConfig.channel, 0xFFFF);
	ledc_update_duty(ledcChannelConfig.speed_mode, ledcChannelConfig.channel);
}


void Display::sendInitialize()
{
	writeCommand(0x01);  // Software reset
 
	vTaskDelay(5 / portTICK_RATE_MS);
	
	writeCommand(0xEF);
	writeData(0x03);
	writeData(0x80);
	writeData(0x02);
 
	writeCommand(0xCF);
	writeData(0x00);
	writeData(0XC1);
	writeData(0X30);
 
	writeCommand(0xED);
	writeData(0x64);
	writeData(0x03);
	writeData(0X12);
	writeData(0X81);
 
	writeCommand(0xE8);
	writeData(0x85);
	writeData(0x00);
	writeData(0x78);
 
	writeCommand(0xCB);
	writeData(0x39);
	writeData(0x2C);
	writeData(0x00);
	writeData(0x34);
	writeData(0x02);
 
	writeCommand(0xF7);
	writeData(0x20);
 
	writeCommand(0xEA);
	writeData(0x00);
	writeData(0x00);
 
	writeCommand(ILI9341_PWCTR1);  //Power control
	writeData(0x23);               //VRH[5:0]
 
	writeCommand(ILI9341_PWCTR2);  //Power control
	writeData(0x10);               //SAP[2:0];BT[3:0]
 
	writeCommand(ILI9341_VMCTR1);  //VCM control
	writeData(0x3e);
	writeData(0x28);
 
	writeCommand(ILI9341_VMCTR2);  //VCM control2
	writeData(0x86);               //--
 
	writeCommand(ILI9341_MADCTL);  // Memory Access Control
	writeData(0x8);
 
	writeCommand(ILI9341_PIXFMT);
	writeData(0x55);
 
	writeCommand(ILI9341_FRMCTR1);
	writeData(0x00);
	writeData(0x13);  // 0x18 79Hz, 0x1B default 70Hz, 0x13 100Hz
 
	writeCommand(ILI9341_DFUNCTR);  // Display Function Control
	writeData(0x08);
	writeData(0x82);
	writeData(0x27);
 
	writeCommand(0xF2);  // 3Gamma Function Disable
	writeData(0x00);
 
	writeCommand(ILI9341_GAMMASET);  //Gamma curve selected
	writeData(0x01);
 
	writeCommand(ILI9341_GMCTRP1);  //Set Gamma
	writeData(0x0F);
	writeData(0x31);
	writeData(0x2B);
	writeData(0x0C);
	writeData(0x0E);
	writeData(0x08);
	writeData(0x4E);
	writeData(0xF1);
	writeData(0x37);
	writeData(0x07);
	writeData(0x10);
	writeData(0x03);
	writeData(0x0E);
	writeData(0x09);
	writeData(0x00);
 
	writeCommand(ILI9341_GMCTRN1);  //Set Gamma
	writeData(0x00);
	writeData(0x0E);
	writeData(0x14);
	writeData(0x03);
	writeData(0x11);
	writeData(0x07);
	writeData(0x31);
	writeData(0xC1);
	writeData(0x48);
	writeData(0x08);
	writeData(0x0F);
	writeData(0x0C);
	writeData(0x31);
	writeData(0x36);
	writeData(0x0F);
 
	writeCommand(ILI9341_SLPOUT);  //Exit Sleep
 
	vTaskDelay(120 / portTICK_RATE_MS);
 
	writeCommand(ILI9341_DISPON);  //Display on
}


void Display::write(gsl::span<const uint8_t> message, MessageMode mode)
{
	// Unfortunately esp-idf uses tasks and queues for individual transactions.
	// This creates large waiting periods when trying to send individual messages, as is required when working with 
	// sending interlaced pixel rows. So we use the actual registers here.
	// It's not pretty, but it works.
	
	gpio_set_level(DISPLAY_DC, static_cast<uint32_t>(mode));	

	spi_dev_t* spi = (spi_dev_t*)DR_REG_SPI3_BASE;
	spicommon_dmaworkaround_idle(DISPLAY_DMA_CHANNEL);

	spi->slave.trans_done = 0; 
	spi->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
	spi->dma_out_link.start = 0;
	spi->dma_in_link.start = 0;
	spi->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
	spi->dma_conf.out_data_burst_en = 1;
	spi->ctrl.val &= ~(SPI_FREAD_DUAL|SPI_FREAD_QUAD|SPI_FREAD_DIO|SPI_FREAD_QIO);
	spi->user.val &= ~(SPI_FWRITE_DUAL|SPI_FWRITE_QUAD|SPI_FWRITE_DIO|SPI_FWRITE_QIO);

	spi->dma_in_link.addr = 0;
	spi->dma_in_link.start = 1;


	spicommon_dmaworkaround_transfer_active(DISPLAY_DMA_CHANNEL);
	spicommon_setup_dma_desc_links(DmaDescription, message.size_bytes(), message.data(), false);
	spi->user.usr_mosi_highpart = 0;
	spi->dma_out_link.addr = (int)DmaDescription & 0xFFFFF;
	spi->dma_out_link.start = 1;
	spi->user.usr_mosi_highpart = 0;
	spi->user.usr_dummy = 0;

	spi->mosi_dlen.usr_mosi_dbitlen = message.size_bytes() * 8;

	spi->miso_dlen.usr_miso_dbitlen= 0;

	spi->user.usr_mosi = 1;
	spi->user.usr_miso = 0;

	spi->cmd.usr=1;
    while(spi->cmd.usr);
}