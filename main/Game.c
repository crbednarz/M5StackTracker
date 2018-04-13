#include "Game.h"
#include "Render.h"
#include "Fluid.h"
#include "Input.h"
#include <string.h>
#include <stdio.h>

static void Game_AddFluid(Game* game, int16_t x, int16_t y, int radius)
{
	size_t index = game->FluidCount;
	game->Fluids[index].X = x;
	game->Fluids[index].Y = y;
	game->Fluids[index].VelocityX = 0;
	game->Fluids[index].VelocityY = 0;
	game->Fluids[index].Radius = radius;
	
	game->FluidCount++;
}


void Game_Initialize(Game* game)
{
	memset(game, 0, sizeof(Game));

	Fluid_Initialize();
	Game_AddFluid(game, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 20);
	
	Game_AddFluid(game, DISPLAY_WIDTH / 2 - 40, DISPLAY_HEIGHT / 2, 30);
	
	Game_AddFluid(game, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 40, 35);
}


void Game_Update(Game* game)
{
	Input_Update();

	int velocityX = 0;
	int velocityY = 0;
	if (Input_IsKeyDown(GAMEPAD_LEFT))
		velocityX -= 1;
	
	if (Input_IsKeyDown(GAMEPAD_RIGHT))
		velocityX += 1;

	if (Input_IsKeyDown(GAMEPAD_UP))
		velocityY -= 1;
	
	if (Input_IsKeyDown(GAMEPAD_DOWN))
		velocityY += 1;

	for (int i = 0; i < game->FluidCount; i++)
	{
		game->Fluids[i].VelocityX += velocityX * (10 - game->Fluids[i].Radius / 5);
		game->Fluids[i].VelocityY += velocityY * (10 - game->Fluids[i].Radius / 5);
	}

	Fluid_Update(game->Fluids, game->FluidCount);
}

void Game_Render(const Game* game)
{
	FrameBuffer* frame = Render_GetWorkingFrame();
	World_Render(&game->World, frame);
	Fluid_Render(game->Fluids, game->FluidCount, frame);
	Render_SwapBuffers();
}