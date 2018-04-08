#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "Display.h"
#include "Game.h"
#include "Motion.h"
#include "Input.h"

Game ActiveGame;

static void DisableAudio()
{
	gpio_set_direction(25, GPIO_MODE_OUTPUT);
	gpio_set_level(25, 0);	
}


static void InitializeSystems()
{
	DisableAudio();
	Motion_Initialize();
	Display_Initialize();
	Input_Initialize();
}

void app_main()
{
	InitializeSystems();

	Game_Initialize(&ActiveGame);
	while (1)
	{
		Game_Update(&ActiveGame);
		Game_Render(&ActiveGame);
	}
}
