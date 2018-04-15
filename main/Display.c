#include "Display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <string.h>
#include <driver/ledc.h>
#include "ILI9341.h"
#include "PeripheralConfig.h"
#include "soc/spi_reg.h"

typedef enum
{
	MESSAGE_COMMAND = 0,
	MESSAGE_DATA = 1
} MessageMode;

lldesc_t* DmaDescription;
spi_device_handle_t Display_SpiDeviceHandle;
#define DMA_CHANNEL 1


static void Display_SetupPins()
{
	//Initialize non-SPI GPIOs
	gpio_set_direction(DISPLAY_PIN_DC, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_PIN_RST, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_PIN_BCKL, GPIO_MODE_OUTPUT);
	gpio_set_direction(DISPLAY_PIN_CS, GPIO_MODE_OUTPUT);

	gpio_set_level(DISPLAY_PIN_CS, 1);
	
	//Reset the display
	gpio_set_level(DISPLAY_PIN_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(DISPLAY_PIN_RST, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(DISPLAY_PIN_BCKL, 0);
	
	
	gpio_set_level(DISPLAY_PIN_CS, 0);
	
	ledc_timer_config_t ledcTimer = {
		.duty_resolution = LEDC_TIMER_13_BIT,
		.freq_hz = 5000,
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.timer_num = LEDC_TIMER_0
	};
	
	ledc_timer_config(&ledcTimer);
	
	ledc_channel_config_t ledcChannelConfig = {
		.channel    = LEDC_CHANNEL_0,
		.duty       = 0,
		.gpio_num   = DISPLAY_PIN_BCKL,
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.timer_sel  = LEDC_TIMER_0
	};
	
	ledc_channel_config(&ledcChannelConfig);
	
	ledc_fade_func_install(0);
	
	ledc_set_duty(ledcChannelConfig.speed_mode, ledcChannelConfig.channel, 0xFFFF);
	ledc_update_duty(ledcChannelConfig.speed_mode, ledcChannelConfig.channel);
}

		
static void Display_SetupSPI()
{
	DmaDescription = heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
	spi_bus_config_t spiBugConfig =
	{
		.mosi_io_num = DISPLAY_PIN_MOSI,
		.miso_io_num = -1,
		.sclk_io_num = DISPLAY_PIN_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};
	
	spi_device_interface_config_t deviceInterfaceConfig =
	{
		.clock_speed_hz = 60000000,
		.mode = 0,
		.spics_io_num = DISPLAY_PIN_CS, 
		.queue_size = 7
	};
	
	spi_bus_initialize(VSPI_HOST, &spiBugConfig, DMA_CHANNEL);
	
	spi_bus_add_device(VSPI_HOST, &deviceInterfaceConfig, &Display_SpiDeviceHandle);
	
	// Sending a blank dummy command allows us to piggy-back off of esp-idf's SPI initialization code.
	uint8_t command = 0x55;
	spi_transaction_t transaction;
	memset(&transaction, 0, sizeof(transaction));
	transaction.length = 8;
	transaction.tx_buffer = &command;
	spi_device_transmit(Display_SpiDeviceHandle, &transaction);
}


static void Display_SendInitializationCommands()
{
	Display_WriteCommand(0x01);  // Software reset
 
	vTaskDelay(5 / portTICK_RATE_MS);
	
	Display_WriteCommand(0xEF);
	Display_WriteData(0x03);
	Display_WriteData(0x80);
	Display_WriteData(0x02);
 
	Display_WriteCommand(0xCF);
	Display_WriteData(0x00);
	Display_WriteData(0XC1);
	Display_WriteData(0X30);
 
	Display_WriteCommand(0xED);
	Display_WriteData(0x64);
	Display_WriteData(0x03);
	Display_WriteData(0X12);
	Display_WriteData(0X81);
 
	Display_WriteCommand(0xE8);
	Display_WriteData(0x85);
	Display_WriteData(0x00);
	Display_WriteData(0x78);
 
	Display_WriteCommand(0xCB);
	Display_WriteData(0x39);
	Display_WriteData(0x2C);
	Display_WriteData(0x00);
	Display_WriteData(0x34);
	Display_WriteData(0x02);
 
	Display_WriteCommand(0xF7);
	Display_WriteData(0x20);
 
	Display_WriteCommand(0xEA);
	Display_WriteData(0x00);
	Display_WriteData(0x00);
 
	Display_WriteCommand(ILI9341_PWCTR1);  //Power control
	Display_WriteData(0x23);               //VRH[5:0]
 
	Display_WriteCommand(ILI9341_PWCTR2);  //Power control
	Display_WriteData(0x10);               //SAP[2:0];BT[3:0]
 
	Display_WriteCommand(ILI9341_VMCTR1);  //VCM control
	Display_WriteData(0x3e);
	Display_WriteData(0x28);
 
	Display_WriteCommand(ILI9341_VMCTR2);  //VCM control2
	Display_WriteData(0x86);               //--
 
	Display_WriteCommand(ILI9341_MADCTL);  // Memory Access Control
	Display_WriteData(0x8);
 
	Display_WriteCommand(ILI9341_PIXFMT);
	Display_WriteData(0x55);
 
	Display_WriteCommand(ILI9341_FRMCTR1);
	Display_WriteData(0x00);
	Display_WriteData(0x13);  // 0x18 79Hz, 0x1B default 70Hz, 0x13 100Hz
 
	Display_WriteCommand(ILI9341_DFUNCTR);  // Display Function Control
	Display_WriteData(0x08);
	Display_WriteData(0x82);
	Display_WriteData(0x27);
 
	Display_WriteCommand(0xF2);  // 3Gamma Function Disable
	Display_WriteData(0x00);
 
	Display_WriteCommand(ILI9341_GAMMASET);  //Gamma curve selected
	Display_WriteData(0x01);
 
	Display_WriteCommand(ILI9341_GMCTRP1);  //Set Gamma
	Display_WriteData(0x0F);
	Display_WriteData(0x31);
	Display_WriteData(0x2B);
	Display_WriteData(0x0C);
	Display_WriteData(0x0E);
	Display_WriteData(0x08);
	Display_WriteData(0x4E);
	Display_WriteData(0xF1);
	Display_WriteData(0x37);
	Display_WriteData(0x07);
	Display_WriteData(0x10);
	Display_WriteData(0x03);
	Display_WriteData(0x0E);
	Display_WriteData(0x09);
	Display_WriteData(0x00);
 
	Display_WriteCommand(ILI9341_GMCTRN1);  //Set Gamma
	Display_WriteData(0x00);
	Display_WriteData(0x0E);
	Display_WriteData(0x14);
	Display_WriteData(0x03);
	Display_WriteData(0x11);
	Display_WriteData(0x07);
	Display_WriteData(0x31);
	Display_WriteData(0xC1);
	Display_WriteData(0x48);
	Display_WriteData(0x08);
	Display_WriteData(0x0F);
	Display_WriteData(0x0C);
	Display_WriteData(0x31);
	Display_WriteData(0x36);
	Display_WriteData(0x0F);
 
	Display_WriteCommand(ILI9341_SLPOUT);  //Exit Sleep
 
	vTaskDelay(120 / portTICK_RATE_MS);
 
	Display_WriteCommand(ILI9341_DISPON);  //Display on
}


void Display_Initialize()
{
	Display_SetupPins();
	
	Display_SetupSPI();
	
	Display_SendInitializationCommands();
}


static void Display_Transmit(const uint8_t* data, size_t length, MessageMode mode)
{
	// Unfortunately esp-idf uses tasks and queues for individual transactions.
	// This creates large waiting periods when trying to send individual messages, as is required when working with 
	// sending interlaced pixel rows. So we use the actual registers here.
	// It's not pretty, but it works.
	
	gpio_set_level(DISPLAY_PIN_DC, mode);	

	spi_dev_t* spi = (spi_dev_t*)DR_REG_SPI3_BASE;
	spicommon_dmaworkaround_idle(DMA_CHANNEL);

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


	spicommon_dmaworkaround_transfer_active(DMA_CHANNEL);
	spicommon_setup_dma_desc_links(DmaDescription, length, data, false);
	spi->user.usr_mosi_highpart = 0;
	spi->dma_out_link.addr = (int)DmaDescription & 0xFFFFF;
	spi->dma_out_link.start = 1;
	spi->user.usr_mosi_highpart = 0;
	spi->user.usr_dummy = 0;

	spi->mosi_dlen.usr_mosi_dbitlen = length * 8;

	spi->miso_dlen.usr_miso_dbitlen= 0;

	spi->user.usr_mosi = 1;
	spi->user.usr_miso = 0;

	spi->cmd.usr=1;
    while(spi->cmd.usr);

}


void Display_WriteCommand(uint8_t command)
{
	Display_Transmit(&command, 1, MESSAGE_COMMAND);
}


void Display_WriteData(uint8_t data)
{
	Display_Transmit(&data, 1, MESSAGE_DATA);
}

static void Display_WriteFragmentedDataArray(const uint8_t* data, size_t length)
{
	int i;
	for (i = 0; i + SPI_MAX_DMA_LEN < length; i += SPI_MAX_DMA_LEN)
		Display_Transmit(&data[i], SPI_MAX_DMA_LEN, MESSAGE_DATA);

	if (i != length)
		Display_Transmit(&data[i], (length - i - 1), MESSAGE_DATA);
}


void Display_WriteDataArray(const uint8_t* data, size_t length)
{
	if (length > SPI_MAX_DMA_LEN)
	{
		Display_WriteFragmentedDataArray(data, length);
		return;
	}

	Display_Transmit(data, length, MESSAGE_DATA);
}
