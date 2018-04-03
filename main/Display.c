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


#if true
#define PIN_MOSI 23
#define PIN_CLK  18
#define PIN_CS   14
#define PIN_DC   27
#else
#define PIN_MOSI 2
#define PIN_CLK  25
#define PIN_CS   26
#define PIN_DC   5
#endif

#define PIN_RST  33
#define PIN_BCKL 32

spi_device_handle_t Display_SpiDeviceHandle;

static void Display_PreTransferCallback(spi_transaction_t* transaction)
{
	int dc = (int)transaction->user;
	gpio_set_level(PIN_DC, dc);	
}

static void Display_SetupPins()
{
	//Initialize non-SPI GPIOs
	gpio_set_direction(PIN_DC, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_RST, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_BCKL, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_CS, GPIO_MODE_OUTPUT);

	gpio_set_level(PIN_CS, 1);
	
	//Reset the display
	gpio_set_level(PIN_RST, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(PIN_RST, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(PIN_BCKL, 0);
	
	
	gpio_set_level(PIN_CS, 0);
	
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
		.gpio_num   = PIN_BCKL,
		.speed_mode = LEDC_HIGH_SPEED_MODE,
		.timer_sel  = LEDC_TIMER_0
	};
	
	ledc_channel_config(&ledcChannelConfig);
	
	ledc_fade_func_install(0);
	
	ledc_set_duty(ledcChannelConfig.speed_mode, ledcChannelConfig.channel, 100);
	ledc_update_duty(ledcChannelConfig.speed_mode, ledcChannelConfig.channel);
}


static void Display_SetupSPI()
{
	spi_bus_config_t spiBugConfig =
	{
		.mosi_io_num = PIN_MOSI,
		.miso_io_num = -1,
		.sclk_io_num = PIN_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};
	
	spi_device_interface_config_t deviceInterfaceConfig =
	{
		.clock_speed_hz = 40000000,
		.mode = 0,
		.spics_io_num = PIN_CS, 
		.queue_size = 7,
		.pre_cb = Display_PreTransferCallback
	};
	
	spi_bus_initialize(VSPI_HOST, &spiBugConfig, 1);
	
	spi_bus_add_device(VSPI_HOST, &deviceInterfaceConfig, &Display_SpiDeviceHandle);
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
	Display_WriteData(0x48);
 
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
	
	uint8_t row[DISPLAY_WIDTH * 2];
	while (true)
	{
		memset(row, 0xFF, DISPLAY_WIDTH * 2);
	
		Display_WriteCommand(ILI9341_CASET);
		Display_WriteData(0);
		Display_WriteData(0);
		Display_WriteData((DISPLAY_WIDTH >> 8) & 0xFF);
		Display_WriteData(DISPLAY_WIDTH & 0xFF);
	
	
		Display_WriteCommand(ILI9341_PASET);
		Display_WriteData(0);
		Display_WriteData(0);
		Display_WriteData((DISPLAY_HEIGHT >> 8) & 0xFF);
		Display_WriteData(DISPLAY_HEIGHT & 0xFF);
	
	
		Display_WriteCommand(ILI9341_RAMWR);
		for (int y = 0; y < DISPLAY_HEIGHT; y++)
			Display_WriteDataArray(row, DISPLAY_WIDTH * 2);
	}
}


void Display_BeginWrite()
{
	
}


void Display_WriteCommand(uint8_t command)
{
	spi_transaction_t transaction;
	memset(&transaction, 0, sizeof(transaction));
	transaction.length = 8;
	transaction.tx_buffer = &command;
	transaction.user = (void*)0;
	spi_device_transmit(Display_SpiDeviceHandle, &transaction);
}


void Display_WriteData(uint8_t data)
{
	spi_transaction_t transaction;
	memset(&transaction, 0, sizeof(transaction));
	transaction.length = 8;
	transaction.tx_buffer = &data;
	transaction.user = (void*)1;
	spi_device_transmit(Display_SpiDeviceHandle, &transaction);
}

void Display_WriteDataArray(uint8_t* data, size_t length)
{
	spi_transaction_t transaction;
	memset(&transaction, 0, sizeof(transaction));
	transaction.length = length * 8;
	transaction.tx_buffer = data;
	transaction.user = (void*)1;
	transaction.flags = 0;
	spi_device_transmit(Display_SpiDeviceHandle, &transaction);
}


void Display_EndWrite()
{
	
}


