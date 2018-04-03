#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "Display.h"
#include "Game.h"

Game game;

static void DisableAudio()
{
	gpio_set_direction(25, GPIO_MODE_OUTPUT);
	gpio_set_level(25, 0);	
}

void app_main()
{
	DisableAudio();
	Display_Initialize();
	
	Game_Initialize(&game);
	while (1)
	{
		Game_Update(&game);
		Game_Render(&game);
	}
}
