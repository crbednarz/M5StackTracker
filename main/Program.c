#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "Display.h"
#include "Game.h"
#include "Motion.h"
#include "Input.h"
#include "I2C.h"
#include "Performance.h"

static Game ActiveGame;
static Stopwatch Timer;

static void DisableAudio()
{
	// The M5Stack has some issues with its speaker making a lot of sounds when using the display.
	// So for the time being we're just disabling it.
	
	gpio_set_direction(25, GPIO_MODE_OUTPUT);
	gpio_set_level(25, 0);	
}

static void InitializeISR()
{
	gpio_install_isr_service(0);
}

static void InitializeSystems()
{
	InitializeISR();
	DisableAudio();
	Render_Initialize();
	I2C_Initialize();
	Motion_Initialize();
	Display_Initialize();
	Input_Initialize();
}


static void RenderThreadEntryPoint()
{
	while (true)
	{
		Render_Present();
	}
}


void app_main()
{
	InitializeSystems();

	xTaskCreatePinnedToCore(&RenderThreadEntryPoint, "RenderThread", 2048, NULL, 25, NULL, 1);

	Game_Initialize(&ActiveGame);
	Stopwatch_Reset(&Timer);
	printf("RAM left %d\n", esp_get_free_heap_size());
	while (1)
	{
		Stopwatch_Start(&Timer);
		Motion_UpdateState();
		Game_Update(&ActiveGame);
		Game_Render(&ActiveGame);
		Stopwatch_End(&Timer);

		if (Timer.RecordCount == 100)
		{
			float averageTimeElapsed = Stopwatch_AverageTime(&Timer);
			printf("Average Frame Time: %ffps\n", 1.0 / averageTimeElapsed);
			Stopwatch_Reset(&Timer);
		}
	}
}
