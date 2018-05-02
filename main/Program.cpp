#include "Program.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "Display.hpp"

#include <stddef.h>
#include <stdint.h>

void Program::launch()
{
	// The M5Stack has some issues with its speaker making a lot of sounds when using the display.
	// So for the time being we're just disabling it.
	
	gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_25, 0);	

	Display::initialize();

	while (true)
	{

	}
}


// This is to avoid potential calls to new: It'll create a compile-time error.
void *operator_new_blocker();

void *operator new(size_t size)
{
	return operator_new_blocker();
}


extern "C" void app_main()
{
	Program::launch();
}
