#include "Program.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "Display.hpp"
#include "App.hpp"
#include <stddef.h>
#include <stdint.h>


RemoteFrame Program::_remoteFrame;

void Program::launch()
{
	App app;
	// The M5Stack has some issues with its speaker making a lot of sounds when using the display.
	// So for the time being we're just disabling it.
	
	gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_25, 0);	

	Display::initialize();
	
	xTaskCreatePinnedToCore(reinterpret_cast<TaskFunction_t>(&renderThreadEntryPoint), "RenderThread", 2048, NULL, 25, NULL, 1);

	while (true)
	{
		app.update();
		app.render(_remoteFrame.workingFrame());
		_remoteFrame.swap();
	}
}



void Program::renderThreadEntryPoint()
{
	while (true)
	{
		_remoteFrame.upload();
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
